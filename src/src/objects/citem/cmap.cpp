/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "enums.h"
#include "skills/skills.h"
#include "objects/citem/cmap.h"
#include "objects/cchar.h"
#include "networking/sent.h"
#include "networking/cclient.h"

cMap::cMap() : cItem()
{
}

/*!
\brief cMap constructor
\param ser serial number
*/
cMap::cMap(uint32_t ser) : cItem(ser)
{
	flags |= flagWritable;
}

cMap &cMap::operator= (const cMap &oldmap)
{
	pinData = oldmap.pinData;
	return dynamic_cast<cMap&>(cItem::operator=(dynamic_cast<const cItem&>(oldmap)));
	//using the parent operator = function to copy all other parameters
}


/*!
\brief Adds a new pin to the map
\param p Coords of the pin to add
\retval true The pin is added correctly
\retval false The pin wasn't added because the map is not writable or there are
	already 50 ore more pins
*/
bool cMap::addPin(sPoint p)
{
	if ( pinData.size() >= 50 || !(flags & flagWritable)) return false;
	pinData.push_back(p);
        return true;
}

/*!
\brief Inserts a pin between 2 existing ones
\param p Coords of the pin to insert
\param pin Index of the new pin to insert at
\note Existing pins greater (or equal) pin numer get shifted by one

\retval true The pin was inserted correctly
\retval false The pin wasn't inserted because the map is not writable or there
	are already too much pins, or the index is out of bound

\bug This function doesn't restrict the size of the pin vector to 50
*/
bool cMap::insertPin(sPoint p, uint32_t pin)
{
	if (pin >= pinData.size() || !(flags & flagWritable)) return false;
	for( int i = pinData.size() -1; i >= (pin-1); i++ )
		pinData[i+1] = pinData[i];
	
	pinData[(pin-1)] = sPoint(x, y);
        
	return true;
}

/*!
\brief Moves pin to another position
\param p Coords to set for the pin
\param pin Index of the pin to edit
\retval true The pin was changed correctly
\retval false The pin wasn't changed because the map is not writable or the
	given index is out of bound
*/
bool cMap::changePin(sPoint p, uint32_t pin)
{
	if (pin > pinData.size() || !(flags & flagWritable)) return false;
	pinData[pin - 1] = sPoint(x,y);
        return true;
}

/*!
\brief Removes the given pin
\param pin Index to the pin to remove
\retval true The pin was removed correctly
\retval false The pin wasn't removed (the index is 0, the index is greater than
	the pins' number, or the map isn't writable.
*/
bool cMap::removePin(uint32_t pin)
{
	if ((pin < 1) || (pin > pinData.size()) || !(flags & flagWritable)) return false;
        pinData.erase(pinData.begin() + pin - 1);
        return true;
}

/*!
\brief Removes all the pin from the map
\retval true The map is writable and now is clear
\retval flase The map wasn't cleared because it's not writable
*/
bool cMap::clearAllPins()
{
	if (!(flags & flagWritable)) return false;
	pinData.clear();
        return true;
}

/*!
\brief Toggle pin addability and replies to client actual writeability status
\param client Client which is toggling the writable flag (and so the one to send the packet)
*/
bool cMap::toggleWritable(pClient client)
{
	if (!isTreasureMap()) flags ^= flagWritable;
        else flags &= ~flagWritable;
	nPackets::Sent::MapPlotCourse pk(this, pccWriteableStatus, writeable, 0, 0);
	client->sendPacket(&pk);
        return true;
}

/*!
\brief Execute the double click of a client on the map
\param client Client performing the double click

This function overloads the cItem::doubleClicked() function for Cmap items and
tests for internal data to show and internal functions to call.

\li If the map is not a treasure map it plots the course of the pins in it.
\li If the map is a treasure map and is not deciphered, it calls the
decipher() function to decipher it.
\li If the map is a deciphere treasure map I actually don't know what it does :)

\todo Add documentation about the third case described over here.
*/
void cMap::doubleClicked(pClient client)
{
	if ( (flags & flagTreasure) && !(flags & flagDeciphered) ) {
		//! \todo redo when treasures redone
		decipher(client);
		return;
	}
	
	// This is common both for ITYPE_MAP and ITYPE_DECIPHERED_MAP
	nPackets::Sent::OpenMapGump pk(this);
	client->sendPacket(&pk);
	nPackets::Sent::MapPlotCourse pk2(this, pccClearAllPins); //Sending clear all pins command
	client->sendPacket(&pk2);
	
	//!\todo Replace mores with some decent variables
	if ( (flags & flagTreasure) && (flags & flagDeciphered) ) {
		// Generate message to add a map point
		uint16_t posx, posy;					// tempoary storage for map point
		uint16_t tlx, tly, lrx, lry;				// tempoary storage for map extends
		
		tlx = (more1.moreb1 << 8)  | more1.moreb2;
		tly = (more1.moreb3 << 8)  | more1.moreb4;
		lrx = (more2.moreb1 << 8) | more2.moreb2;
		lry = (more2.moreb3 << 8) | more2.moreb4;
		posx = (256 * (morex - tlx)) / (lrx - tlx);		// Generate location for point
		posy = (256 * (morey - tly)) / (lry - tly);
		
		nPackets::Sent::MapPlotCourse pk3(this, pccAddPin, 0,posx, posy);//Sending add pin command
		client->sendPacket(&pk3);
	} else {
		for(PointVector::iterator it = pinData.begin(); it != pinData.end(); it++)
		{
			nPackets::Sent::MapPlotCourse pki(this, pccAddPin, 0, (*it).x, (*it).y);
			client->sendPacket(&pki);
		}
	}
}

