#ifndef SHARED_H
#define SHARED_H

#define GAMEPORT 42730
#define CONNECTED_LENGTH 6
#define DECLINECONN "connection declined"
#define ACCEPTCONN "connection accepted"
#define MAXPLAYERS 6
#define MAXDWELLINGS 4

#define CLASSDELIM "**"
#define VARDELIM "^"
#define LISTDELIM "+"

#include <iostream>
#include <string>
#include <ctime>
#include "commonlibrary_global.h"

using namespace std;

enum COMMONLIBRARY_EXPORT Direction 
{
    EDGE_NONE =-1, 
    EDGE_A=0, 
    EDGE_B, 
    EDGE_C, 
    EDGE_D, 
    EDGE_E, 
    EDGE_F
};

enum COMMONLIBRARY_EXPORT CharAdvantages
{
	AIM,
	REPUTATION,
	BARTER,
	CLEAVER,
	STAMINA,
	SHORTLEGS,
	CAVEKNOWLEGDE,
	ELUSIVNESS,
	ARCHER,
	FEAR
};

enum COMMONLIBRARY_EXPORT CharacterType
{
	NullCharacter=-1,
	Amazon=0,
	BlackKnight,
	Captain,
	Dwarf,
	Elf,
	Swordsman
};

enum COMMONLIBRARY_EXPORT ArmorType
{
	Helmet,
	Breastplate,
	Shield,
	Suit
};

enum COMMONLIBRARY_EXPORT WeaponType
{
	MediumBow,
	LightBow,
	CrossBow,
	Spear,
	Staff,
	GreatSword,
	BroadSword,
	MorningStar,
	GreatAxe,
	ThrustingSword,
	ShortSword,
	Axe,
	Mace
};

enum COMMONLIBRARY_EXPORT AttackType
{
	Missile,
	Striking
};

enum COMMONLIBRARY_EXPORT ActionType
{
	NoAction=-1,
	MoveAction=0,
	SearchAction,
	TradeAction,
	HideAction
};

enum COMMONLIBRARY_EXPORT PhaseType
{
	MovePhase,
	BasicPhase,
	SunlightPhase,
	HidePhase
};

enum COMMONLIBRARY_EXPORT TimeOfDay
{
    BIRDSONG,
    SUNRISE,
    DAYLIGHT,
    SUNSET,
    EVENING,
    MIDNIGHT
};

enum COMMONLIBRARY_EXPORT ClearingType
{
    WOODS,
    CAVES,
    MOUNTAIN
};

enum COMMONLIBRARY_EXPORT DwellingType
{
	CHAPEL,
	GUARD,
	HOUSE,
	INN
};

enum COMMONLIBRARY_EXPORT TreasureType
{
	SMALL,
    LARGE,
    GREAT
};

enum COMMONLIBRARY_EXPORT SearchType
{
	PEER,
	LOCATE,
	LOOT
};

enum COMMONLIBRARY_EXPORT TileType
{
	TILE_VALLEY = 0,
	TILE_WOODS,
	TILE_CAVES,
	TILE_MOUNTAIN
};

enum COMMONLIBRARY_EXPORT ChitType
{
	CHIT_SOUND = 0,
	CHIT_SITE,
	CHIT_WARNING,
	CHIT_LOST
};

enum COMMONLIBRARY_EXPORT PathType
{
	NORMAL,
	SECRET,
	HIDDEN
};
#endif