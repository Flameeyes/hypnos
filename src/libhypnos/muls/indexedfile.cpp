/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypnos/muls/indexedfile.h"
#include "libhypnos/exceptions.h"

namespace nLibhypnos {
namespace nMULFiles {

	/*!
	\brief Gets the offset of the given record in the indexed file
	\param index ID of the record (index to search for). Zero-based.
	\return The offset of the requested record in the indexed file
	\throw eOutOfBound If the requested index is greater than the count of records
	*/
	template <class T>
	uint32_t tplIndexFile<T>::getLookup(uint16_t index) const
	{
		if ( index >= file.getCount() )
			throw eOutOfBound(file.getCount()-1, index);
		
		return getArray()[index].getLookup();
	}
	
	/*!
	\brief Gets the length of the given record in the indexed file
	\param index ID of the record (index to search for). Zero-based.
	\return The length of the requested record in the indexed file
	\throw eOutOfBound If the requested index is greater than the count of records
	*/
	template <class T>
	uint32_t tplIndexFile<T>::getSize(uint16_t index) const
	{
		if ( index >= file.getCount() )
			throw eOutOfBound(file.getCount()-1, index);
		
		return getArray()[index].getSize();
	}
	
} }