/*!
\brief Attempt to decipher a tattered map
\param client Client of the decipher

Called by cMap::doubleClicked() when called for a not deciphered treasure map.
*/
void nSkills::decipher(pClient client)
{
	if ( ! client ) return;
 	pChar pc = client->currChar();
	if ( ! pc ) return;

	char sect[512];         // Needed for script search
	int regtouse;           // Stores the region-number of the TH-region
	int i;                  // Loop variable
	int btlx, btly, blrx, blry; // Stores the borders of the tresure region (topleft x-y, lowright x-y)
	int tlx, tly, lrx, lry;     // Stores the map borders
	int x, y;                   // Stores the final treasure location
	cScpIterator* iter = NULL;
	char script1[1024];
	
	// Set the skill delay, no matter if it was a success or not
	SetTimerSec(&pc->skilldelay, nSettings::Skills::getSkillDelay() );
	pc->playSFX(0x0249);
	
	if( pc->skilldelay > getClockmSecs() && !pc->isGM()) // Char doin something?
	{
		client->sysmessage("You must wait to perform another action");
		return;
	}
    
	if ( ! pc->checkSkill( skCartography, morey * 10, 1000)) // Is the char skilled enaugh to decipher the map
	{
		client->sysmessage("You fail to decipher the map");
		return;
	}
	
	// Stores the new map
	pItem nmap=item::CreateFromScript( 70025, pc->getBackpack() );
	if (!nmap)
	{
		LogWarning("bad script item # 70025(Item Not found).");
		return; //invalid script item
	}

	nmap->setCurrentName("a deciphered lvl.%d treasure map", morez);   // Give it the correct name
	nmap->morez = morez;              // Give it the correct level
	nmap->creator = pc->getCurrentName();  // Store the creator


	sprintf(sect, "SECTION TREASURE %i", nmap->morez);

	iter = Scripts::Regions->getNewIterator(sect);

	if (iter == NULL)
	{
		LogWarning("Treasure hunting cMap::decipher : Unable to find 'SECTION TREASURE %d' in regions-script", nmap->morez);
		return;
	}
	strcpy(script1, iter->getEntry()->getFullLine().c_str());               // skip the {
	strcpy(script1, iter->getEntry()->getFullLine().c_str());               // Get the number of areas
	regtouse = rand()%str2num(script1); // Select a random one
	for (i = 0; i < regtouse; i++)      // Skip the ones before the correct one
	{
		strcpy(script1, iter->getEntry()->getFullLine().c_str());
		strcpy(script1, iter->getEntry()->getFullLine().c_str());
		strcpy(script1, iter->getEntry()->getFullLine().c_str());
		strcpy(script1, iter->getEntry()->getFullLine().c_str());
	}
	strcpy(script1, iter->getEntry()->getFullLine().c_str());
	btlx = str2num(script1);
	strcpy(script1, iter->getEntry()->getFullLine().c_str());
	btly = str2num(script1);
	strcpy(script1, iter->getEntry()->getFullLine().c_str());
	blrx = str2num(script1);
	strcpy(script1, iter->getEntry()->getFullLine().c_str());
	blry = str2num(script1);

	safedelete(iter);

	if ((btlx < 0) || (btly < 0) || (blrx > 0x13FF) || (blry > 0x0FFF)) // Valid region?
	{
		sprintf(sect, "Treasure Hunting cMap::decipher : Invalid region borders for lvl.%d , region %d", nmap->morez, regtouse+1);   // Give out detailed warning :D
		LogWarning(sect);
		return;
	}
	x = btlx + (rand()%(blrx-btlx));    // Generate treasure location
	y = btly + (rand()%(blry-btly));
	tlx = x - 250;      // Generate map borders
	tly = y - 250;
	lrx = x + 250;
	lry = y + 250;
	// Check if we are over the borders and correct errors
	if (tlx < 0)    // Too far left?
	{
		lrx -= tlx; // Add the stuff too far left to the right border (tlx is neg. so - and - gets + ;)
		tlx = 0;    // Set tlx to correct value
	}
	else if (lrx > 0x13FF) // Too far right?
	{
		tlx -= lrx - 0x13FF;    // Subtract what is to much from the left border
		lrx = 0x13FF;   // Set lrx to correct value
	}
	if (tly < 0)    // Too far top?
	{
		lry -= tly; // Add the stuff too far top to the bottom border (tly is neg. so - and - gets + ;)
		tly = 0;    // Set tly to correct value
	}
	else if (lry > 0x0FFF) // Too far bottom?
	{
		tly -= lry - 0x0FFF;    // Subtract what is to much from the top border
		lry = 0x0FFF;   // Set lry to correct value
	}
	nmap->more1 = tlx>>8;   // Store the map extends
	nmap->more2 = tlx%256;
	nmap->more3 = tly>>8;
	nmap->more4 = tly%256;
	nmap->moreb1 = lrx>>8;
	nmap->moreb2 = lrx%256;
	nmap->moreb3 = lry>>8;
	nmap->moreb4 = lry%256;
	nmap->morex = x;        // Store the treasure's location
	nmap->morey = y;
	Delete();    // Delete the tattered map

	client->sysmessage("You put the deciphered tresure map in your pack");
}
