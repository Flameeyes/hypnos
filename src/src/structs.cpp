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

#include "structs.hpp"
#include "objects/citem.hpp"
#include "data.hpp"
#include "inlines.hpp"

/*!
\brief Moves a location
\param dir Direction where to move the location
\param paces Number of paces to move the location

This function is used to calculate the direction of a movement (like in
cBoat::step() ) from a given location.
*/
void sLocation::move(Direction dir, uint8_t paces)
{
	switch(dir)
	{
	case dirNorth:
		y -= paces;
		break;
	case dirNorthEast:
		x += paces;
		y -= paces;
		break;
	case dirEast:
		x += paces;
		break;
	case dirSouthEast:
		x += paces;
		y += paces;
		break;
	case dirSouth:
		y += paces;
		break;
	case dirSouthWest:
		x -= paces;
		y += paces;
		break;
	case dirWest:
		x -= paces;
		break;
	case dirNorthWest:
		x -= paces;
		y -= paces;
		break;
	}
}

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
\brief Constructor for a sRect between two points
*/
sRect::sRect(uint16_t ulx, uint16_t uly, uint16_t brx, uint16_t bry)
{
	ul.x = qmin(ulx, brx);
	ul.y = qmin(uly, bry);
	br.x = qmax(ulx, brx);
	br.y = qmax(uly, bry);
}

/*!
\brief Checks if a given point is inside the rectangle
\param p Point to check if inside the rectangle
\retval true The point is inside the box described by the rectangle
\retval false The point is outside the box described by the rectangle
*/
bool sRect::isInside(sPoint p) const
{
	return between(p.x, ul.x, br.x) && between(p.y, ul.y, br.y);
}
