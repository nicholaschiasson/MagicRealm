#include "combatdialog.h"
#include <QDebug>
#include <QtNetwork>

CombatDialog::CombatDialog(Character* myChar, Character* enemyChar, QPixmap* myPix, QPixmap* enemyPix,
						   CombatState state, ServerCommThread* serv, QWidget* parentWindow)
{
	myCharacter = myChar;
	enemyCharacter = enemyChar;
	server = serv;
	combatState = state;
	formsToFill[0] = 1;
	formsToFill[1] = 1;
	formsToFill[2] = 1;
	defaultMoveCounter = false;
	defaultFightCounter = false;
	defaultShieldType = false;

	ui.setupUi(this);
	ui.myCharacter->setPixmap(*myPix);
	ui.enemyCharacter->setPixmap(*enemyPix);

	ui.moveGroupBox->setEnabled(true);
	ui.moveGroupBox->setVisible(true);
	vector<Counter*>* counters = myCharacter->getCounters();
	for (vector<Counter*>::iterator it = counters->begin(); it != counters->end(); ++it)
	{
		if ((*it)->getType() == COUNTER_MOVE && (*it)->isAvailable())
		{
			formsToFill[0] = 0;
			ui.submitButton->setEnabled(false);
			QString cntr;
			cntr.sprintf("%c%d", (*it)->getSize(), (*it)->getSpeed());
			for (int i = 0; i < (*it)->getFatigue(); i++)
			{
				cntr.append("*");
			}
			QListWidgetItem* item = new QListWidgetItem(cntr);
			item->setData(Qt::UserRole, (*it)->getID());
			ui.moveCounterList->addItem(item);
		}
	}

	if (formsToFill[0] == 1)
	{
		defaultMoveCounter = true;
	}
	
	ui.submitButton->setEnabled(false);

	if (combatState == ENCOUNTER)
	{
		ui.encounter_runGroupBox->setEnabled(true);
		ui.encounter_runGroupBox->setVisible(true);
		ui.submitButton->setText("Submit Encounter Decisions");

		ui.melee_fightGroupBox->setEnabled(false);
		ui.melee_fightGroupBox->setVisible(false);
		ui.melee_moveTypeGroupBox->setEnabled(false);
		ui.melee_moveTypeGroupBox->setVisible(false);
		ui.melee_fightTypeGroupBox->setEnabled(false);
		ui.melee_fightTypeGroupBox->setVisible(false);
		ui.melee_blockTypeGroupBox->setEnabled(false);
		ui.melee_blockTypeGroupBox->setVisible(false);
	}
	else if (combatState == MELEE)
	{
		QRect geo = ui.moveGroupBox->geometry();
		ui.moveGroupBox->setGeometry(50, geo.y(), geo.width(), geo.height());
		ui.melee_fightGroupBox->setEnabled(true);
		ui.melee_fightGroupBox->setVisible(true);
		for (vector<Counter*>::iterator it = counters->begin(); it != counters->end(); ++it)
		{
			if ((*it)->getType() == COUNTER_FIGHT && (*it)->isAvailable())
			{
				formsToFill[1] = 0;
				ui.submitButton->setEnabled(false);
				QString cntr;
				cntr.sprintf("%c%d", (*it)->getSize(), (*it)->getSpeed());
				for (int i = 0; i < (*it)->getFatigue(); i++)
				{
					cntr.append("*");
				}
				QListWidgetItem* item = new QListWidgetItem(cntr);
				item->setData(Qt::UserRole, (*it)->getID());
				ui.fightCounterList->addItem(item);
			}
		}
		if (formsToFill[1] == 1)
		{
			defaultFightCounter = true;
		}
		ui.melee_moveTypeGroupBox->setEnabled(true);
		ui.melee_moveTypeGroupBox->setVisible(true);
		ui.melee_fightTypeGroupBox->setEnabled(true);
		ui.melee_fightTypeGroupBox->setVisible(true);
		ui.melee_blockTypeGroupBox->setVisible(true);
		ui.melee_blockTypeGroupBox->setEnabled(false);
		ui.melee_blockTypeGroupBox->setTitle("No shield");
		vector<Equipment*>* equpmnt = myCharacter->getEquipment();
		for (vector<Equipment*>::iterator it = equpmnt->begin(); it != equpmnt->end(); ++it)
		{
			if ((*it)->getName() == "Shield")
			{
				formsToFill[2] = 0;
				ui.submitButton->setEnabled(false);
				ui.melee_blockTypeGroupBox->setEnabled(true);
				ui.melee_blockTypeGroupBox->setTitle("Select Shield Block");
				break;
			}
		}
		if (formsToFill[2] == 1)
		{
			defaultShieldType = true;
		}
		ui.submitButton->setText("Submit Melee Decisions");
		
		ui.encounter_runGroupBox->setEnabled(false);
		ui.encounter_runGroupBox->setVisible(false);
	}
}


