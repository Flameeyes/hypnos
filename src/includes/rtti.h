  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*!
\author Flameeyes
\brief RTTI Namespace, for identify which type of object is one
*/

#ifndef __RTTI_H__
#define __RTTI_H__

namespace rtti
{
	static const UI32 cObject 	= 0x00000000; //! cObject class

	static const UI32 cItem		= 0x00000001; //! cItem class
	static const UI32 cContainer	= 0x00000011; //! cContainer class

	static const UI32 cBody		= 0x00010000; //! cBody class

	static const UI32 cChar		= 0x00100000; //! cChar class
	static const UI32 cPC		= 0x01100000; //! cPC class
	static const UI32 cNPC		= 0x10100000; //! cNPC class
}

#endif