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

//! Struct representing a newbie item
struct sNewbieItem {
	pItem item;	//!< Item to add
	ItemPlace place;//!< Place where to add the item
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

void giveItems(pChar pc)
{
	if ( ! pc ) return;
	
	Skills first = pc->bestSkill();
	Skills second = pc->nextBestSkill(first);
	Skills third = pc->nextBestSkill(second);
	
	if ( pc->getBody()->getSkill(third) )
		third = skInvalid;
	
}

void newbieitems(pChar pc)
{
	pClient ps=pc->getClient();
	if(ps==NULL)
		return;

	int storeval, itemaddperskill, loopexit = 0;
	char sect[512];
	char whichsect[105];
	cScpIterator* iter = NULL;

	first = pc->bestSkill();
	second = pc->nextBestSkill(first);
	third = pc->nextBestSkill(second);
	if (pc->baseskill[third] < 190)
		third = 46;

	for (itemaddperskill = 1; itemaddperskill <= 5; itemaddperskill++)
	{
		switch (itemaddperskill)
		{
			// first of all the general section with the backpack, else where we put items?
			case 1: strcpy(whichsect, "SECTION ALLNEWBIES");		break;
			case 2:
				if ( (pc->getId() == BODY_MALE) && (pc->getOldId() == BODY_MALE) )
					strcpy(whichsect, "SECTION MALENEWBIES");
				else
					strcpy(whichsect, "SECTION FEMALENEWBIES");
				break;
			case 3: sprintf(whichsect, "SECTION BESTSKILL %i", first);	break;
			case 4: sprintf(whichsect, "SECTION BESTSKILL %i", second);	break;
			case 5: sprintf(whichsect, "SECTION BESTSKILL %i", third);	break;
			default:
				ErrOut("Switch fallout. newbie.cpp, newbieitems()/n"); // Morrolan
		}

		sprintf(sect, whichsect);
		char script1[1000], script2[1000];
		safedelete(iter);
		iter = Scripts::Newbie->getNewIterator(sect);
		if (iter==NULL) return;

		do
		{
			iter->parseLine(script1,script2);

			if (script1[0] == '@') pc->loadEventFromScript(script1, script2); 	// Sparhawk: Huh loading character events 
												// from newbie item scripts????

			if (script1[0] != '}')
			{
				if (!(strcmp("PACKITEM", script1)))
				{
					std::string itemnum, amount;
					splitLine( script2, itemnum, amount );
					int amt = ( amount != "" )? str2num( amount ) : INVALID; //ndEndy defined amount
					pItem pi_n = item::CreateFromScript( str2num( itemnum ), pc->getBackpack(), amt );
					if ( pi_n ) {
						pi_n->priv |= 0x02; // Mark as a newbie item
					}
					strcpy(script1, "DUMMY");
				}
				else if (!strcmp("BANKITEM", script1))
				{
					std::string itemnum, amount;
					splitLine( script2, itemnum, amount );
					int amt= (amount!="")? str2num( amount ) : INVALID;
					pItem pi = item::CreateFromScript( str2num( itemnum ), pc->GetBankBox(), amt );
					if (pi) {
						pi->priv |= 0x02; // Mark as a newbie item
					}
					strcpy(script1, "DUMMY");
				}
				else if (!strcmp("EQUIPITEM", script1))
				{
					pItem pi = item::CreateFromScript( script2 );
					if (pi)
					{
						pi->priv |= 0x02; // Mark as a newbie item
						pi->setContainer(pc);
						storeval = pi->getScriptID();
					}
					strcpy(script1, "DUMMY");
				}
			}
		}
		while ((script1[0] != '}') &&(++loopexit < MAXLOOPS));
	
		safedelete(iter);
	}
	
	// Give the character some gold
	if ( goldamount > 0 )
	{
		item::CreateFromScript( "$item_gold_coin", pc->getBackpack(), goldamount );
	}
}
