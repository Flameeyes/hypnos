/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\author Flameeyes
\brief RTTI Namespace, for identify which type of object is one
*/

#ifndef __RTTI_H__
#define __RTTI_H__

namespace rtti
{
	static const uint32_t cObject 		= 0x00000000; //! cObject class

	static const uint32_t cItem			= 0x00000001; //! cItem class
	static const uint32_t cContainer		= 0x00000011; //! cContainer class
        static const uint32_t cMap			= 0x00000021; //! cMap class
        static const uint32_t cMsgBoardMessage      = 0x00000031; //! cMsgBoardMessage class
        static const uint32_t cMsgBoard      	= 0x00000041; //! cMsgBoard class


	static const uint32_t cBody		= 0x00010000; //! cBody class

	static const uint32_t cChar		= 0x00100000; //! cChar class
	static const uint32_t cPC		= 0x01100000; //! cPC class
	static const uint32_t cNPC		= 0x10100000; //! cNPC class
}

#endif