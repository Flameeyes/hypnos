/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypnos/muls/cliloc.h"

namespace nLibhypnos {
namespace nMULFiles {

/*!
\brief Constructor for fCliloc class
\param filename File path and name of the cliloc file to open.
*/
fCliloc::fCliloc(std::string filename)
	: tplMMappedFile<char>(filename)
{
	char *ptr = array+6;
	
	char *end = (array+tplMMappedFile<char>::getCount());
	
	while(ptr < end)
	{
		uint32_t index = mtohl( *reinterpret_cast<uint32_t*>(ptr) );
		assert(ptr[4] == 0x00); // If it's not, need to check the format
		uint16_t size = mtohs( *reinterpret_cast<uint16_t*>(ptr+5) );
		
		char *buff = new char[size+1];
		strncmp(buff, ptr+7, size);
		buff[size] = '\0';
		
		entries[index] = std::string(buff);
		delete[] buff;
		
		ptr += 7+size;
	}
}

/*!
\brief Gets the entry for the given index
\param index Index of the entry to read
\return The entry read from the cliloc file
*/
const std::string &fCliloc::getEntry(uint32_t index)
{
	return entries[index];
}

/*!
\brief Gets the entries count

This function is overloaded because the internal count of tplMMappedFile::array
isn't valid for cliloc files (they are variable-lenght files).
*/
uint32_t fCliloc::getCount()
{
	return entries.size();
}

}}
