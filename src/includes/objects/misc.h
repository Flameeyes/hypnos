/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\brief
\file Miscellaneous objects' (structures, small classes) declarations
*/

#ifndef __MISC_H__
#define __MISC_H__

//! Location on the map
struct Location {
	uint16_t x;		//!< X-Coordinate of the location
	uint16_t y;		//!< Y-Coordinate of the location
	int8_t z;		//!< Z-Coordinate of the location
	int8_t dispz;	//!< Displayed Z of the location
	uint8_t map;	//!< Map the location (if 255 -> every map)
};

//! == operator redefinition for Location
inline bool operator ==(Location a, Location b)
{
	return (
		(a.x == b.x) &&
		(a.y == b.y) &&
		(a.z == b.z) &&
		(a.dispz == b.dispz) &&
		(a.map == b.map)
	);
}

//! != operator redefinition for Location
inline bool operator !=(Location a, Location b)
{ return ! ( a == b ); }

#endif
