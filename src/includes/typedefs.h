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
*/

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__
#include <set>


typedef double				R64;	//< 64 bits floating point
typedef float				R32;	//< 32 bits floating point

#if defined __GNUC__
	#include <bits/types.h>
	typedef uint8_t			UI08;
	typedef uint16_t		UI16;
	typedef uint32_t		UI32;
	typedef uint64_t		UI64;
	typedef int8_t			SI08;
	typedef int16_t			SI16;
	typedef int32_t			SI32;
	typedef int64_t			SI64;
#elif defined __BORLANDC__
	typedef unsigned __int8		UI08;
	typedef unsigned __int16	UI16;
	typedef unsigned __int32	UI32;
	typedef unsigned __int64	UI64;

	typedef signed __int8 SI08;
	typedef signed __int16 SI16;
	typedef signed __int32 SI32;
	typedef signed __int64 SI64;
#endif

typedef std::vector<pItem>	ItemList;


typedef UI32				TIMERVAL;
typedef SI32				ACCOUNT;
typedef SI32				FUNCIDX;

// typedef std::vector<UI32>		UI32VECTOR;	//!< vector of unsigned 32 bits integers
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

struct lookuptr_st //Tauriel  used to create pointers to the items dynamically allocated
{               //         so don't mess with it unless you know what you are doing!
  SI32 max;
  SI32 *pointer;
};

struct location_st
{
 SI32 x1;
 SI32 y1;
 SI32 x2;
 SI32 y2;
 //char region;
 UI08 region;
};

struct logout_st//Instalog
{
        UI32 x1;
        UI32 y1;
        UI32 x2;
        UI32 y2;
};

struct path_st {
	UI32 x;
	UI32 y;
};


//REPSYS
struct repsys_st
{
	SI32	murderdecay;
	UI32		maxkills;
	SI32		crimtime;
};


struct resource_st
{
	UI32		logs;
	TIMERVAL	logtime;
	TIMERVAL	lograte;//AntiChrist
	UI32		logarea;
	SI32		logtrigger;//AntiChrist
	SI32		logstamina;//AntiChrist
};


struct creat_st
{
	SI32 basesound;
	UI08 soundflag;
	UI08 who_am_i;
	SI32 icon;
};

// XYZZY
struct unitile_st
{
 SI08 basez;
 UI08 type; // 0=Terrain, 1=Item
 UI16 id;
 UI08 flag1;
 UI08 flag2;
 UI08 flag3;
 UI08 flag4;
 SI08 height;
 UI08 weight;
} PACK_NEEDED;

struct skill_st
{
 SI32 st;
 SI32 dx;
 SI32 in;
 SI32 unhide_onuse;
 SI32 unhide_onfail;
 SI32 advance_index;
 TEXT madeword[50]; // Added by Magius(CHE)
};

struct advance_st
{
 UI08 skill;
 SI32 base;
 SI32 success;
 SI32 failure;
};

struct gmpage_st
{
	TEXT reason[80];
	Serial serial;
	TEXT timeofcall[9];
	TEXT name[20];
	SI32 handled;
} PACK_NEEDED;

struct begging_st
{
 SI32 timer;
 UI32 range;
 TEXT text[3][256];
};

struct fishing_st
{
 TIMERVAL basetime;
 TIMERVAL randomtime;
};

struct spiritspeak_st
{
 TIMERVAL spiritspeaktimer;
};

/*!
\brief struct for manage lag fix
\note extended by AntiChrist and Ripper
*/
struct speed_st
{
	SI32 nice;
	R32 itemtime;
	TIMERVAL srtime;
	R32 npctime;
	R32 tamednpctime;
	R32 npcfollowtime;
	R32 npcaitime;
};


/*!
\brief struct for manage custom titles
*/
struct title_st
{
 TEXT fame[50];
 TEXT skill[50];
 TEXT prowess[50];
 TEXT other[50];
};

/*!
\brief Represent a location on one map
\author Anthalir
*/
struct Location {
	UI16 x;		//!< X-Coordinate of the location
	UI16 y;		//!< Y-Coordinate of the location
	SI08 z;		//!< Z-Coordinate of the location
	SI08 dispz;	//!< Displayed Z of the location
	UI08 map;	//!< Map the location (if 255 -> every map)
	
	/*!
	\brief Constructor for Location structure
	\param X x of the new location
	\param Y y of the new location
	\param Z z of the new location
	\param DISPZ dispz of the new location
	\param MAP map of the new location
	 
	\note Replaces Loc() function
	*/
	inline Location(UI16 X, UI16 Y, SI08 Z, SI08 DISPZ = 0, UI08 MAP = 255)
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
	UI32 rate_eff; //!< regeneration rate current
	UI32 rate_real; //!< regeneration rate real
} regen_st;

#endif //__TYPEDEFS_H__
