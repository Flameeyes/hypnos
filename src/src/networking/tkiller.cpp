/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "networking/tkiller.hpp"
#include "networking/tpllistener.hpp"

tKiller *tKiller::instance = NULL;

/*!
\brief Constructor for tKiller thread
\todo Throw an exception of there's already an instance
*/
tKiller::tKiller() : Wefts::Thread()
{
	instance = this;
}

/*!
\brief Loop function for killing thread

This function does the dirt work for tKiller thread, deleting the dead socket-
handling threads.
*/
void *tKiller::run()
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
