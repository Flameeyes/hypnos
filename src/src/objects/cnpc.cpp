/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cNPC class
*/

#include "objects/cnpc.h"

/*!
\brief Sets the NPC's owner
\author Flameeyes
\param pc new owner
*/
void cNPC::setOwner(pChar pc)
{
	if ( owner )
		owner->removeOwned(this);

	owner = pc;

	if ( owner )
		owner->addOwned(this);
}
