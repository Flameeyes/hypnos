/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "networking/tkilling.h"
#include "networking/treceiving.h"

tKilling *tKilling::instance = NULL;

/*!
\brief Constructor for tKilling thread
\todo Throw an exception of there's already an instance
*/
tKilling::tKilling() : Wefts::Thread()
{
	instance = this;
}

/*!
\brief Loop function for killing thread

This function does the dirt work for tKilling thread, deleting the dead socket-
handling threads.
*/
void *tKilling::run()
{
	tUOReceiver *dead = NULL;
	
	while(1)
	{
		deadSockets.pop(dead);
		if ( ! dead ) continue;
		
		dead.join(); // Avoid still-running deletions. Hope not to go
		             // in a infinite loop event.
		
		delete dead;
	}
}
