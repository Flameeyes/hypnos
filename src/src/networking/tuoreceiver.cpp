/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "networking/tuoreceiver.h"
#include "networking/tkiller.h"

/*!
\brief Constructor for tUOReceiver thread

This function register the thread in tUOListener::threads set and also starts
the thread itself.
*/
tUOReceiver::tUOReceiver(Cabal::TCPSocket *aSock) : tReceiver(aSock)
{
	// Lock the threads set
	tUOListener::instance->threads_m.lock();
	threads.insert(this);
	tUOListener::instance->threads_m.unlock();
	// Unlock the threads set
	
	start();
}

/*!
\brief Receiving loop function

This function does all the dirt work for tUOReceiver thread, looping until the
socket is closed, receiving data in the buffer and transforming it into
packets to be executed.

This function also register the socket as dead in tKiller after the socket is
closed.

\todo Missing all the work in this :)
*/
void *tUOReceiver::run()
{
	if ( ! sock ) return NULL;
	
	while ( ! sock->closed() )
	{
		
	}
	
	// Lock the threads set
	tUOListener::instance->threads_m.lock();
	threads.erase(this);
	tUOListener::instance->threads_m.unlock();
	// Unlock the threads set
	
	tKiller::instance->deadSockets->push(this);
}
