/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypnos/muls/multi.h"
#include "libhypnos/exceptions.h"
#include "structs.h"

namespace nLibhypnos {
namespace nMULFiles {

/*!
\brief Constructor

This function simply calls the tplIndexedFile constructor with the path given by
getMULpath() function and the filename \c multi.mul .

*/
fMulti::fMulti()
	: tplIndexedFile<cMultiIDX>( nMULFiles::getMULpath() + "multi.mul" )
{
}

/*!
\brief Gets the item id for the given multi block item
\param index Index of the item in the list. Zero-based
\return The item id for the given multi block item
\throw eOutOfBound If the requested index is greater than the count of items
*/
uint16_t cMultiItem::getItemID(uint16_t index)
{
	if ( index >= getCount() )
		throw eOutOfBound(getCount()-1, index);
	
	return items[index].getItemID();
}

/*!
\brief Gets the x-coord for the given multi block item
\param index Index of the item in the list. Zero-based
\return The x coord for the given multi block item
\throw eOutOfBound If the requested index is greater than the count of items
*/
uint16_t cMultiItem::getX(uint16_t index)
{
	if ( index >= getCount() )
		throw eOutOfBound(getCount()-1, index);
	
	return items[index].getX();
}

/*!
\brief Gets the y-coord for the given multi block item
\param index Index of the item in the list. Zero-based
\return The y coord for the given multi block item
\throw eOutOfBound If the requested index is greater than the count of items
*/
uint16_t cMultiItem::getY(uint16_t index)
{
	if ( index >= getCount() )
		throw eOutOfBound(getCount()-1, index);
	
	return items[index].getX();
}

/*!
\brief Gets the z-coord for the given multi block item
\param index Index of the item in the list. Zero-based
\return The z coord for the given multi block item
\throw eOutOfBound If the requested index is greater than the count of items
*/
uint16_t cMultiItem::getZ(uint16_t index)
{
	if ( index >= getCount() )
		throw eOutOfBound(getCount()-1, index);
	
	return items[index].getX();
}

/*!
\brief Gets the flags for the given multi block item
\param index Index of the item in the list. Zero-based
\return The flags for the given multi block item
\throw eOutOfBound If the requested index is greater than the count of items
*/
uint16_t cMultiItem::getFlags(uint16_t index)
{
	if ( index >= getCount() )
		throw eOutOfBound(getCount()-1, index);
	
	return items[index].getFlags();
}

}}
