/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPNOS_MULS_MULTI_H__
#define __LIBHYPNOS_MULS_MULTI_H__

#include "libhypnos/muls/mulfiles.h"
#include "libhypnos/muls/indexedfile.h"
#include "libhypnos/muls/multiidx.h"
#include "libhypnos/types.h"

namespace nLibhypnos {
namespace nMULFiles {

/*!
\brief Internal structure of a multi block
*/
class cMultiItem
{
protected:
	uint16_t itemid;	//!< ID of the item
	uint16_t x;		//!< X-position (referred to 0,0,0) of the item
	uint16_t y;		//!< X-position (referred to 0,0,0) of the item
	uint16_t z;		//!< X-position (referred to 0,0,0) of the item
	uint32_t flags;		//!< Flags of the item (if 1, is visible)
public:
	inline uint16_t getItemID() const
	{ return mtohs(itemid); }
	
	inline uint16_t getX() const
	{ return mtohs(x); }
	
	inline uint16_t getY() const
	{ return mtohs(y); }
	
	inline uint16_t getZ() const
	{ return mtohs(z); }
	
	inline uint32_t getFlags() const
	{ return mtohs(flags); }
} PACK_NEEDED;

/*!
\brief Multi block into multi.mul file

This class is used to access the elements of the multi block. It contains a
pointer to the mmapped file (so can be invalidated if the file is unmmapped),
and a size delimiter, which is tested when seeking data to avoid going after
the single block.
*/
class cMultiBlock
{
protected:
	cMultiItem *items;
	uint32_t size;
public:
	cMultiBlock(cMultiItem *aItems, uint32_t aSize)
		: items(aItems), size(aSize)
	{ }
	
	uint32_t getCount() const
	{ return size/sizeof(cMultiItem); }
	
	const cMultiItem *getItem(uint16_t id) const;
	
	uint16_t getItemID(uint16_t id) const;
	uint16_t getX(uint16_t id) const;
	uint16_t getY(uint16_t id) const;
	uint16_t getZ(uint16_t id) const;
	uint32_t getFlags(uint16_t id) const;
};

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
class fMulti : public tplIndexedFile<cMultiIDX>
{
public:
	fMulti(fMultiIDX *aIdx);

	/*!
	\brief Gets the multi block at the given id.
	\param id Index of the multi block to get
	\return An instance for the multi block requested
	\throw eOutOfBound If the id is greater than the count of indexed blocks (is
		actually thrown by tplIndexedFile::getAddress() ).
	*/
	cMultiBlock getBlock(uint16_t id)
	{ return cMultiBlock(reinterpret_cast<cMultiItem*>(getAddress(id)), getSize(id)); }
};

}}

#endif
