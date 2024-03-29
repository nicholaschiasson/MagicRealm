#include "combatManager.h"

CombatManager::CombatManager(Character* a, Character* d){
	attacker = a;
	defender = d;
	currentPhase = PHASE_ENCOUNTER;
	attackerMoveCounter = NULL;
	defenderMoveCounter = NULL;
	attackerFightCounter = NULL;
	defenderFightCounter = NULL;

	attackerFlee = false;
	defenderFlee = false;
	firstMelee = true;
	stageWinAttacker = false;

	attackerResult = ACTION_MISS;
	defenderResult = ACTION_MISS;
    attackerWounds = 0;
    defenderWounds = 0;

	doubleMiss = 0;
	attackerDamagedArmor = NULL;
	defenderDamagedArmor = NULL;
}

Character* CombatManager::getAttacker(){
	return attacker;
}

Character* CombatManager::getDefender(){
	return defender;
}

void CombatManager::submitEncounter(Character* combatant,  bool run, Counter* counterUsed)
{
	if(combatant == attacker){
		attackerFlee = run;
	}
	else if (combatant == defender){
		defenderFlee = run;
	}
	else{
		cout << "ERR: CombatManager::submitEncounter ::: Combatant not Attacker or Defender" << endl;
	}
	
	setupMoveCounter(combatant, counterUsed);
}

CombatPhaseType CombatManager::getCurrentPhase()
{
	return currentPhase;
}
void CombatManager::runEncounter()
{
    if(currentPhase != PHASE_ENCOUNTER){
        cout << "ERR: Not in Encounter Phase" << endl;
        return;
    }

	attackerResult = ACTION_MISS;
	defenderResult = ACTION_MISS;
	attackerDamagedArmor = NULL;
	defenderDamagedArmor = NULL;

	if(attackerMoveCounter == NULL){
		stageWinAttacker = false;
		currentPhase = PHASE_MELEE;
		return;
	}
	else if(defenderMoveCounter == NULL){
		stageWinAttacker = true;
		currentPhase = PHASE_MELEE;
		return;
	}

	if(attackerMoveCounter->getSpeed() == defenderMoveCounter->getSpeed()){
		if(!attackerFlee)
			stageWinAttacker = true;
		if(!defenderFlee)
			stageWinAttacker = false;
	}
	//if attacker Move is faster then the attacker won.
	(attackerMoveCounter->getSpeed() < defenderMoveCounter->getSpeed()) ? stageWinAttacker = true : stageWinAttacker = false;
	currentPhase = EncounterVictorRun() ? PHASE_FLEE : PHASE_MELEE;
}

 Character* CombatManager::getPhaseWinner()
{
	return stageWinAttacker ? attacker : defender;
}

bool CombatManager::EncounterVictorRun()
{
	return stageWinAttacker ? attackerFlee : defenderFlee;
}

void CombatManager::submitMelee(Character* combatant, Counter* fCounter, CombatFightType fType, Counter* mCounter, CombatMoveType mType, CombatShieldBlock sBlock)
{
	setupFightCounter(combatant, fCounter);
	setupMoveCounter(combatant, mCounter);

	if(combatant == attacker){
		attackerMoveType = mType;
		attackerFightType = fType;
		attackerBlock = sBlock;
		return;
	}
	if(combatant == defender){
		defenderMoveType = mType;
		defenderFightType = fType;
		defenderBlock = sBlock;
		return;
	}
}

void CombatManager::setupMoveCounter(Character* combatant, Counter* counterUsed){
	bool isAttacker = false;

	vector<Counter *>* characterCounters = combatant->getCounters();
	if(combatant == attacker)
		isAttacker = true;
	if(combatant == defender)
		isAttacker = false;

	if(counterUsed == NULL || counterUsed->getType() != COUNTER_MOVE){
		cout << "Combatant used invalid counter for moveCounter" <<endl;
		isAttacker? attackerMoveCounter = NULL : defenderMoveCounter = NULL;
        return;
	}
	for(vector<Counter*>::iterator iter = characterCounters->begin(); iter != characterCounters->end(); ++iter){
		if((*iter)->getID() == counterUsed->getID() && counterUsed->isAvailable()){
			isAttacker? attackerMoveCounter = (*iter) : defenderMoveCounter = (*iter);
		}
	}
}

