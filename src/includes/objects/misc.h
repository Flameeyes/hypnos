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
	UI16 x;		//!< X-Coordinate of the location
	UI16 y;		//!< Y-Coordinate of the location
	SI08 z;		//!< Z-Coordinate of the location
	SI08 dispz;	//!< Displayed Z of the location
	UI08 map;	//!< Map the location (if 255 -> every map)
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