CombatDialog::~CombatDialog()
{
}

void CombatDialog::on_submitButton_clicked()
{
	if (combatState == ENCOUNTER)
	{
		subEncounter(myCharacter->getType(), ui.yesRun->isChecked(), ui.moveCounterList->currentItem()->data(Qt::UserRole).toInt());
	}
	else if (combatState == MELEE)
	{
		int moveCounter;
		CombatMoveType moveType;
		int fightCounter;
		CombatFightType fightType;
		CombatShieldBlock blockType;

		if (defaultMoveCounter) moveCounter = -1;
		else moveCounter = ui.moveCounterList->currentItem()->data(Qt::UserRole).toInt();

		moveType = (CombatMoveType)ui.moveTypeList->currentRow();

		if (defaultFightCounter) fightCounter = -1;
		else fightCounter = ui.fightCounterList->currentItem()->data(Qt::UserRole).toInt();

		fightType = (CombatFightType)ui.fightTypeList->currentRow();

		if (defaultShieldType) blockType = SHIELD_NONE;
		else blockType = (CombatShieldBlock)ui.blockTypeList->currentRow();

		subMelee(myCharacter->getType(), fightCounter, fightType, moveCounter, moveType, blockType);
	}
}

void CombatDialog::on_moveCounterList_currentRowChanged(int row)
{
	formsToFill[0] = 1;
	if (formsToFill[0] == 1 && formsToFill[1] == 1 && formsToFill[2] == 1)
	{
		ui.submitButton->setEnabled(true);
	}
}

void CombatDialog::on_fightCounterList_currentRowChanged(int row)
{
	formsToFill[1] = 1;
	if (formsToFill[0] == 1 && formsToFill[1] == 1 && formsToFill[2] == 1)
	{
		ui.submitButton->setEnabled(true);
	}
}

void CombatDialog::on_blockTypeList_currentRowChanged(int row)
{
	formsToFill[2] = 1;
	if (formsToFill[0] == 1 && formsToFill[1] == 1 && formsToFill[2] == 1)
	{
		ui.submitButton->setEnabled(true);
	}
}

void CombatDialog::subEncounter(CharacterType character, bool run, int counter)
{
	QString serializedCombat;
	serializedCombat.sprintf("SubEncounter%s%d%s%d%s%d", CLASSDELIM, (int)character, VARDELIM, (int)run, VARDELIM, counter);
	server->writeMessage(&serializedCombat);
	done(1);
}

void CombatDialog::subMelee(CharacterType character ,int fightCounter, CombatFightType fightType, int moveCounter, 
							CombatMoveType moveType, CombatShieldBlock blockType)
{
	QString serializedCombat;
	serializedCombat.sprintf("SubMelee%s%d%s%d%s%d", CLASSDELIM, (int)character, VARDELIM, fightCounter, VARDELIM, fightType,
		VARDELIM, moveCounter, VARDELIM, moveType, VARDELIM, blockType);
	server->writeMessage(&serializedCombat);
	done(1);
}