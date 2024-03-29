#include "character.h"
#include "weapon.h"
#include "armor.h"
#include <sstream>

Character::Character(CharacterType type) {
    hidden = false;
    blocked = false;
	init(type);
	fame = 0;
	notoriety = 0;
	discoveries = new vector<Chit*>;
}

Character::Character(string* serialString) {
	hidden = false;
	discoveries = new vector<Chit*>;
	knownPaths = new vector <Path*>;
    int pos = serialString->find(CLASSDELIM);
	string first = serialString->substr(0, pos);
	string second = serialString->substr(pos + 2);

	pos = second.find(VARDELIM);
	first = second.substr(0, pos);
	second= second.substr(pos + 1);
	init((CharacterType) atoi(first.c_str()));

	pos = second.find(VARDELIM);
	first = second.substr(0, pos);
	second = second.substr(pos + 1);
	gold = atoi(second.c_str());
    fame = 0;
    notoriety = 0;
}
void Character::init(CharacterType type) {
	myType = type;
	gold = 10;
    location = NULL;
    knownPaths = new vector <Path*>;
	equipment = new vector<Equipment*>();
	counters = new vector<Counter*>;
	switch (type) {
	case Amazon: initAmazon();
		break;
	case BlackKnight: initBlackknight();
		break;
	case Captain: initCaptain();
		break;
	case Dwarf: initDwarf();
		break;
	case Elf: initElf();
		break;
	case Swordsman: initSwordsman();
		break;
	}
}

int Character::getGold() 
{
	return gold;
}

int Character::getFame()
{
	return fame;
}

int Character::getNotoriety()
{
	return notoriety;
}

void Character::addFame(int incr)
{
	fame += incr;
}

void Character::addNotoriety(int incr)
{
	notoriety += incr;
}

Clearing* Character::getCurrentLocation() {
	return location;
}

vector<Counter*>* Character::getCounters()
{
	return counters;
}
vector<Equipment *>* Character::getEquipment() {
	return equipment;
}

void  Character::moveToClearing(Clearing *destination) {
	location = destination;
}

bool Character::hasAdvantage(CharAdvantages testAdvantage) {
	bool result = false;
	
	if (testAdvantage == advantages[0] || testAdvantage == advantages[1]) {
		result = true;
	}

	return result;
}

CharacterType Character::getType()
{
	return myType;
}

bool Character::isDiscovered(Path* aPath)
{
    return false;
}

void Character::addPath(Path* discoveredPath)
{
    //checking to see if the path is already in this list.
    for(vector<Path*>::iterator it = knownPaths->begin(); it != knownPaths->end(); ++it){
        if(*it == discoveredPath){
            cout << "WARN: Path already known to Character, not adding to list" <<endl;
            return;
        }
    }
    knownPaths->push_back(discoveredPath);
}

bool Character::isHidden()
{
    return hidden;
}

void Character::toggleHide()
{
    hidden = !hidden;
}

void Character::discover(Chit* newDiscovery)
{
	for(vector<Chit*>::iterator it = discoveries->begin(); it != discoveries->end(); ++it){
		if(*it == newDiscovery){
			cout << "WARN: Path already known to Character, not adding to list" <<endl;
            return;
		}
	}
	discoveries->push_back(newDiscovery);
}

bool Character::hasDiscovered(Chit* checkDiscovery)
{
	for(vector<Chit*>::iterator it = discoveries->begin(); it != discoveries->end(); ++it){
		if(*it == checkDiscovery){
			cout << "WARN: Path already known to Character, not adding to list" <<endl;
            return true;
		}
	}
	return false;
}

bool Character::isBlocked()
{
    return blocked;
}

void Character::setBlock(bool newBlock)
{
    blocked = newBlock;
}
char* Character::getTypeString(CharacterType t)
{
	switch (t)
	{
	case Amazon:
		return "Amazon";
		break;
	case BlackKnight:
		return "Black Knight";
		break;
	case Captain:
		return "Captain";
		break;
	case Dwarf:
		return "Dwarf";
		break;
	case Elf:
		return "Elf";
		break;
	case Swordsman:
		return "Swordsman";
		break;
	default:
		return "?";
		break;
	}
}

vector<DwellingType>* Character::getStartLocations(CharacterType t)
{
	vector<DwellingType>* out = new vector<DwellingType>();
	switch (t)
	{
	case Amazon:
		out->push_back(INN);
		break;
	case BlackKnight:
		out->push_back(INN);
		break;
	case Captain:
		out->push_back(INN);
		out->push_back(HOUSE);
		out->push_back(GUARD);
		break;
	case Dwarf:
		out->push_back(INN);
		out->push_back(GUARD);
		break;
	case Elf:
		out->push_back(INN);
		break;
	case Swordsman:
		out->push_back(INN);
		break;
	default:
		break;
	}
	return out;
}

string* Character::serialize() {
	stringstream s;
	s << "Character";
	s << CLASSDELIM;
	s << myType;
	s << VARDELIM;
	s << gold;
	//TODO serialize character equipment

	string *myString = new string(s.str());
	return myString;
}

void Character::addGold(int in) {
	gold += in;
}

