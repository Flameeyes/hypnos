/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file typedefs.h
\brief contains typedefs for integer values and common structures.
\todo complete the documentation of types
\todo Rename structures ending in _st to starting with s
*/

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <stdint.h>
#include <set>

typedef double				R64;	//< 64 bits floating point
typedef float				R32;	//< 32 bits floating point

#ypedef std::vector<pItem>	ItemList;


typedef uint32_t				TIMERVAL;
typedef int32_t				ACCOUNT;
typedef int32_t				FUNCIDX;

// typedef std::vector<uint32_t>		uint32_tVECTOR;	//!< vector of unsigned 32 bits integers
// typedef std::vector<SERIAL>		SERIAL_VECTOR;	//!< vector of serials
// typedef slist<SERIAL>                   SERIAL_SLIST;	//!< slist of serials

// typedef SERIAL 				SCRIPTID; 	//!< a script id
// typedef std::set<SERIAL>		SERIAL_SET;	//!< set of serials
// typedef std::vector< P_OBJECT >		POBJECT_VECTOR;	//!<  a vector of pointers to cObject
// typedef POBJECT_VECTOR::iterator	POBJECT_VECTOR_IT;//!< an iterator to a vector of pointers to cObject
// typedef std::vector< P_CHAR >		PCHAR_VECTOR;	//!< a vector of pointers to cChar
// typedef PCHAR_VECTOR::iterator		PCHAR_VECTOR_IT;//!< an iterator to a vector of pointers to cChar
// typedef std::vector< P_ITEM >		PITEM_VECTOR;	//!<  a vector of pointers to cItem
// typedef PITEM_VECTOR::iterator		PITEM_VECTOR_IT;//!< an iterator to a vector of pointers to cChar
// typedef class cPartyMember* P_PARTY_MEMBER;
// typedef class cParty*	P_PARTY;
// typedef class cBasicMenu* P_MENU;
// typedef class cGuild* P_GUILD;
// typedef class cGuildMember* P_GUILD_MEMBER;
// typedef class cGuildRecruit* P_GUILD_RECRUIT;
// typedef class cClient* P_CLIENT;

#if defined __GNUC__ && (__GNUC__ < 3 || defined(WIN32))
	typedef std::basic_string <wchar_t> wstring;
#else
	using std::wstring;
#endif

#ifdef __BORLANDC__
	#include <stlport/hash_map>
#endif

template < class T >
class cPoint{
public:

	cPoint( T a=0, T b=0 ) { this->a=a; this->b=b; };

	T a;
	T b;
};

struct location_st
{
 int32_t x1;
 int32_t y1;
 int32_t x2;
 int32_t y2;
 //char region;
 uint8_t region;
};

struct logout_st//Instalog
{
        uint32_t x1;
        uint32_t y1;
        uint32_t x2;
        uint32_t y2;
};

struct path_st {
	uint32_t x;
	uint32_t y;
};


//REPSYS
struct repsys_st
{
	int32_t	murderdecay;
	uint32_t		maxkills;
	int32_t		crimtime;
};


struct resource_st
{
	uint32_t		logs;
	TIMERVAL	logtime;
	TIMERVAL	lograte;//AntiChrist
	uint32_t		logarea;
	int32_t		logtrigger;//AntiChrist
	int32_t		logstamina;//AntiChrist
};


struct creat_st
{
	int32_t basesound;
	uint8_t soundflag;
	uint8_t who_am_i;
	int32_t icon;
};

/*!
\todo rename to sUnitile
*/
struct unitile_st
{
 int8_t basez;
 uint8_t type; // 0=Terrain, 1=Item
 uint16_t id;
 uint8_t flag1;
 uint8_t flag2;
 uint8_t flag3;
 uint8_t flag4;
 int8_t height;
 uint8_t weight;
} PACK_NEEDED;

/*!
\todo rename to sSkill
\todo use std::string instead of c-strings
*/
struct skill_st
{
 int32_t st;
 int32_t dx;
 int32_t in;
 int32_t unhide_onuse;
 int32_t unhide_onfail;
 int32_t advance_index;
 TEXT madeword[50]; // Added by Magius(CHE)
};

/*!
\todo rename to sAdvance
\todo verify if the int32_t are too much for the values
*/
struct advance_st
{
 uint8_t skill;
 int32_t base;
 int32_t success;
 int32_t failure;
};

/*!
\todo rename to sGMPage
\todo Use std::string instead of c-strings
*/
struct gmpage_st
{
	TEXT reason[80];
	Serial serial;
	TEXT timeofcall[9];
	TEXT name[20];
	int32_t handled;
} PACK_NEEDED;

/*!
\todo We really need it? It's only a value!
*/
struct spiritspeak_st
{
 TIMERVAL spiritspeaktimer;
};

/*!
\brief struct for manage lag fix
\note extended by AntiChrist and Ripper
*/
struct sSpeed
{
	int32_t nice;
	R32 itemtime;
	TIMERVAL srtime;
	R32 npctime;
	R32 tamednpctime;
	R32 npcfollowtime;
	R32 npcaitime;
};


/*!
\brief struct for manage custom titles
\todo Verify if it's actually used or not
*/
struct sTitle
{
	std::string fame;
	std::string skill;
	std::string prowess;
	std::string other;
};

/*!
\brief Represent a location on one map
\author Anthalir
*/
struct Location {
	uint16_t x;		//!< X-Coordinate of the location
	uint16_t y;		//!< Y-Coordinate of the location
	int8_t z;		//!< Z-Coordinate of the location
	int8_t dispz;	//!< Displayed Z of the location
	uint8_t map;	//!< Map the location (if 255 -> every map)
	
	/*!
	\brief Constructor for Location structure
	\param X x of the new location
	\param Y y of the new location
	\param Z z of the new location
	\param DISPZ dispz of the new location
	\param MAP map of the new location
	 
	\note Replaces Loc() function
	*/
	inline Location(uint16_t X, uint16_t Y, int8_t Z, int8_t DISPZ = 0, uint8_t MAP = 255)
	{
		x = X; y = Y; z = Z; dispz = DISPZ ? DISPZ : z; map = MAP;
	}
};

struct tele_locations_st {
	Location destination, origem;
};


/*!
\brief Stat regeneration info
\author Endymion
\since 0.83
\note regeneration rage is in milliseconds
*/
typedef struct {
	TIMERVAL timer; //!< the timer
	uint32_t rate_eff; //!< regeneration rate current
	uint32_t rate_real; //!< regeneration rate real
} regen_st;

#endif //__TYPEDEFS_H__
