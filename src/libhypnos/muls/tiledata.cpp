/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypmul/tiledata.h"

std::string nMULFiles::cTiledataLand::cEntry::getName()
{
	char tmp[21];
	strncpy(tmp, name, 20);
	
	return std::string(tmp);
}

std::string nMULFiles::cTiledataStatic::cEntry::getName()
{
	char tmp[21];
	strncpy(tmp, name, 20);
	
	return std::string(tmp);
}

nMULFiles::fTiledataLand::fTiledataLand()
	: tMMappedFile<cTiledataLand>(nMULFiles::getMULpath() + "tiledata.mul", 0, 512*sizeof(cTiledataLand))
{
}

nMULFiles::fTiledataStatic::fTiledataStatic()
	: tMMappedFile<cTiledataStatic>()
{
	open(nMULFiles::getMULpath() + "tiledata.mul");
	
	mmap(512*sizeOf(cTiledataLand));
}
