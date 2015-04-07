#ifndef COMBAT_MANAGER_H
#define COMBAT_MANAGER_H

#include "shared.h"
#include "character.h"
#include "counter.h"
#include "equipment.h"
#include "weapon.h"
#include "armor.h"
#include "commonlibrary_global.h"


class COMMONLIBRARY_EXPORT CombatManager
{
public:
    CombatManager(Character* attacker, Character* defender);
	
	Character* getAttacker();
	Character* getDefender();
	CombatPhaseType getCurrentPhase();
	
	//returns true if attacker won, false if defender won step
	void runEncounter();
	void runMelee();
	Character* getPhaseWinner();
	bool EncounterVictorRun();
	void submitEncounter(Character* combatant, bool run,  Counter* counterUsed);
	void submitMelee(Character* combatant, Counter* fightCounter, CombatFightType fightType, Counter* moveCounter, CombatMoveType moveType, CombatShieldBlock shieldBlock);

private:
	Character* attacker;
	Character* defender;

	Counter* attackerMoveCounter;
	Counter* defenderMoveCounter;

	Counter* attackerFightCounter;
	Counter* defenderFightCounter;

	CombatFightType attackerFightType;
	CombatFightType defenderFightType;

	CombatMoveType attackerMoveType;
	CombatMoveType defenderMoveType;

	CombatShieldBlock attackerBlock;
	CombatShieldBlock defenderBlock;

	void setupFightCounter(Character* combatant, Counter* fCounter);
	void setupMoveCounter(Character* combatant, Counter* mCounter);
	int getWeaponLength(Weapon* weapon);

	void fight(Character* combatant);


	bool stageWinAttacker;

	bool attackerFlee;
	bool defenderFlee;

	bool firstMelee;

	CombatPhaseType currentPhase;
};

#endif //COUNTER_H