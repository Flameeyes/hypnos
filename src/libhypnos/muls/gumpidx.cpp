/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypmul/gumpidx.h"
#include "structs.h"

/*!
\brief Constructor

This function simply calls the tMMappedFile constructor with the path given by
getMULpath() function and the filename \c gumpidx.mul .

*/
nMULFiles::fGumpIDX::fGumpIDX()
	: tMMappedFile<cGumpIDX>( nMULFiles::getMULpath() + "gumpidx.mul" )
{
}

nMULFiles::fGumpIDX::~fGumpIDX()
{
}

/*!
\brief Gets the dimension for the given gump
\param index ID of the gump (index to search for). Zero-based.
\return A point with the maximum dimensions of the gump.
*/
sPoint nMULFiles::fGumpIDX::getDimensions(uint16_t index)
{
	if ( index >= getCount() )
	{
		/*!
		\todo Here we should throw an out-of-bound exception when it's
		done :D
		*/
		return InvalidCoord;
	}
	
	return sPoint(array[index].getHeight(), array[index].getWidth());
}

