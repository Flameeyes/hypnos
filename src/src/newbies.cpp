/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "newbies.h"
#include "basics.h"
#include "inlines.h"
#include "constants.h"
#include "enums.h"
#include "logsystem.h"
#include "objects/citem.h"
#include "objects/citem/cequippablecontainer.h"
#include "objects/cbody.h"
#include "archetypes/generic.h"
#include "backend/strconstants.h"

#include <mxml.h>
#include <fstream>

namespace nNewbies {
	//@{
	/*!
	\name Newbies' items' lists
	
	The variables in this group are variables used for store the data about the
	items to add to the newbies.
	*/
	//! Items common to all the newbies
	NBItemSList NewbiesAll;
	//! Items common to all the male newbies
	NBItemSList NewbiesMale;
	//! Items common to all the female newbies
	NBItemSList NewbiesFemale;
	//! Items for skills' newbies
	NBItemSList NewbiesSkills[skTrueSkills];
	//@}
	
	static void padMissingLocations();
	void deleteLocations();
	
	NBItemSList loadItemsList(MXML::Node *n);
	
	void giveItemsMale(pBody body, uint16_t pantsColor, uint16_t shirtColor);
	void giveItemsFemale(pBody body, uint16_t pantsColor, uint16_t shirtColor);
}

/*!
\brief Checks startLocation for missing locations and pad them

This function is called by loadStartLocations() to fix the empty (NULL)
starting locations.

Is needed that the number starting locations is 9, so if the actually loaded
locations are less than that, the first location is duped in the other slots.
*/
static void nNewbies::padMissingLocations()
{
	for(register int i = 0; i < 9; i++)
	{
		if ( ! startLocations[i] )
			startLocations[i] = startLocations[0];
	}
}

/*!
\brief Deletes the locations

This function is called by loadStartLocations() function to reset the
startLocations array if there's already other locations defined and loaded.

This function deletes the pointer and sets it to NULL (so it won't be deleted
more than one time if the first location is duped on other locations.
*/
void nNewbies::deleteLocations()
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
void nNewbies::loadStartLocations()
{
	// The first time we can have a dirty startLocations array, so we
	// can't try to delete pointers (they'll probably point to other data!)
	static bool firstLoad = true;
	
	if ( ! firstLoad ) deleteLocations();
	
	memset(startLocations, 0, sizeof(startLocations));
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
						l->position.x = cVariant(p->getAttribute("x")).toUInt16();
						l->position.y = cVariant(p->getAttribute("y")).toUInt16();
						l->position.z = cVariant(p->getAttribute("z")).toSInt8();
					}
					
				} while ( (p = p->next()) );
				
				startLocations[i] = l;
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