void CombatManager::setupFightCounter(Character* combatant, Counter* counterUsed){
	bool isAttacker = false;

	vector<Counter *>* characterCounters = combatant->getCounters();
	if(combatant == attacker)
		isAttacker = true;
	if(combatant == defender)
		isAttacker = false;

	if(counterUsed == NULL || counterUsed->getType() != COUNTER_FIGHT){
		cout << "Combatant used invalid counter for fightCounter" <<endl;
		isAttacker? attackerFightCounter = NULL : defenderFightCounter = NULL;
		return;
	}
	for(vector<Counter*>::iterator iter = characterCounters->begin(); iter != characterCounters->end(); ++iter){
		if((*iter)->getID() == counterUsed->getID() && counterUsed->isAvailable()){
			isAttacker? attackerFightCounter = (*iter) : defenderFightCounter = (*iter);
		}
	}
}

void CombatManager::runMelee()
{
    if(currentPhase != PHASE_MELEE){
        cout << "ERR: Not in Melee Phase" <<endl;
        return;
    }
	bool attackerFirst = true;

	bool firstHitSecond = false;
	bool secondHitFirst = false;

	Counter* firstMoveCounter;
	Counter* secondMoveCounter;

	Counter* firstFightCounter;
	Counter* secondFightCounter;

	CombatFightType firstFightType;
	CombatFightType secondFightType;

	CombatMoveType firstMoveType;
	CombatMoveType secondMoveType;

	CombatShieldBlock firstBlock;
	CombatShieldBlock secondBlock;
	vector<Equipment*>* firstEquipment;
	vector<Equipment*>* secondEquipment;

	Equipment* damagedArmor;


	int attackerLength;
	int defenderLength;
	vector<Equipment*>* attackerEquipment = attacker->getEquipment();
	vector<Equipment*>* defenderEquipment = defender->getEquipment();
	//getting weapon lengths
	for(vector<Equipment*>::iterator iter = attackerEquipment->begin(); iter != attackerEquipment->end(); ++iter){
			if((*iter)->getType() == EQUIPMENT_WEAPON){
				attackerLength = (*iter)->getLength();
				break;
			}
		}
	for(vector<Equipment*>::iterator iter = defenderEquipment->begin(); iter != defenderEquipment->end(); ++iter){
			if((*iter)->getType() == EQUIPMENT_WEAPON){
				defenderLength = (*iter)->getLength();
				break;
			}
		}
	//determining who attacks first.
	if(firstMelee){
		firstMelee = false;
		if(attackerLength == defenderLength)
			attackerFirst = (attackerFightCounter->getSpeed() <= defenderFightCounter->getSpeed());
		else
			attackerFirst = (attackerLength > defenderLength);
	}
	else{
        if(attackerFightCounter == NULL)
            attackerFirst = false;
        else if(defenderFightCounter == NULL)
            attackerFirst = true;
		else if(attackerFightCounter->getSpeed() == defenderFightCounter->getSpeed())
			attackerFirst = (attackerLength < defenderLength);
		else
			attackerFirst = (attackerFightCounter->getSpeed() < defenderFightCounter->getSpeed());
	}

	//Ordering the attackers
	if(attackerFirst){
		cout << "Attacker makes the first move" <<endl;
		firstMoveCounter = attackerMoveCounter;
		secondMoveCounter = defenderMoveCounter;

		firstFightCounter = attackerFightCounter;
		secondFightCounter = defenderFightCounter;

		firstFightType = attackerFightType;
		secondFightType = defenderFightType;

		firstMoveType = attackerMoveType;
		secondMoveType = defenderMoveType;

		firstBlock = attackerBlock;
		secondBlock = defenderBlock;

		firstEquipment = attackerEquipment;
		secondEquipment = defenderEquipment;
	}
	else{
		cout << "Defender makes the first move" <<endl;
		firstMoveCounter = defenderMoveCounter;
		secondMoveCounter = attackerMoveCounter;

		firstFightCounter = defenderFightCounter;
		secondFightCounter = attackerFightCounter;

		firstFightType = defenderFightType;
		secondFightType = attackerFightType;

		firstMoveType = defenderMoveType;
		secondMoveType = attackerMoveType;

		firstBlock = defenderBlock;
		secondBlock = attackerBlock;

		firstEquipment = defenderEquipment;
		secondEquipment = attackerEquipment;
	}
	//now execute the combat
    if(firstMoveCounter != NULL){
	    //executing first person to go
	    firstHitSecond = (secondMoveCounter == NULL || (firstFightCounter->getSpeed() < secondMoveCounter->getSpeed())); //checking auto hit
	    if(!firstHitSecond) //check lineup
		    firstHitSecond = hitScan(firstFightType, secondMoveType);

	    //if hit calculate damage done.

	    if(firstHitSecond){
			damagedArmor = wasBlocked(secondEquipment, secondBlock, firstFightType);
		    if(damagedArmor == NULL){
			    if(attackerFirst)
				    defenderResult = ACTION_WOUND;
			    else
				    attackerResult = ACTION_WOUND;
			    cout << "First Player Cut off the Head of Second Player" << endl;
		    }
		    else{
			    if(attackerFirst){
					defenderDamagedArmor = damagedArmor;
				    defenderResult = ACTION_DAMAGED;
				}
			    else{
					attackerDamagedArmor = damagedArmor;
				    attackerResult = ACTION_DAMAGED;
				}
		    }
	    }
	    else{
		    if(attackerFirst)
				    defenderResult = ACTION_MISS;
			    else
				    attackerResult = ACTION_MISS;
	    }
    }
    else{
        if(attackerFirst)
	        defenderResult = ACTION_MISS;
		 else
		    attackerResult = ACTION_MISS;
    }
	
	damagedArmor = NULL;
	//executing second person to go
    if(secondFightCounter != NULL){
	    secondHitFirst = (firstMoveCounter == NULL || (secondFightCounter->getSpeed() < firstMoveCounter->getSpeed())); //checking auto hit
	    if(!secondHitFirst) //check lineup
		    secondHitFirst = hitScan(secondFightType, firstMoveType);

	    //if hit calculate damage done.

	    if(secondHitFirst){
			damagedArmor = wasBlocked(firstEquipment, firstBlock, secondFightType);
		    if(damagedArmor == NULL){
			    if(!attackerFirst)
				    defenderResult = ACTION_WOUND;
			    else
				    attackerResult = ACTION_WOUND;
		    }
		    else{
			    if(!attackerFirst){
					defenderDamagedArmor = damagedArmor;
				    defenderResult = ACTION_DAMAGED;
				}
			    else{
					attackerDamagedArmor = damagedArmor;
				    attackerResult = ACTION_DAMAGED;
				}
		    }
	    }
	    else{
		    if(!attackerFirst)
				    defenderResult = ACTION_MISS;
			    else
				    attackerResult = ACTION_MISS;
	    }
    }
    else{
        if(!attackerFirst)
				    defenderResult = ACTION_MISS;
			    else
				    attackerResult = ACTION_MISS;
    }
}

