/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPMUL_GUMPIDX_H__
#define __LIBHYPMUL_GUMPIDX_H__

#include "libhypnos/muls/mulfiles.h"
#include "libhypnos/muls/mmappedfile.h"

/*!
\brief Internal structure of gumpidx.mul file
*/
class nMULFiles::cGumpIDX
{
protected:
	uint32_t lookup;	//!< Offset to seek to to have the
				//!< gumpart.mul element
	uint32_t size;		//!< Size of the gumpart.mul element
	uint16_t height;	//!< Height of the gump
	uint16_t width;		//!< Width of the gump
public:
	inline uint32_t getLookup() const
	{ return mtohl(lookup); }
	
	inline uint32_t getSize() const
	{ return mtohl(size); }
	
	inline uint32_t getHeight() const
	{ return mtohs(height); }
	
	inline uint32_t getWidth() const
	{ return mtohs(width); }
} PACK_NEEDED;

/*!
\class fGumpIDX gumpidx.h "libhypnos/muls/cgumpidx.h"
\brief Access class to gumpidx.mul file

This class handles the access to the gumpidx.mul file, which contains not only
the indexes for gumpart.mul file, but also the sizes of the gumps

\section gumpidx_structure Structur of gumpidx.mul file

The gumpidx.mul file is structured like this:
	\li \b dword lookup (offset of the gump data in  gumpart.mul file)
	\li \b dword size (size of the gump data in gumpart.mul file)
	\li \b word height (height of the gump in pixels)
	\li \b word width (width of the gump in pixels)

Information grabbed from Alazane's UO File Formats guide.
*/
class nMULFiles::fGumpIDX : public tMMappedFile<cGumpIDX>
{
protected:
public:
	fGumpIDX();
	~fGumpIDX();
	
	sPoint getDimensions(uint16_t index);
};

#endif
