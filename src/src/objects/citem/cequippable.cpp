/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file 
\brief Implementation of class cEquippable
*/

#include "objects/citem/cequippable.h"
#include "objects/cchar.h"

cEquippable::cEquippable()
	: cItem()
{
}

cEquippable::cEquippable(uint32_t newSerial)
	: cItem(newSerial)
{
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
	{
		findLayerFromID(getId());
		body->setLayerItem(layer, this);
	} else
		cItem::setContainer(obj);
}
