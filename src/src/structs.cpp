/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Contains constructors and functions for structs defined in structs.h
*/

#include "structs.h"
#include "objects/citem.h"
#include "constants.h"

/*!
\brief Quick constructor passing an item pointer
\param item Item to add to the container's item list
*/
sContainerItem::sContainerItem(pItem item)
{
	serial	= item->getSerial();
	id	= item->getAnimId();
	amount	= item->getAmount();
	x	= item->getPosition().x;
	y	= item->getPosition().y;
	color	= item->getColor();
}

sSkillInfo::sSkillInfo()
{
	st = 0;
	dx = 0;
	in = 0;
	flags = 0;
	advanceIndex = 0;
	madeword = strNull;
}
