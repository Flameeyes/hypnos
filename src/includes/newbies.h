/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __NEWBIE_H__
#define __NEWBIE_H__

/*!
\brief Start Location

This struct is used to store the data about the start locations for the new
characters.

The locations must be exactly 9, and if less than that are present, they
need to be padded using the first one.
The nine locations are stored in the startLocations array.
*/
struct sStartLocation
{
	std::string city;	//!< City of the start location
	std::string place;	//!< Name of the place inside the city (Inn usually)
	sLocation position;	//!< Coordinates of the exat start location
};

//! Start locations
sStartLocation *startLocations[9];

void loadStartLocations();

void newbieitems(pChar pc);

#endif