int CombatManager::getWounds(Character * c)
{
	if(c == attacker)
		return attackerWounds;
	if(c == defender)
		return defenderWounds;

	cout << "ERR: CombatManager::getWounds ::: input character does not match combatants" <<endl;
	return 0;
}

CombatActionType CombatManager::getResult(Character* c)
{
	if(c == attacker)
		return attackerResult;
	if(c == defender)
		return defenderResult;

	cout << "ERR: CombatManager::getResult ::: input character does not match combatants" <<endl;
	return ACTION_MISS; //probably improper
}

void CombatManager::woundCounter(Counter* aCounter)
{
    aCounter->wound();
}

void CombatManager::runResolve()
{
    int attackerHealth = 0;
    int defenderHealth = 0;
    //note Fight Counters should never be NULL if ACTION_WOUND appears.. i hope
    if(attackerResult == ACTION_WOUND){
        attackerWounds = getValue(defenderFightCounter->getSize());
        for(vector<Counter*>::iterator iter = attacker->getCounters()->begin(); iter!= attacker->getCounters()->end(); ++iter){
            if((*iter)->isAvailable())
                ++attackerHealth;
        }
        if(attackerWounds > attackerHealth){
            attackerResult = ACTION_DEAD;
			cout << "Defender Player Cut off the Head of Attacking Player" << endl;
        }
    }
    else
        attackerWounds = 0;

    if(defenderResult == ACTION_WOUND){
        defenderWounds = getValue(attackerFightCounter->getSize());
        for(vector<Counter*>::iterator iter = defender->getCounters()->begin(); iter!= defender->getCounters()->end(); ++iter){
            if((*iter)->isAvailable())
                ++defenderHealth;
        }
        if(defenderWounds > defenderHealth){
			cout << "First Player Cut off the Head of Second Player" << endl;
            defenderResult = ACTION_DEAD;
		}
    }
    else
        defenderWounds = 0;

    if(attackerResult != ACTION_DEAD && defenderResult != ACTION_DEAD)
    {
		if(attackerResult == ACTION_MISS && defenderResult == ACTION_MISS){
			doubleMiss++;
			if(doubleMiss > 1)
				currentPhase = PHASE_MISSED;
			else
				currentPhase = PHASE_ENCOUNTER;
		}
		else{
			doubleMiss = 0;
			currentPhase = PHASE_ENCOUNTER;
		}
    }
	else
		currentPhase = PHASE_DEAD;
}

