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

	template<class cData> tplIndexFile<cData>::tplIndexFile<cData>(std::string filename)
		: tplMMappedFile<cData>(filename)
	{
	}	

	template<class cData> tplIndexedFile<cData>::tplIndexedFile<cData>(tplIndexFile<cData> *aIdx, std::string filename)
		: tplMMappedFile<cData>(filename), idx(aIdx)
	{
	}
	
	/*!
	\brief Gets the offset of the given record in the indexed file
	\param index ID of the record (index to search for). Zero-based.
	\return The offset of the requested record in the indexed file
	\throw eOutOfBound If the requested index is greater than the count of records
	*/
	template<class cData> uint32_t tplIndexFile<cData>::getLookup(uint16_t index) const
	{
		if ( index >= tplMMappedFile<cData>::getCount() )
			throw eOutOfBound(tplMMappedFile<cData>::getCount()-1, index);
		
		return tplMMappedFile<cData>::array[index].getLookup();
	}
	
	/*!
	\brief Gets the length of the given record in the indexed file
	\param index ID of the record (index to search for). Zero-based.
	\return The length of the requested record in the indexed file
	\throw eOutOfBound If the requested index is greater than the count of records
	*/
	template<class cData> uint32_t tplIndexFile<cData>::getSize(uint16_t index) const
	{
		if ( index >= tplMMappedFile<cData>::getCount() )
			throw eOutOfBound(tplMMappedFile<cData>::getCount()-1, index);
		
		return tplMMappedFile<cData>::array[index].getSize();
	}
	
} }
