/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPNOS_MULS_MULFILES_H__
#define __LIBHYPNOS_MULS_MULFILES_H__

#include "libhypnos/commons.h"

namespace nLibhypnos {

/*!
\namespace nMULFiles
\brief MUL files handling

This namespace contains the classes and the function to handle the MUL files and
their indexes.

The MUL files are Ultima OnLine's datafiles and are used to define everything
the client can see on the screen, it's skills and so on.

Hypnos uses only some of the MUL files used by the client, becase we need only
a part of all the data the client uses. Some files are also mmapped into the
memory to access them quickly, for example we mmap all the index files (see \ref
mulindexes).

\note The classes which has the f prefix are used to identify the file-access' 
	classes

\section mulindexes MUL files' indexes

The MUL files are binary files, some of these have fixed-length records, others
have variable-length records. Because the access to variable-length records is
quite difficult in random order, Ultima OnLine uses some indexes files, which we
use too to access those files.

Because the indexes aren't so big and all have fixed-length records, we can mmap
those we use to access them quickly.

\see tplMMappedFile for more information about the memory mapping stuff

\section mulendianness MUL files' endianness

Ultima OnLine is released only for Windows, which runs only on x86 compatible
machines, so all the data inside the MUL files is wrote in little endian. Hypnos
instead must run also on other platforms, like PPC, which are big endian. To
solve this, we use two functions similar to ntohs and ntohl, which converts
between the network (big) endian to the host endian (little on x86, big on
others).
Those functions are the mtohs() and mtohl(). The \c m stands for Mul-endianness.

*/
namespace nMULFiles {
#ifdef __BIG_ENDIAN__
	inline uint32_t mtohl(uint32_t dword)
	{
		uint32_t ret;
		uint8_t *pRet = (uint8_t*)&ret;
		
		pRet[0] = (dword >> 24);
		pRet[1] = (dword >> 16) & 0xFF;
		pRet[2] = (dword >> 8) & 0xFF;
		pRet[3] = dword & 0xFF;
		
		return ret;
	}

	inline uint16_t mtohs(uint16_t word)
	{
		uint16_t ret;
		uint8_t *pRet = (uint8_t*)&ret;
		
		pRet[0] = word >> 8;
		pRet[1] = word & 0xFF;
		
		return ret;
	}
#else
	#define mtohl(x) x
	#define mtohs(x) x
#endif
	
	void setMULpath(std::string path);
	std::string getMULpath();

}}

#endif
