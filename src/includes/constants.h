/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declarations of extern extern constants and enumerated types
*/

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "common_libs.h"

static const uint32_t DAYSECONDS = 86400;	//!< Number of seconds in a day. To don't repeat it
static const uint32_t DAYMSECONDS = 86400000;	//!< Number of milliseconds in a day. To don't repeat it
static const uint16_t SECS = 1000;		//!< Used to convert seconds to milliseconds ( getclock() returns msecs )
static const uint32_t MINUTS = 60*SECS;		//!< Used to convert minutes to milliseconds ( getclock() returns msecs )
static const uint32_t HOURS = 60*MINUTES;	//!< Used to convert hours to milliseconds ( getclock() returns msecs )

static const uint16_t T2A = 0x0001;
static const uint16_t LBR = 0x0002;

//@{
//! \name Colors
	static const uint16_t COLOR_LIGHTBLUE	= 0x0489;
	static const uint16_t COLOR_VIOLET	= 0x000D;
	static const uint16_t COLOR_WHITE	= 0x0481;
	static const uint16_t COLOR_BLACK	= 0x0001;
	static const uint16_t COLOR_NONE	= 0x0000;
//@}

//@{
//! \name Human bodies
	static const uint16_t bodyMale		= 0x0190;	//!< Male human body
	static const uint16_t bodyFemale	= 0x0191;	//!< Female body
	static const uint16_t bodyMaleDead	= 0x0192;	//!< Dead male body
	static const uint16_t bodyFemaleDead	= 0x0193;	//!< Dead female body
	static const uint16_t bodyGM		= 0x03DB;	//!< GM Staff body
//@}

//@{
//! \name Some items
	static const uint16_t ITEMID_DEATHSHROUD= 0x204E;
	static const uint16_t ITEMID_CORPSEBASE	= 0x2006;
	static const uint16_t ITEMID_BANKBOX	= 0x09AB;
	static const uint16_t ITEMID_GOLD	= 0x0EED;
//@}

static const uint16_t SPIRITSPEAKTIMER	= 30;
			//!< spiritspeak lasts at least 30 seconds, additional time is calculated by Skill & INT
static const uint16_t VERY_VERY_FAR = 50000; //!< Used as distance between invalid objects

static const int8_t illegal_z = -128;	//!< An illegal z-value to return in case of error

static const uint16_t XBORDER = 200;	//!< Border from x-limit
static const uint16_t YBORDER = 200;	//!< Border from y-limit

static const char strNull[] = "";

//@{
/*!
\name Items identifiers

These constants are the identifiers for some items of general use.
*/
	static const char strGoldId[] = "item_gold";
	static const char strBackpackId[] = "item_backpack";
	static const char strEmptyBottleId[] = "item_bottle_empty";
//@}

//@{
/*!
\name Made words

These are constants used by sSkillInfo struct to define the made word for the
skills.
*/
	static const char strMixed[] = "mixed";
	static const char strMade[] = "made";
	static const char strForged[] = "forged";
	static const char strBowcrafted[] = "bowcrafted";
	static const char strWrote[] = "wrote";
	static const char strCooked[] = "cooked";
	static const char strSewn[] = "sewn";
	static const char strSmelted[] = "smelted";
	static const char strEvoked[] = "evoked";
	static const char strFished[] = "fished";
	static const char strPlayed[] = "played";
	static const char strTamed[] = "tamed";
//@}

// Miscellaneous constants
enum
{
	MAXLOOPS=66666,		//!< max cycles in a loop
	MaxZstep=9,

	MAXIMUM = 1024,		//!< Maximum open connections to server
	MAXCLIENT = MAXIMUM +1,
				//!< Maximum clients allowed
	MAXBUFFER = 2560,	//!< Buffer Size (For socket operations)
	VISRANGE = 18,		//!< Visibility for normal items & chars
	BUILDRANGE = 90,	//!< Visibility for castles and keeps
	MAP0CACHE = 300,

	XYMAX = 256,		//!< Maximum items which can be handle on one X/Y square
	