//Character inits
void Character::initAmazon() {
	advantages[0] = AIM;
	advantages[1] = STAMINA;
	equipment->push_back(new Weapon(ShortSword, "Short Sword"));
	equipment->push_back(new Armor(Helmet, "Helmet"));
	equipment->push_back(new Armor(Shield, "Shield"));
	equipment->push_back(new Armor(Breastplate, "Breastplate"));

	counters->push_back(new Counter(COUNTER_MOVE, 'M', 4, 0));
	counters->push_back(new Counter(COUNTER_MOVE, 'M', 3, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'L', 4, 0));
	counters->push_back(new Counter(COUNTER_MOVE, 'M', 4, 0));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 5, 0));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 4, 1));
	counters->push_back(new Counter(COUNTER_MOVE, 'M', 3, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 3, 2));
	counters->push_back(new Counter(COUNTER_FIGHT, 'H', 4, 2));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 4, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 3, 2));
	counters->push_back(new Counter(COUNTER_MOVE, 'M', 3, 1));
}

void Character::initBlackknight() {
	advantages[0] = AIM;
	advantages[1] = FEAR;
	equipment->push_back(new Weapon(Mace, "Mace"));
	equipment->push_back(new Armor(Shield, "Shield"));
	equipment->push_back(new Armor(Suit, "Suit"));

	counters->push_back(new Counter(COUNTER_MOVE, 'M', 5, 0));
	counters->push_back(new Counter(COUNTER_MOVE, 'H', 5, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'H', 5, 1));
	counters->push_back(new Counter(COUNTER_MOVE, 'H', 6, 0));
	counters->push_back(new Counter(COUNTER_MOVE, 'M', 4, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'H', 6, 0));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 4, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 4, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 5, 0));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 3, 2));
	counters->push_back(new Counter(COUNTER_FIGHT, 'H', 4, 2));
	counters->push_back(new Counter(COUNTER_MOVE, 'H', 4, 2));
}

void Character::initCaptain() {
	advantages[0] = AIM;
	advantages[1] = REPUTATION;
	equipment->push_back(new Weapon(ShortSword, "Short Sword"));
	equipment->push_back(new Armor(Helmet, "Helmet"));
	equipment->push_back(new Armor(Shield, "Shield"));
	equipment->push_back(new Armor(Breastplate, "Breastplate"));

	counters->push_back(new Counter(COUNTER_MOVE, 'M', 4, 1));
	counters->push_back(new Counter(COUNTER_MOVE, 'M', 5, 0));
	counters->push_back(new Counter(COUNTER_FIGHT, 'H', 5, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 5, 0));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 3, 2));
	counters->push_back(new Counter(COUNTER_MOVE, 'M', 3, 2));
	counters->push_back(new Counter(COUNTER_MOVE, 'M', 4, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'H', 5, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 4, 1));
	counters->push_back(new Counter(COUNTER_MOVE, 'M', 4, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'H', 6, 0));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 4, 1));
}

void Character::initDwarf() {
	advantages[0] = SHORTLEGS;
	advantages[1] = CAVEKNOWLEGDE;
	equipment->push_back(new Weapon(GreatAxe, "Great Axe"));
	equipment->push_back(new Armor(Helmet, "Helmet"));

	counters->push_back(new Counter(COUNTER_DUCK, 'T', 3, 1));
	counters->push_back(new Counter(COUNTER_MOVE, 'H', 6, 0));
	counters->push_back(new Counter(COUNTER_FIGHT, 'H', 5, 1));
	counters->push_back(new Counter(COUNTER_MOVE, 'T', 6, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'H', 6, 0));
	counters->push_back(new Counter(COUNTER_FIGHT, 'H', 4, 2));
	counters->push_back(new Counter(COUNTER_MOVE, 'T', 5, 2));
	counters->push_back(new Counter(COUNTER_FIGHT, 'T', 5, 2));
	counters->push_back(new Counter(COUNTER_FIGHT, 'T', 5, 1));
}

void Character::initElf() {
	advantages[0] = ELUSIVNESS;
	advantages[1] = ARCHER;
	equipment->push_back(new Weapon(LightBow, "Light Bow"));

	counters->push_back(new Counter(COUNTER_MOVE, 'L', 3, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'L', 3, 1));
	counters->push_back(new Counter(COUNTER_MOVE, 'L', 2, 1));
	counters->push_back(new Counter(COUNTER_MOVE, 'M', 4, 0));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 3, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 4, 0));
}

void Character::initSwordsman() {
	advantages[0] = BARTER;
	advantages[1] = CLEAVER;
	equipment->push_back(new Weapon(ThrustingSword, "Thrusting Sword"));

	counters->push_back(new Counter(COUNTER_MOVE, 'L', 4, 0));
	counters->push_back(new Counter(COUNTER_MOVE, 'L', 3, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'L', 3, 1));
	counters->push_back(new Counter(COUNTER_MOVE, 'L', 3, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'L', 2, 2));
	counters->push_back(new Counter(COUNTER_MOVE, 'L', 2, 2));
	counters->push_back(new Counter(COUNTER_MOVE, 'M', 4, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 4, 1));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 3, 2));
	counters->push_back(new Counter(COUNTER_FIGHT, 'L', 4, 0));
	counters->push_back(new Counter(COUNTER_FIGHT, 'M', 5, 0));
	counters->push_back(new Counter(COUNTER_FIGHT, 'L', 2, 2));
}

CharAdvantages Character::getAdvantage(bool firstAdvantage)
{
	if(firstAdvantage)
		return advantages[0];
	return advantages[1];
}