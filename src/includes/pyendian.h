  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/*!
\file
\brief Endian Abstraction level
\author Endymion
\note splitted from unicode.cpp by Akron
*/

#ifndef __ENDIAN_H__
#define __ENDIAN_H__

#include "nxwcommn.h"

/*
\brief endian-independent unsigned int 16 bits
\author Endymion
*/
class euint16_t {
	private:
		uint16_t a;
	public:
		euint16_t();
		euint16_t(uint16_t v);
		euint16_t& operator =( uint16_t v );
		euint16_t& operator=( euint16_t v );
		uint16_t get( );

} PACK_NEEDED;


/*
\brief endian-independent unsigned int 32 bits
\author Endymion
*/
class euint32_t {
	private:
		uint32_t a;
	public:
		euint32_t();
		euint32_t(uint32_t v);
		euint32_t& operator =( uint32_t v );
		euint32_t& operator=( euint32_t v );
		uint32_t get( );

} PACK_NEEDED;

typedef uint8_t euint8_t;		//!< fake endian-independent uint8_t
typedef int8_t eint8_t;		//!< fake endian-independent int8_t
typedef bool eBool;		//!< fake endian-independent bool
typedef euint16_t eCOLOR;	//!< endian-independent color
typedef euint32_t eSERIAL;	//!< endian-independent serial

#endif