	DOORTYPES = 17,
	POLYMORPHMENUOFFSET = 8196,
				//!< polymorph spell menu offset
	MAKEMENUOFFSET = 2048

}; //enum

//@{
/*!
\name Invalid constants
\brief Constants used to represent an invalid integer value
\deprecated We shouldn't use them as widespread as it's now
*/
	static const int64_t INVALID = -1;
	static const uint8_t UINVALID08 = 0xFF;
	static const uint16_t UINVALID16 = 0xFFFF;
	static const uint32_t UINVALID32 = 0xFFFFFFFF;
//@}

//@{
/*!
\name Region privs
*/
	static const uint8_t rgnFlagGuarded		= 0x01; //!< The region is guarded
	static const uint8_t rgnFlagMarkAllowed		= 0x02; //!< Marking rule is allowed
	static const uint8_t rgnFlagGateAllowed		= 0x04; //!< Gates are allowed
	static const uint8_t rgnFlagRecallAllowed	= 0x08; //!< Recall is allowed
//@}

//@{
/*!
\name Teleport flags
\brief Flags used by cChar::teleport() function
*/
	static const uint8_t teleNone		= 0x00; //!< Send nothing
	static const uint8_t teleSendWornItems	= 0x01; //!< Sends the worn items
	static const uint8_t teleSendNearChars	= 0x02; //!< Sends the chars near self
	static const uint8_t teleSendNearItems	= 0x04; //!< Sends the items near self
	static const uint8_t teleSendLight	= 0x08; //!< Sends the light level
	static const uint8_t teleSendWeather	= 0x10; //!< Sends the weather status
	static const uint8_t teleAll		= 0x1F; //!< Mask for all the flags
//@}

//@{
/*!
\name Item types
\brief Constant used to identify some kind of items.
\deprecated This should be changed in a better class-ed thing
*/
	static const uint16_t ITYPE_ORDER_GATES = 2;
	static const uint16_t ITYPE_ORDER_GATES_OPENER = 3;
	static const uint16_t ITYPE_CHAOS_GATES = 4;
	static const uint16_t ITYPE_CHAOS_GATES_OPENER = 5;
	static const uint16_t ITYPE_TELEPORTRUNE = 6;
	static const uint16_t ITYPE_KEY = 7;
	static const uint16_t ITYPE_LOCKED_ITEM_SPAWNER = 8;
	static const uint16_t ITYPE_SPELLBOOK = 9
	static const uint16_t ITYPE_MAP = 10;
	static const uint16_t ITYPE_DOOR = 12;
	static const uint16_t ITYPE_LOCKED_DOOR = 13;
	static const uint16_t ITYPE_FOOD = 14;
	static const uint16_t ITYPE_WAND = 15;
	static const uint16_t ITYPE_RESURRECT = 16;
	static const uint16_t ITYPE_MANAREQ_WAND = 17;
	static const uint16_t ITYPE_POTION = 19;
	static const uint16_t ITYPE_RUNE = 50;
	static const uint16_t ITYPE_ITEM_SPAWNER = 61;
	static const uint16_t ITYPE_NPC_SPAWNER = 62;
	static const uint16_t ITYPE_UNLOCKED_CONTAINER = 63;
	static const uint16_t ITYPE_LOCKED_CONTAINER = 64;
	static const uint16_t ITYPE_NODECAY_ITEM_SPAWNER = 65;
	static const uint16_t ITYPE_DECAYING_ITEM_SPAWNER = 66;
	static const uint16_t ITYPE_RAND_NPC_SPAWNER = 69;
	static const uint16_t ITYPE_ADVENCEMET_GATE1 = 80;
	static const uint16_t ITYPE_ADVENCEMET_GATE2 = 81;
	static const uint16_t ITYPE_ADVENCEMET_GATE_SEX = 82;
	static const uint16_t ITYPE_TRASH = 87;
	static const uint16_t ITYPE_BOATS = 117;
	static const uint16_t ITYPE_FIREWORKS_WAND = 181;
	static const uint16_t ITYPE_SMOKE = 185;
	static const uint16_t ITYPE_RENAME_DEED = 186;
	static const uint16_t ITYPE_POLYMORPH = 101;
	static const uint16_t ITYPE_POLYMORPH_BACK = 102;
	static const uint16_t ITYPE_ARMY_ENLIST = 103;
	static const uint16_t ITYPE_TELEPORT = 104;
	static const uint16_t ITYPE_DRINK = 105;
	static const uint16_t ITYPE_ESCORTSPAWN = 125;
	static const uint16_t ITYPE_GUMPMENU = 203;
	static const uint16_t ITYPE_PLAYER_VENDOR_DEED = 217;
	static const uint16_t ITYPE_ITEMID_WAND = 404;
