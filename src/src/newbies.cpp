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
#include <assert.h>

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
	NBItemSlist NewbiesSkills[skTrueSkills];
	//@}
	
	void padMissingLocations();
	void deleteLocations();
	
	void giveItemsMale(pPC pc, uint16_t pantsColor, uint16_t shirtColor);
	void giveItemsFemale(pPC pc, uint16_t pantsColor, uint16_t shirtColor);
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

/*!
\brief Creates the newbie item and set it to the given character
\param pc Character to give the item to
*/
void nNewbies::sNewbieItem::createItem(pPC pc)
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

/*!
\brief Give to the passed player the newbie's items for his skills and gender.
\param pc Player to give the newbie's items to
\param pantsColor Color of the pants to give to the player
\param shirtColor Color of the shirt to give to the player

This function adds the base items to the newly created player, so it gives to
him the backpack, the shirt and the pants, and the datafile-defined items for
the newbies, for the males or females (as needed) and for him starting skills.
*/
void nNewbies::giveItems(pPC pc, uint16_t pantsColor, uint16_t shirtColor)
{
	assert(pc);
	
	pEquippableContainer pi = dynamic_cast<pEquippableContainer>(nArchetypes::createItem(strBackpackId));
	assert(pi);
	pc->getBody()->setBackpack(pi);

	Skills first = pc->bestSkill();
	Skills second = pc->nextBestSkill(first);
	Skills third = pc->nextBestSkill(second);
	
	if ( pc->getBody()->getSkill(third) < 190 )
		third = skInvalid;
	
	for( NBItemSList::iterator it = NewbiesAll.begin(); it != NewbiesAll.end(); it++ )
		(*it).createItem(pc);
	
	if ( pc->getBody()->getId() == BODY_MALE )
		giveItemsMale(pc, pantsColor, shirtColor);
	else if ( pc->getBody()->getId() == BODY_FEMALE )
		giveItemsFemale(pc, pantsColor, shirtColor);
	else
		LogWarning("We are creating a new player which isn't an human.");

	// Add items by skill
	for( NBItemSList::iterator it = NewbiesSkills[first].begin(); it != NewbiesSkills[first].end(); it++ )
		(*it).createItem(pc);
	
	for( NBItemSList::iterator it = NewbiesSkills[second].begin(); it != NewbiesSkills[second].end(); it++ )
		(*it).createItem(pc);
	
	if ( pc->getBody()->getSkill(third) >= 190 )
		for( NBItemSList::iterator it = NewbiesSkills[third].begin(); it != NewbiesSkills[third].end(); it++ )
			(*it).createItem(pc);
	
	// Add the gold
	if ( ! nSettings::Server::getNewbiesGold() )
		return;
	
	pItem gold = nArchetypes::createItem(strGoldId);
	gold->setAmount( nSettings::Server::getNewbiesGold() );
	gold->setNewbie(true);
	gold->setContainer( pc->getBody()->getBackpack() );
	gold->setColor(0);
	gold->refresh();
}

/*!
\brief Gives the items restricted to male players
\param pc Player to give the items to (i.e.: the parameter of giveItems())
\param pantsColor Color of the pants to give to the player
\param shirtColor Color of the shirt to give to the player

This function also gives the items contained in NewbiesMale list.

\note Shirt is added here because this makes trivial the change to make the
	default shirt for females/males differs.
*/
void nNewbies::giveItemsMale(pPC pc, uint16_t pantsColor, uint16_t shirtColor)
{
	pEquippable pants = NULL;
	if ( ! RandomNum(0, 1) )
		pants = dynamic_cast<pEquippable>(nArchetypes::createItem("item_pants_long"));
	else
		pants = dynamic_cast<pEquippable>(nArchetypes::createItem("item_pants_short"));
	assert(pants);
	
	pants->setColor(pantsColor);
	pc->getBody()->equip(pants);
	
	pEquippable shirt = NULL;
	if ( ! RandomNum(0, 1) )
		shirt = dynamic_cast<pEquippable>(nArchetypes::createItem("item_shirt_fancy"));
	else
		shirt = dynamic_cast<pEquippable>(nArchetypes::createItem("item_shirt"));
	assert(shirt);
	
	shirt->setColor(shirtColor);
	pc->getBody()->equip(shirt);
	
	for( NBItemSList::iterator it = NewbiesMale.begin(); it != NewbiesMale.end(); it++ )
		(*it).createItem(pc);
}

/*!
\brief Gives the items restricted to female players
\param pc Player to give the items to (i.e.: the parameter of giveItems())
\param pantsColor Color of the pants to give to the player
\param shirtColor Color of the shirt to give to the player

This function also gives the items contained in NewbiesFemale list.

\note Shirt is added here because this makes trivial the change to make the
	default shirt for females/males differs.
*/
void nNewbies::giveItemsFemale(pPC pc, uint16_t pantsColor, uint16_t shirtColor)
{
	pEquippable pants = NULL;
	if ( ! RandomNum(0, 1) )
		pants = dynamic_cast<pEquippable>(nArchetypes::createItem("item_skirt"));
	else
		pants = dynamic_cast<pEquippable>(nArchetypes::createItem("item_kilt"));
	assert(pants);
	
	pants->setColor(pantsColor);
	pc->getBody()->equip(pants);
	
	pEquippable shirt = NULL;
	if ( ! RandomNum(0, 1) )
		shirt = dynamic_cast<pEquippable>(nArchetypes::createItem("item_shirt_fancy"));
	else
		shirt = dynamic_cast<pEquippable>(nArchetypes::createItem("item_shirt"));
	assert(shirt);
	
	shirt->setColor(shirtColor);
	pc->getBody()->equip(shirt);
	
	for( NBItemSList::iterator it = NewbiesFemale.begin(); it != NewbiesFemale.end(); it++ )
		(*it).createItem(pc);
}

/*!
\brief Gives to the defined player him new hairs and facial hairs with the chosen color
\param pc Player to give the hairs to
\param hairStyle Hairs' ID
\param hairColor Hairs' color ID
\param facialStyle Facial hairs' ID
\param facialColor Facial hairs' color ID
*/
void nNewbies::giveHairs(pPC pc, uint16_t hairStyle, uint16_t hairColor, uint16_t facialStyle, uint16_t facialColor)
{
	pEquippable hair = NULL, facial = NULL;
	if ( hairStyle && (hair = dynamic_cast<pEquippable>(nArchetypes::createItem(hairStyle))) )
	{
		hair->setColor(hairColor);
		pc->getBody()->equip(hair);
	}
	
	if ( facialStyle && (facial = dynamic_cast<pEquippable>(nArchetypes::createItem(facialStyle))) )
	{
		facial->setColor(facialColor);
		pc->getBody()->equip(facial);
	}
}
