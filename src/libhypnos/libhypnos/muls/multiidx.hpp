/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPNOS_MULS_MULTIIDX_H__
#define __LIBHYPNOS_MULS_MULTIIDX_H__

#include "libhypnos/muls/mulfiles.h"
#include "libhypnos/muls/indexedfile.h"
#include "libhypnos/types.h"

namespace nLibhypnos {
namespace nMULFiles {

/*!
\brief Internal structure of multi.idx file
*/
class cMultiIDX
{
protected:
	uint32_t lookup;	//!< Offset to seek to to have the
				//!< gumpart.mul element
	uint32_t size;		//!< Size of the gumpart.mul element
	uint32_t unknown;	//!< Unknown value
public:
	inline uint32_t getLookup() const
	{ return mtohl(lookup); }
	
	inline uint32_t getSize() const
	{ return mtohl(size); }
} PACK_NEEDED;

/*!
\class fMultiIDX multiidx.h "libhypnos/muls/multiidx.h"
\brief Access class to multi.idx file

This class handles the access to the multi.idx file, which is the index for
multi.mul file.

\section multiidx_structure Structure of multi.idx file

The gumpidx.mul file is structured like this:
	\li \b dword lookup (offset of the multi data in multi.mul file)
	\li \b dword size (size of the multi data in multi.mul file)
	\li \b dword unknown

Information grabbed from WolfPack files formats guide.
*/
class fMultiIDX : public tplIndexFile<cMultiIDX>
{
protected:
public:
	fMultiIDX();
	fMultiIDX(const string &);
};

}}

#endif