//@}

//@{
/*!
\name Line of sight
*/
	static const uint8_t losTreesBushes	= 0x01; //!< Trees and other large vegetaion in the way
	static const uint8_t losWallsChimneys	= 0x02; //!< Walls, chimineys, ovens, etc... in the way
	static const uint8_t losDoors		= 0x04; //!< Doors in the way
	static const uint8_t losRoofingSlanted	= 0x08; //!< So can't tele onto slanted roofs, basically
	static const uint8_t losRoofingFlat	= 0x10; //!< So can't tele onto flat roofs, basically
	static const uint8_t losLavaWater	= 0x20; //!< Don't know what all to use this for yet
//@}

//@{
/*!
\name enumerated constants
*/

//! List of monster sounds (see cChar::playMonsterSound())
typedef enum { SND_STARTATTACK = 0, SND_IDLE, SND_ATTACK, SND_DEFEND, SND_DIE } MonsterSound;
#define ALL_MONSTER_SOUND 5

enum { ALLSKILLS = 49, STR , DEX, INTEL, FAME, KARMA,
	I_ACCOUNT, SKILLS
};

//! for spinning wheel -vagrant
enum { YARN, THREAD };

//! Wrestling - Luxor
enum { WRESDISARM = 1, WRESSTUNPUNCH };

//! type of item use
enum { ITEM_USE_UNKNOWN = 0, ITEM_USE_WEAR, ITEM_USE_CHECKEQUIP, ITEM_USE_DBLCLICK };

//! Stat types
enum StatType { STAT_HP = 0, STAT_MANA, STAT_STAMINA, ALL_STATS };

//! Bank Type
enum { BANK_GOLD = 1, BANK_ITEM = 0, MOREY_GOLDONLYBANK = 123, MOREX_BANK = 1 };

/*!
\brief temp effects
\author Luxor
*/
namespace tempfx {
	enum {
		SPELL_PARALYZE = 1,
		SPELL_LIGHT,
		spellClumsy,
		spellFeebleMind,
		spellWeaken,
		spellAgility,
		spellCunning,
		spellStrenght,
		ALCHEMY_GRIND,
		ALCHEMY_END,
		spellBless,
		spellCurse,
		AUTODOOR,
		TRAINDUMMY,
		SPELL_REACTARMOR,
		EXPLOTIONMSG,
		EXPLOTIONEXP,
		SPELL_POLYMORPH,
		SPELL_INCOGNITO,
		LSD,
		spellProtection,
		HEALING_HEAL,
		HEALING_RESURRECT,
		HEALING_CURE,
		POTION_DELAY,
		SPELL_INVISIBILITY,
		DRINK_EMOTE,
		DRINK_FINISHED,
		GM_HIDING = 33,
		GM_UNHIDING,
		HEALING_DELAYHEAL,
		COMBAT_PARALYZE	= 44,
		COMBAT_CONCUSSION = 45,
		AMXCUSTOM = 121,	//!< custom amx effects
		GREY,			//!< mark grey
		CRIMINAL,		//!< mark criminal
		FIELD_DAMAGE,		//!< magic fields damage
		spellTelekinesys,	//!< telekinesys spell
		MAX_TEMPFX_INDEX
	};
};

#endif
