/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPNOS_MULS_CLILOC_H__
#define __LIBHYPNOS_MULS_CLILOC_H__

#include "libhypnos/muls/mulfiles.h"
#include "libhypnos/muls/mmappedfile.h"
#include "libhypnos/types.h"

namespace nLibhypnos {
namespace nMULFiles {

/*!
\brief Client Locales data handlign class

This class handles the cliloc* files, which are used to send localized data to
the client.
At the moment the class handles only read-only support, in the future I'll add
interfaces to load all the data from the file in memory and allow to store it
in the file for edit it.

The format of the cliloc files, took from Steamengine's
(http://steamengine.sf.net/) cliloc editor, is this:

At the start of the file:
byte[6] unknown: format and version IDs?

For each entry:
uint entryID
byte 00
ushort textLen
char[textLen] text (not null terminated)
*/
class fCliloc
{
public:
	fCliloc(std::string filepath);
	
	const std::string &getEntry(uint32_t index);
	uint32_t getCount();
protected:
	cMMappedFile file;
	stringVector entries;
};

}}

#endif