/*!
\brief Loads the newbies.xml datafile and saves it into the items' lists.

This function clears the items' lists and fill them with the items load from 
the datafile.

To accomplish that, the function calls loadItemsList() function for the nodes
after their identiifcation.

The function also loads the nSettings::Server::varNewbiesGold value from the
same datafile.
*/
void nNewbies::loadNewbieItems()
{
	NewbiesAll.clear(); NewbiesMale.clear(); NewbiesFemale.clear();
	for(register int i = 0; i < skTrueSkills; i++)
	{
		NewbiesSkills[i].clear();
	}
	
	//!\todo Add code to cleanup the items' lists
	ConOut("Loading newbies' items...\t\t");
	
	std::ifstream xmlfile("config/newbies.xml");
	if ( ! xmlfile )
	{
		ConOut("[ Failed ]\n");
		LogCritical("Unable to open newbies.xml file.");
		return;
	}

	try {
		MXML::Document doc(xmlfile);
		
		MXML::Node *n = doc.main()->child();
		
		if ( doc.main()->name() != "newbies" )
		{
			ConOut("[ Failed ]\n");
			LogCritical("Unknown document node %s in newbies.xml, failing out", n->name().c_str() );
			return;
		}
		
		if ( doc.main()->hasAttribute("gold") )
			nSettings::Server::setNewbiesGold(cVariant(doc.main()->getAttribute("gold")));
		
		if ( ! n )
			return;
		
		int i = 0;
		
		do {
			if ( n->name() == "allnewbies" )
				NewbiesAll = loadItemsList(n);
			else if ( n->name() == "malenewbies" )
				NewbiesMale = loadItemsList(n);
			else if ( n->name() == "femalenewbies" )
				NewbiesFemale = loadItemsList(n);
			else if ( n->name() == "bestskill" )
			{
				try {
					Skill sk = nStrConstants::skills(n->getAttribute("skill"));
					if ( sk == skInvalid )
					{
						LogWarning("Incomplete node in newbies.xml, ignoring");
						continue;
					}
					NewbiesSkills[sk] = loadItemsList(n);
				} catch( MXML::NotFoundError e ) {
					LogWarning("Incomplete node in newbies.xml, ignoring");
					continue;
				}
			} else {
				LogWarning("Unknown node %s in newbies.xml, ignoring", n->name().c_str() );
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

/*!
\brief Function used for load a newbies' items' list from xmldatafile
\param n Node to start the load from
*/
nNewbies::NBItemSList nNewbies::loadItemsList(MXML::Node *n)
{
	NBItemSList ret;
	MXML::Node *itm = n->child();
	if ( ! itm ) return ret;
	
	do {
		try {
			sNewbieItem i;
			
			i.item = itm->getAttribute("id");
			if ( itm->hasAttribute("color") )
				i.color = itm->getAttribute("color");
			if ( itm->hasAttribute("place") )
				i.place = nStrConstants::itemPlace(itm->getAttribute("place"));
			if ( itm->hasAttribute("amount") )
				i.amount = cVariant(itm->getAttribute("amount"));
			
			ret.push_front(i);
			
		} catch ( MXML::NotFoundError e ) {
			LogWarning("Incomplete node in newbies.xml, ignoring");
			continue;
		}
	} while((itm = itm->next()));
	
	return ret;
}

/*!
\brief Creates the newbie item and set it to the given character
\param body Body of the player to give the newbie's items to
*/
void nNewbies::sNewbieItem::createItem(pBody body)
{
	pItem pi = nArchetypes::createItem( item );
	pi->setAmount(amount);
	//!\todo Change the setColor to one which uses the random colors!
	pi->setColor(0);
	pi->setNewbie(true);
	
	switch( place )
	{
	case ipBackpack:
		pi->setContainer( body->getBackpack(true) );
		break;
	case ipBankbox:
		pi->setContainer( body->getBankBox() );
		break;
	case ipEquip:
		{
			pEquippable pe = dynamic_cast<pEquippable>(pi);
			if ( ! pe ) break; // pass over if not equippable
			body->equip(pe);
		}
	}
}

/*!
\brief Give to the passed player the newbie's items for his skills and gender.
\param body Body of the player to give the newbie's items to
\param pantsColor Color of the pants to give to the player
\param shirtColor Color of the shirt to give to the player

This function adds the base items to the newly created player, so it gives to
him the backpack, the shirt and the pants, and the datafile-defined items for
the newbies, for the males or females (as needed) and for him starting skills.
*/
void nNewbies::giveItems(pBody body, uint16_t pantsColor, uint16_t shirtColor)
{
	assert(body);
	
	// Create the backpack
	pEquippableContainer bp = dynamic_cast<pEquippableContainer>(nArchetypes::createItem(strBackpackId));
	assert(bp);
	body->equip(bp);

	Skill first = body->bestSkill();
	Skill second = body->nextBestSkill(first);
	Skill third = body->nextBestSkill(second);
	
	for( NBItemSList::iterator it = NewbiesAll.begin(); it != NewbiesAll.end(); it++ )
		(*it).createItem(body);
	
	if ( body->getId() == bodyMale )
		giveItemsMale(body, pantsColor, shirtColor);
	else if ( body->getId() == bodyFemale )
		giveItemsFemale(body, pantsColor, shirtColor);
	else
		LogWarning("We are creating a new player which isn't an human.");

	// Add items by skill
	if ( body->getSkill(third) >= 190 )
		for( NBItemSList::iterator it = NewbiesSkills[third].begin(); it != NewbiesSkills[third].end(); it++ )
			(*it).createItem(body);
	
	for( NBItemSList::iterator it = NewbiesSkills[second].begin(); it != NewbiesSkills[second].end(); it++ )
		(*it).createItem(body);
	
	for( NBItemSList::iterator it = NewbiesSkills[first].begin(); it != NewbiesSkills[first].end(); it++ )
		(*it).createItem(body);
	
	// Add the gold
	if ( ! nSettings::Server::getNewbiesGold() )
		return;
	
	pItem gold = nArchetypes::createItem(strGoldId);
	gold->setAmount( nSettings::Server::getNewbiesGold() );
	gold->setNewbie(true);
	gold->setContainer( body->getBackpack() );
	gold->setColor(0);
}

/*!
\brief Gives the items restricted to male players
\param body Body of the player to give the newbie's items to
\param pantsColor Color of the pants to give to the player
\param shirtColor Color of the shirt to give to the player

This function also gives the items contained in NewbiesMale list.

\note Shirt is added here because this makes trivial the change to make the
	default shirt for females/males differs.
*/
void nNewbies::giveItemsMale(pBody body, uint16_t pantsColor, uint16_t shirtColor)
{
	pEquippable pants = NULL;
	if ( ! RandomNum(0, 1) )
		pants = dynamic_cast<pEquippable>(nArchetypes::createItem("item_pants_long"));
	else
		pants = dynamic_cast<pEquippable>(nArchetypes::createItem("item_pants_short"));
	assert(pants);
	
	pants->setColor(pantsColor);
	body->equip(pants);
	
	pEquippable shirt = NULL;
	if ( ! RandomNum(0, 1) )
		shirt = dynamic_cast<pEquippable>(nArchetypes::createItem("item_shirt_fancy"));
	else
		shirt = dynamic_cast<pEquippable>(nArchetypes::createItem("item_shirt"));
	assert(shirt);
	
	shirt->setColor(shirtColor);
	body->equip(shirt);
	
	for( NBItemSList::iterator it = NewbiesMale.begin(); it != NewbiesMale.end(); it++ )
		(*it).createItem(body);
}

/*!
\brief Gives the items restricted to female players
\param body Body of the player to give the newbie's items to
\param pantsColor Color of the pants to give to the player
\param shirtColor Color of the shirt to give to the player

This function also gives the items contained in NewbiesFemale list.

\note Shirt is added here because this makes trivial the change to make the
	default shirt for females/males differs.
*/
void nNewbies::giveItemsFemale(pBody body, uint16_t pantsColor, uint16_t shirtColor)
{
	pEquippable pants = NULL;
	if ( ! RandomNum(0, 1) )
		pants = dynamic_cast<pEquippable>(nArchetypes::createItem("item_skirt"));
	else
		pants = dynamic_cast<pEquippable>(nArchetypes::createItem("item_kilt"));
	assert(pants);
	
	pants->setColor(pantsColor);
	body->equip(pants);
	
	pEquippable shirt = NULL;
	if ( ! RandomNum(0, 1) )
		shirt = dynamic_cast<pEquippable>(nArchetypes::createItem("item_shirt_fancy"));
	else
		shirt = dynamic_cast<pEquippable>(nArchetypes::createItem("item_shirt"));
	assert(shirt);
	
	shirt->setColor(shirtColor);
	body->equip(shirt);
	
	for( NBItemSList::iterator it = NewbiesFemale.begin(); it != NewbiesFemale.end(); it++ )
		(*it).createItem(body);
}

/*!
\brief Gives to the defined player him new hairs and facial hairs with the chosen color
\param body Body of the player to give the hairs to
\param hairStyle Hairs' ID
\param hairColor Hairs' color ID
\param facialStyle Facial hairs' ID
\param facialColor Facial hairs' color ID
*/
void nNewbies::giveHairs(pBody body, uint16_t hairStyle, uint16_t hairColor, uint16_t facialStyle, uint16_t facialColor)
{
	pEquippable hair = NULL, facial = NULL;
	if ( hairStyle && (hair = dynamic_cast<pEquippable>(nArchetypes::createItem(hairStyle))) )
	{
		hair->setColor(hairColor);
		body->equip(hair);
	}
	
	if ( facialStyle && (facial = dynamic_cast<pEquippable>(nArchetypes::createItem(facialStyle))) )
	{
		facial->setColor(facialColor);
		body->equip(facial);
	}
}
