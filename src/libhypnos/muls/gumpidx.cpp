/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypnos/muls/gumpidx.h"
#include "libhypnos/exceptions.h"
#include "structs.h"

namespace nLibhypnos {
namespace nMULFiles {

/*!
\brief Constructor

This function simply calls the tplMMappedFile constructor with the path given by
getMULpath() function and the filename \c gumpidx.mul .

*/
fGumpIDX::fGumpIDX()
	: tplMMappedFile<cGumpIDX>( nMULFiles::getMULpath() + "gumpidx.mul" )
{
}

fGumpIDX::~fGumpIDX()
{
}

/*!
\brief Gets the dimension for the given gump
\param index ID of the gump (index to search for). Zero-based.
\return A point with the maximum dimensions of the gump.
*/
sPoint fGumpIDX::getDimensions(uint16_t index)
{
	if ( index >= getCount() )
		throw eOutOfBound(getCount()-1, index);
	
	return sPoint(array[index].getHeight(), array[index].getWidth());
}

}}
