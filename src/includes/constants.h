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
static const uint16_t BODY_MALE		= 0x0190;	//!< Male body
static const uint16_t BODY_FEMALE	= 0x0191;	//!< Female body
static const uint16_t BODY_DEADMALE	= 0x0192;	//!< Dead male body
static const uint16_t BODY_DEADFEMALE	= 0x0193;	//!< Dead female body
static const uint16_t BODY_GMSTAFF	= 0x03DB;	//!< GM Staff body
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

static const uint16_t MAP_WIDTH = 6144;		//!< Width of the UO Map
static const uint16_t MAP_HEIGHT = 4096;	//!< Height of the UO Map

static const char strNull[] = "";

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
	MAXSERV = 15,		//!< Maximum servers in login listing
	MAXBUFFER = 2560,	//!< Buffer Size (For socket operations)
	MAXSTART = 15,		//!< Maximum starting locations
	MAXPAGES = 75,		//!< Maximum number of pages in the GM queue
	VISRANGE = 18,		//!< Visibility for normal items & chars
	BUILDRANGE = 90,	//!< Visibility for castles and keeps
	MAP0CACHE = 300,
	PATHNUM = 2,

	XYMAX = 256,		//!< Maximum items which can be handle on one X/Y square
	CMAX = 40,		//!< Maximum parameters in one line (Only for memory reasons)
	
	DOORTYPES = 17,
	POLYMORPHMENUOFFSET = 8196,
				//!< polymorph spell menu offset
	MAKEMENUOFFSET = 2048

}; //enum

#define INVALID -1
#define UINVALID08 0xFF
#define UINVALID16 0xFFFF
#define UINVALID32 0xFFFFFFFF

//@{
/*!
\name enumerated extern extern constants
*/

//! Verdata patches index
enum
{
	VERFILE_MAP = 0, VERFILE_STAIDX, VERFILE_STATICS, VERFILE_ARTIDX, VERFILE_ART,
	VERFILE_ANIMIDX, VERFILE_ANIM, VERFILE_SOUNDIDX, VERFILE_SOUND, VERFILE_TEXIDX,
	VERFILE_TEXMAPS, VERFILE_GUMPIDX, VERFILE_GUMPART, VERFILE_MULTIIDX, VERFILE_MULTI,
	VERFILE_SKILLSIDX, VERFILE_SKILLS, VERFILE_TILEDATA, VERFILE_ANIMDATA,
	TILEDATA_TILES = 0x68800
};

//! List of monster sounds (see cChar::playMonsterSound())
typedef enum { SND_STARTATTACK = 0, SND_IDLE, SND_ATTACK, SND_DEFEND, SND_DIE } MonsterSound;
#define ALL_MONSTER_SOUND 5

enum { ALLSKILLS = 49, STR , DEX, INTEL, FAME, KARMA,
	I_ACCOUNT, SKILLS
};

//! cChar::teleport flags
enum {
	TELEFLAG_NONE = 0x0, TELEFLAG_SENDWORNITEMS = 0x1, TELEFLAG_SENDNEARCHARS = 0x2,
	TELEFLAG_SENDNEARITEMS = 0x4, TELEFLAG_SENDLIGHT = 0x8, TELEFLAG_SENDWEATHER = 0x10,
	TELEFLAG_SENDALL = 0x1F
};

//! for spinning wheel -vagrant
enum { YARN, THREAD };

//! Wrestling - Luxor
enum { WRESDISARM = 1, WRESSTUNPUNCH };

//! type of item use
enum { ITEM_USE_UNKNOWN = 0, ITEM_USE_WEAR, ITEM_USE_CHECKEQUIP, ITEM_USE_DBLCLICK };

enum { spawn_script = 20, NUM_SCRIPTS = 32};

//! Region privs
enum {	RGNPRIV_GUARDED = 0x01, RGNPRIV_MARKALLOWED = 0x02, RGNPRIV_GATEALLOWED = 0x04,
	RGNPRIV_RECALLALLOWED = 0x08
};

//! Stat types
enum StatType { STAT_HP = 0, STAT_MANA, STAT_STAMINA, ALL_STATS };

//! Type of item
enum ItemType {
	ITYPE_ORDER_GATES = 2, ITYPE_ORDER_GATES_OPENER = 3, ITYPE_CHAOS_GATES = 4,
	ITYPE_CHAOS_GATES_OPENER = 5, ITYPE_TELEPORTRUNE = 6,  ITYPE_KEY = 7, ITYPE_LOCKED_ITEM_SPAWNER = 8,
	ITYPE_SPELLBOOK = 9, ITYPE_MAP = 10, ITYPE_DOOR = 12, ITYPE_LOCKED_DOOR = 13,
	ITYPE_FOOD = 14, ITYPE_WAND = 15, ITYPE_RESURRECT = 16,ITYPE_MANAREQ_WAND = 17, ITYPE_POTION = 19,
	ITYPE_RUNE = 50, ITYPE_ITEM_SPAWNER = 61, ITYPE_NPC_SPAWNER = 62, ITYPE_UNLOCKED_CONTAINER = 63,
	ITYPE_LOCKED_CONTAINER = 64, ITYPE_NODECAY_ITEM_SPAWNER = 65, ITYPE_DECAYING_ITEM_SPAWNER = 66,
	ITYPE_RAND_NPC_SPAWNER = 69, ITYPE_ADVENCEMET_GATE1 = 80, ITYPE_ADVENCEMET_GATE2 = 81,
	ITYPE_ADVENCEMET_GATE_SEX = 82, ITYPE_TRASH = 87, ITYPE_BOATS = 117, ITYPE_FIREWORKS_WAND = 181,
	ITYPE_SMOKE = 185, ITYPE_RENAME_DEED = 186, ITYPE_POLYMORPH = 101, ITYPE_POLYMORPH_BACK = 102,
	ITYPE_ARMY_ENLIST = 103, ITYPE_TELEPORT = 104, ITYPE_DRINK = 105, ITYPE_ESCORTSPAWN = 125,
	ITYPE_GUMPMENU = 203, ITYPE_PLAYER_VENDOR_DEED = 217, ITYPE_ITEMID_WAND = 404
};

//! Bank Type
enum { BANK_GOLD = 1, BANK_ITEM = 0, MOREY_GOLDONLYBANK = 123, MOREX_BANK = 1 };

//! House list type
enum { H_FRIEND = 1, H_BAN };

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

/*!
\brief temp effects
\author Luxor
*/
namespace tempfx {
	enum {
		SPELL_PARALYZE = 1,
		SPELL_LIGHT,
		SPELL_CLUMSY,
		SPELL_FEEBLEMIND,
		SPELL_WEAKEN,
		SPELL_AGILITY,
		SPELL_CUNNING,
		SPELL_STRENGHT,
		ALCHEMY_GRIND,
		ALCHEMY_END,
		SPELL_BLESS,
		SPELL_CURSE,
		AUTODOOR,
		TRAINDUMMY,
		SPELL_REACTARMOR,
		EXPLOTIONMSG,
		EXPLOTIONEXP,
		SPELL_POLYMORPH,
		SPELL_INCOGNITO,
		LSD,
		SPELL_PROTECTION,
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
		SPELL_TELEKINESYS,	//!< telekinesys spell
		MAX_TEMPFX_INDEX
	};
};

#endif
