/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file 
\brief Implementation of class cEquippable
*/

#include "objects/citem/cequippable.h"
#include "objects/cchar.h"
#include "settings.h"

cEquippable::cEquippable()
	: cItem()
{
}

cEquippable::cEquippable(uint32_t newSerial)
	: cItem(newSerial)
{
	minEquippingStrength = 0;  // Minimum strength to equip item
        minEquippingIntelligence = 0; // Minimum inteligence to equip item
        minEquippingDexterity = 0; // Minimum dexterity to equip item
        minEquippingSkill[3]= { UINVALID16, UINVALID16, UINVALID16 }; // holds up to 3 skills to be checked to equip. if INVALID no skill check is done
        minEquippingSkillvalue[3] = {0,0,0};
}

cEquippable::~cEquippable()
{
}

/*!
\brief Overloaded function for set the container of a item
\param obj Container or body equipped with the item

This function overloads cItem::setContainer() function, which is called if the
container isn't a body.
*/
void cEquippable::setContainer(pObject obj)
{
	pBody body = NULL;
	if ( (body = dynamic_cast<pBody>(obj)) )
		body->setLayerItem(findLayerById(getId), this);
	else {
		layer = layNone;
		cItem::setContainer(obj);
	}
}

/*!
\brief Double click on an equippable item
\param client Client who performed the double click

This function will check if the server requires equip on double click and acts
as requested.
*/
void cEquippable::doubleClicked(pClient client)
{
	Layer lay = findLayerById(getId());
	bool pass = nSettings::Actions::shouldEquipOnDClick() &&
		// equip the item only if it is in the backpack of the player
		getContainer() == client->currChar()->getBody()->getBackpack() &&
		(lay != layBackpack && lay != layMount) &&
		this != client->currChar()->getBody()->getLayerItem(lay);
	
	
	if ( ! pass )
	{
		cItem::doubleClicked();
		return;
	}
	
	client->currChar()->getBody()->setLayerItem(lay, this);
}
