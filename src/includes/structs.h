/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Contains misc structs used here and there in the code
\todo complete the documentation of structs
\todo Rename structures ending in _st to starting with s
\deprecated Many structs should be renamed / moved in proper way
*/

#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#include "common_libs.h"

//! Point in the map
struct sPoint {
	uint16_t x;
	uint16_t y;
	sPoint(uint16_t X, uint16_t Y) : x(X), y(Y) { }
};

//! Represent a location on one map
struct sLocation {
	uint16_t x;		//!< X-Coordinate of the location
	uint16_t y;		//!< Y-Coordinate of the location
	int8_t z;		//!< Z-Coordinate of the location
	int8_t dispz;		//!< Displayed Z of the location
	uint8_t map;		//!< Map the location (if 255 -> every map)
	
	/*!
	\brief Constructor for sLocation structure
	\param X x of the new location
	\param Y y of the new location
	\param Z z of the new location
	\param DISPZ dispz of the new location
	\param MAP map of the new location
	*/
	inline sLocation(uint16_t X, uint16_t Y, int8_t Z, int8_t DISPZ = 0, uint8_t MAP = 255)
	{
		x = X; y = Y; z = Z; dispz = DISPZ ? DISPZ : z; map = MAP;
	}

	//! == operator redefinition for sLocation
	inline bool operator ==(const sLocation &b)
	{ return x == b.x && y == b.y && z == b.z; }
	
	//! != operator redefinition for sLocation
	inline bool operator !=(const sLocation &b)
	{ return x != b.x || y != b.y || z != b.z; }

	//! Converts a location (x,y,z) into a point (x,y)
	inline operator sPoint() const
	{ return sPoint(x, y); }
};

/*!
\brief Item in a container

\see cpacket.h
*/
struct sContainerItem
{
	//! Default null constructor
	sContainerItem() { }
	sContainerItem(pItem item);
	
	uint32_t serial;	//!< Serial of the item
	uint16_t id;		//!< ID of the item
	uint16_t amount;	//!< Amount of items
	uint16_t x, y;		//!< Position of the item
	uint16_t color;		//!< Color of the item
};

struct resource_st
{
	uint32_t logs;
	uint32_t logtime;
	uint32_t lograte;//AntiChrist
	uint32_t logarea;
	int32_t logtrigger;//AntiChrist
	int32_t logstamina;//AntiChrist
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
\todo We really need it? It's only a value!
*/
struct spiritspeak_st
{
 uint32_t spiritspeaktimer;
};

/*!
\brief struct for manage lag fix
\note extended by AntiChrist and Ripper
*/
struct sSpeed
{
	int32_t nice;
	float itemtime;
	uint32_t srtime;
	float npctime;
	float tamednpctime;
	float npcfollowtime;
	float npcaitime;
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

struct tele_locations_st {
	sLocation destination, origem;
};


/*!
\brief Stat regeneration info
\author Endymion
\since 0.83
\note regeneration rage is in milliseconds
*/
typedef regen_st {
	uint32_t timer; //!< the timer
	uint32_t rate_eff; //!< regeneration rate current
	uint32_t rate_real; //!< regeneration rate real
};

#endif
