/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypnos/muls/tiledata.h"
#include "libhypnos/exceptions.h"

namespace nLibhypnos {
namespace nMULFiles {

//! Gets the name of the entry
std::string cTiledataLand::cEntry::getName() const
{
	char tmp[21];
	strncpy(tmp, name, 20);
	
	return std::string(tmp);
}

//! Gets the name of the entry
std::string cTiledataStatic::cEntry::getName() const
{
	char tmp[21];
	strncpy(tmp, name, 20);
	
	return std::string(tmp);
}

fTiledataLand::fTiledataLand()
	: tplMMappedFile<cTiledataLand>(nMULFiles::getMULpath() + "tiledata.mul", 0, 512*sizeof(cTiledataLand))
{
}

fTiledataStatic::fTiledataStatic()
	: tplMMappedFile<cTiledataStatic>()
{
	open(nMULFiles::getMULpath() + "tiledata.mul");
	
	mmap(512*sizeof(cTiledataLand));
}

}}
