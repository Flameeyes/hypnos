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
#include "data.h"
#include "inlines.h"

/*!
\brief Quick constructor passing an item pointer
\param item Item to add to the container's item list
*/
sContainerItem::sContainerItem(pItem item)
{
	serial	= item->getSerial();
	id	= item->getId();
	amount	= item->getAmount();
	x	= item->getPosition().x;
	y	= item->getPosition().y;
	color	= item->getColor();
}

/*!
\brief Default constructor for sRect

The rectangle created with this constructor is the entire map
*/
sRect::sRect() :
	ul(0,0), br(map_width*8, map_height*8)
{
}

/*!
\brief Constructor for a sRect between two points
*/
sRect::sRect(sPoint a, sPoint b)
{
	ul.x = qmin(a.x, b.x);
	ul.y = qmin(a.y, b.y);
	br.x = qmax(a.x, b.x);
	br.y = qmax(a.y, b.y);
}

/*!
\brief Checks if a given point is inside the rectangle
\param p Point to check if inside the rectangle
\return true if the point is internal, else false
*/
bool sRect::isInside(sPoint p)
{
	return between(p.x, ul.x, br.x) && between(p.y, ul.y, br.y);
}
