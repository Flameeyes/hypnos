/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declaration of cNPC class
*/

#ifndef __CNPC_H__
#define __CNPC_H__

class cNPC;
typedef cNPC *pNPC;

/*!
\brief This class represent a non-playing character.

In this class there are members used only for NPCs, and not for playing
characters.
*/
class cNPC : cChar
{
public:
	inline cNPC() : cChar()
	{ }

	inline cNPC(UI32 serial) : cChar(serial)
	{ }

protected:
	//! Heartbeat for NPCs
	void heartbeat();

};

#endif