/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypnos/muls/gumpidx.h"
#include "libhypnos/exceptions.h"

namespace nLibhypnos {
namespace nMULFiles {

/*!
\brief Constructor

This function simply calls the tplMMappedFile constructor with the path given by
getMULpath() function and the filename \c gumpidx.mul .

*/
fGumpIDX::fGumpIDX()
	: tplIndexFile<cGumpIDX>( nMULFiles::getMULpath() + "gumpidx.mul" )
{
}

/*!
\brief Gets the dimension for the given gump
\param index ID of the gump (index to search for). Zero-based.
\return A point with the maximum dimensions of the gump.
\throw eOutOfBound If the requested index is greater than the count of records
*/
sPoint fGumpIDX::getDimensions(uint16_t index)
{
	if ( index >= file.getCount() )
		throw eOutOfBound(file.getCount()-1, index);
	
	return sPoint(getArray()[index].getHeight(), getArray()[index].getWidth());
}

}}
