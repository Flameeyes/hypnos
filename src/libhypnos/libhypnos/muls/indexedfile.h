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
#include "libhypnos/mmappedfile.h"
#include "libhypnos/types.h"

namespace nLibhypnos {
namespace nMULFiles {

template<class cData> class tplIndexFile : public tplMMappedFile<cData>
{
public:
	tplIndexFile(std::string filename);
	virtual ~tplIndexFile();
	
	uint32_t getLookup(uint16_t index) const;
	uint32_t getSize(uint16_t index) const;
};

/*!
\brief Class for handle indexed mul files.

Indexed mul files are special mul files whith variable-lenght records. To
have random access to the records, Ultima OnLine uses an index file, which
is usually named as the main mul file with extension .idx.

This class uses an index file to look up the actual address of the record.
This is used by many different classes, such as cMulti.
*/
template<class cData> class tplIndexedFile : public tplMMappedFile<char>
{
protected:
	tplIndexFile<cData> *idx;	//!< Index file to use
public:
	tplIndexedFile(tplIndexFile<cData> *idx, std::string filename);
	virtual ~tplIndexedFile()
	{ }
	
	/*!
	\brief Gets the memory address for the record at the given index
	\param id Index of the record to find out
	\return A void-pointer to the start of the requested record
	\throw eOutOfBound If the index is greater than the count of items in
		index file (is actually thrown by tplIndexFile::getLookup() )
	*/
	inline void *getAddress(uint16_t id)
	{ return tplMMappedFile<char>::array + idx->getLookup(id); }

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
