/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __NEWBIE_H__
#define __NEWBIE_H__

#include "common_libs.h"
#include "structs.h"

/*!
\namespace nNewbies
\brief New players stuff

In this namespace we have the functions and the structure used to load the data
to create new players, so-called \em newbies .

Here we can find the functions to give items to the new players and the ones to
find out the locations where new players can be teletrasported.
*/
namespace nNewbies {

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
	extern sStartLocation *startLocations[9];
	
	void loadStartLocations();
	
	/*!
	\brief Struct representing a newbie item
	\note \c color is represented by a string to allow the use of random colors,
		which at the moment aren't actually implemented.
	*/
	struct sNewbieItem {
		std::string item;	//!< String code of the item to add
		ItemPlace place;	//!< Place where to add the item
		uint16_t amount;	//!< Amount of items to add
		std::string color;	//!< Color of the items to add
		
		sNewbieItem()
		{ place = ipBackpack; amount = 1; color = "colorNeutral"; }
		
		void createItem(pBody body);
	};
	
	//! Singly-linked list of sNewbieItem's
	typedef std::slist<sNewbieItem> NBItemSList;
	
	void loadNewbieItems();
	
	void giveItems(pBody body, uint16_t pantsColor, uint16_t shirtColor);
	void giveHairs(pBody body, uint16_t hairStyle, uint16_t hairColor, uint16_t facialStyle, uint16_t facialColor);
}

#endif
