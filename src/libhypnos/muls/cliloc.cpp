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

#include <iostream>

namespace nLibhypnos {
namespace nMULFiles {

/*!
\brief Constructor for fCliloc class
\param filename File path and name of the cliloc file to open.
*/
fCliloc::fCliloc(std::string filename)
	: file(1, filename)
{
	std::cerr << file.getArray() << '\t' << file.getSize() << std::endl;

	char *ptr = reinterpret_cast<char*>(file.getArray())+6;
	
	char *end = (reinterpret_cast<char*>(file.getArray())+file.getCount());
	
	while(ptr < end)
	{
		uint32_t index = mtohl( *reinterpret_cast<uint32_t*>(ptr) );
		assert(ptr[4] == 0x00); // If it's not, need to check the format
		uint16_t size = mtohs( *reinterpret_cast<uint16_t*>(ptr+5) );
		
		std::cerr << index << '\t' << size << '\t';
		
		char *buff = new char[size+1];
		memcpy(buff, ptr+7, size);
		buff[size] = '\0';
		
		std::cerr << buff << '\t';
		
		entries[index] = std::string(buff);
		delete[] buff;
		
		std::cerr << entries.size() << std::endl;
		
		ptr += 7+size;
	}
}

}}