int CombatManager::getValue(char weight)
{
    switch(weight){
    case 'L': return 1;
    case 'M': return 2;
    case 'H': return 3;
    case 'T': return 4;
    default: return 0;
    }
}
//TODO runRESOLVE PHASE!!!!!
//TODO MISSILE COMBAT HANDLING
//TODO CHIT WOUNDING PAGE 66/67

bool CombatManager::hitScan(CombatFightType firstFight, CombatMoveType secondMove)
{
	if(firstFight == FIGHT_THRUST && secondMove == MOVE_CHARGE)
		return true;
	if(firstFight == FIGHT_SWING && secondMove == MOVE_DODGE)
		return true;
	if(firstFight == FIGHT_SMASH && secondMove == MOVE_DUCK)
		return true;
	else
		return false;
}

Equipment* CombatManager::wasBlocked(vector<Equipment*>* targetEquipment, CombatShieldBlock shielded, CombatFightType fightType)
{
	bool hasArmor = false;
	Equipment* shield = NULL;
	Equipment* breastplate = NULL;
	Equipment* helmet = NULL;
	Equipment* suit = NULL;
	
	//checking shield
	for(vector<Equipment *>::iterator iter = targetEquipment->begin(); iter != targetEquipment->end(); ++iter){
		if((*iter)->getName() == "Shield"){
				hasArmor = true;
				shield = (*iter);
				break;
		}
	}
	if(hasArmor && !shield->isDamaged()){
		if((shielded == SHIELD_THRUST && fightType == FIGHT_THRUST)
			|| (shielded == SHIELD_SWING && fightType == FIGHT_SWING)
			|| (shielded == SHIELD_SMASH && fightType == FIGHT_SMASH)){
			shield->setDamaged(true);
			cout << "Shield has been damaged" << endl;
			return (Armor*) shield;
		}
	}
	hasArmor = false;
	//checking breastplate
	for(vector<Equipment *>::iterator iter = targetEquipment->begin(); iter != targetEquipment->end(); ++iter){
		if((*iter)->getName() == "Breastplate"){
				hasArmor = true;
				breastplate = (*iter);
				break;
		}
	}
	if(hasArmor && !breastplate->isDamaged()){
		if(fightType == FIGHT_THRUST || fightType == FIGHT_SWING){
			breastplate->setDamaged(true);
			cout << "Breastplate has been damaged" << endl;
			return breastplate;
		}
	}
	hasArmor = false;

	//checking helmet
	for(vector<Equipment *>::iterator iter = targetEquipment->begin(); iter != targetEquipment->end(); ++iter){
		if((*iter)->getName() == "Helmet"){
				hasArmor = true;
				helmet = (*iter);
				break;
		}
	}
	if(hasArmor && !helmet->isDamaged()){
		if(fightType == FIGHT_SMASH){
			helmet->setDamaged(true);
			cout << "Helmet has been damaged" << endl;
			return helmet;
		}
	}
	hasArmor = false;

	//checking suit of armor
	for(vector<Equipment *>::iterator iter = targetEquipment->begin(); iter != targetEquipment->end(); ++iter){
		if((*iter)->getName() == "Suit"){
				hasArmor = true;
				suit = (*iter);
				break;
		}
	}
	if(hasArmor && !suit->isDamaged()){
			suit->setDamaged(true);
			cout << "SuitOfArmor has been damaged" << endl;
			return suit;
	}

	//no armor to protect you son :(

	return NULL;
}

Equipment* CombatManager::getDamaged(Character* c)
{
	if(c == attacker)
		return attackerDamagedArmor;
	if(c == defender)
		return defenderDamagedArmor;

	cout << "ERR: CombatManager::getDamaged ::: input character does not match combatants" <<endl;
	return NULL;
}