/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPNOS_MULS_INDEXEDFILE_H__
#define __LIBHYPNOS_MULS_INDEXEDFILE_H__

#include "libhypnos/muls/mulfiles.h"
#include "libhypnos/muls/mmappedfile.h"
#include "libhypnos/exceptions.h"
#include "libhypnos/types.h"

namespace nLibhypnos {
namespace nMULFiles {

template<class cData> class tplIndexFile
{
protected:
	cMMappedFile file;
	
	cData *getArray() const
	{ return reinterpret_cast<cData*>(file.getArray()); }
public:
	tplIndexFile(std::string filename)
		: file(sizeof(cData), filename)
	{ }
	
	/*!
	\brief Gets the offset of the given record in the indexed file
	\param index ID of the record (index to search for). Zero-based.
	\return The offset of the requested record in the indexed file
	\throw eOutOfBound If the requested index is greater than the count of records
	*/
	uint32_t getLookup(uint16_t index) const
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
	uint32_t getSize(uint16_t index) const
	{
		if ( index >= file.getCount() )
			throw eOutOfBound(file.getCount()-1, index);
		
		return getArray()[index].getSize();
	}
};

/*!
\brief Class for handle indexed mul files.

Indexed mul files are special mul files whith variable-lenght records. To
have random access to the records, Ultima OnLine uses an index file, which
is usually named as the main mul file with extension .idx.

This class uses an index file to look up the actual address of the record.
This is used by many different classes, such as cMulti.
*/
template<class cData> class tplIndexedFile
{
protected:
	cMMappedFile file;
	tplIndexFile<cData> *idx;	//!< Index file to use
public:
	tplIndexedFile(tplIndexFile<cData> *aIdx, std::string filename)
		: file(1, filename), idx(aIdx)
	{ }
	
	/*!
	\brief Gets the memory address for the record at the given index
	\param id Index of the record to find out
	\return A void-pointer to the start of the requested record
	\throw eOutOfBound If the index is greater than the count of items in
		index file (is actually thrown by tplIndexFile::getLookup() )
	*/
	inline void *getAddress(uint16_t id)
	{ return reinterpret_cast<char*>(file.getArray()) + idx->getLookup(id); }

	/*!
	\brief Gets the size of the record at the given index
	\param id Index of the record to find the size of
	\return The lenght in bytes of the requested record
	\throw eOutOfBound If the index is greater than the count of items in
		index file (is actually thrown by tplIndexFile::getSize() )
	*/
	inline uint32_t getSize(uint16_t id)
	{ return idx->getSize(id); }
};

}}

#endif
