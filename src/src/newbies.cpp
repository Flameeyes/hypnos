/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "basics.h"
#include "inlines.h"
#include <mxml.h>
#include <fstream>

/*!
\brief Checks startLocation for missing locations and pad them

This function is called by loadStartLocations() to fix the empty (NULL)
starting locations.

Is needed that the number starting locations is 9, so if the actually loaded
locations are less than that, the first location is duped in the other slots.
*/
static void padMissingLocations()
{
	for(register int i = 0; i < 9; i++)
	{
		if ( ! startLocations[i] )
			startLocations[i] = startLocation[0];
	}
}

/*!
\brief Deletes the locations

This function is called by loadStartLocations() function to reset the
startLocations array if there's already other locations defined and loaded.

This function deletes the pointer and sets it to NULL (so it won't be deleted
more than one time if the first location is duped on other locations.
*/
void deleteLocations()
{
	for(register int i = 0; i < 9; i++)
		safedelete(startLocations[i]);
}

/*!
\brief Loads the starting locations

This function loads from start.xml datafile the city name, the place and the
coordinates of the starting locations for new characters, and saves them inside
the startLocations array.

This function calls the deleteLocations() function if not called for the first
time (i.e.: in case of reload of start.xml datafile).

This function also calls padMissingLocations() function if the number of load
functions is less than 9.
*/
void loadStartLocations()
{
	// The first time we can have a dirty startLocations array, so we
	// can't try to delete pointers (they'll probably point to other data!)
	static bool firstLoad = true;
	
	if ( ! firstLoad ) deleteLocations();
	
	memset(startLocations, NULL, sizeof(startLocations));
	firstLoad = false;
	
	ConOut("Loading start locations...\t\t");
	
	std::ifstream xmlfile("config/start.xml");
	if ( ! xmlfile )
	{
		ConOut("[ Failed ]\n");
		LogCritical("Unable to open start.xml file.");
		return;
	}

	try {
		MXML::Document doc(xmlfile);
		
		MXML::Node *n = doc.main()->child();
		if ( ! n )
		{
			padMissingLocations();
			return;
		}
		
		int i = 0;
		
		do {
			if ( n->name() != "location" )
			{
				LogWarning("Unknown node %s in start.xml, ignoring", n->name().c_str() );
				continue;
			}
			
			sStartLocation *l = new sStartLocation;
			try {
				MXML::Node *p = n->child();
				
				if ( ! p )
					continue;
				
				do {
					if ( p->name() == "city" )
						l->city = p->data();
					else if ( p->name() == "place" )
						l->place = p->data();
					else if ( p->name() == "position" )
					{
						l->position.x = tVariant(p->getAttribute("x")).toUInt16();
						l->position.y = tVariant(p->getAttribute("y")).toUInt16();
						l->position.z = tVariant(p->getAttribute("z")).toSInt8();
					}
					
				} while ( (p = p->next()) );
				
				locations[i] = l;
				if ( ++i >= 9 ) // We reached the full array
					break;
			
			} catch ( MXML::NotFoundError e ) {
				delete l;
				LogWarning("Incomplete node in start.xml, ignoring");
				continue;
			}
		} while((n = n->next()));
		
		padMissingLocations();
		
		ConOut("[   OK   ]\n");
	} catch ( MXML::MalformedError e) {
		ConOut("[ Failed ]\n");
		LogCritical("start.xml file not well formed.");
	}
}

//@{
/*!
\name Newbies' items' lists

The variables in this group are variables used for store the data about the
items to add to the newbies.
*/

/*!
\brief Struct representing a newbie item
\note \c color is represented by a string to allow the use of random colors,
	which at the moment aren't actually implemented.
*/
struct sNewbieItem {
	std::string item;	//!< String code of the item to add
		//!\todo Should change to use directly the item archetype
	ItemPlace place;	//!< Place where to add the item
	uint16_t amount;	//!< Amount of items to add
	std::string color;	//!< Color of the items to add
	
	createItem(pChar pc);
};

//! Singly-linked list of sNewbieItem's
typedef std::slist<sNewbieItem> NBItemSList;

//! Items common to all the newbies
NBItemSList NewbiesAll;
//! Items common to all the male newbies
NBItemSList NewbiesMale;
//! Items common to all the female newbies
NBItemSList NewbiesFemale;
//! Items for skills' newbies
NBItemSlist NewbiesSkills[skTrueSkills];
//@}

/*!
\brief Creates the newbie item and set it to the given character
\param pc Character to give the item to
*/
sNewbieItem::createItem(pPC pc)
{
	pItem pi = nArchetypes::createItem( item );
	pi->setAmount(amount);
	//!\todo Change the setColor to one which uses the random colors!
	pi->setColor(0);
	pi->setNewbie(true);
	
	switch( place )
	{
	case ipBackpack:
		pi->setContainer( pc->getBody()->getBackpack(true) );
		break;
	case ipBankbox:
		pi->getContainer( pc->getBody()->getBankBox() );
		break;
	case ipEquip:
		{
			pEquippable pe = dynamic_cast<pEquippable>(pi);
			if ( ! pe ) continue; // pass over if not equippable
			pc->getBody()->equip(pe);
		}
	}
	
	pi->refresh();
}

void giveItems(pPC pc)
{
	if ( ! pc ) return;
	
	Skills first = pc->bestSkill();
	Skills second = pc->nextBestSkill(first);
	Skills third = pc->nextBestSkill(second);
	
	if ( pc->getBody()->getSkill(third) < 190 )
		third = skInvalid;
	
	for( NBItemSList::iterator it = NewbiesAll.begin(); it != NewbiesAll.end(); it++ )
		(*it).createItem(pc);
	
	if ( pc->getBody()->getId() == BODY_MALE )
		for( NBItemSList::iterator it = NewbiesMale.begin(); it != NewbiesMale.end(); it++ )
			(*it).createItem(pc);
	else if ( pc->getBody()->getId() == BODY_FEMALE )
		for( NBItemSList::iterator it = NewbiesFemale.begin(); it != NewbiesFemale.end(); it++ )
			(*it).createItem(pc);
	else
		LogWarning("We are creating a new player which isn't an human.");
	
	for( NBItemSList::iterator it = NewbiesSkills[first].begin(); it != NewbiesSkills[first].end(); it++ )
		(*it).createItem(pc);
	
	for( NBItemSList::iterator it = NewbiesSkills[second].begin(); it != NewbiesSkills[second].end(); it++ )
		(*it).createItem(pc);
	
	if ( pc->getBody()->getSkill(third) >= 190 )
		for( NBItemSList::iterator it = NewbiesSkills[third].begin(); it != NewbiesSkills[third].end(); it++ )
			(*it).createItem(pc);
	
	if ( nSettings::Server::getNewbiesGold() )
	{
		pItem gold = nArchetypes::createItem(strGoldId);
		gold->setAmount( nSettings::Server::getNewbiesGold() );
		gold->setNewbie(true);
		gold->setContainer( pc->getBody()->getBackpack() );
		gold->setColor(0);
		gold->refresh();
	}
}
