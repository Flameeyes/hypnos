/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "networking/tracreceiver.h"
#include "networking/tkilling.h"

/*!
\brief Constructor for tRACReceiver thread

This function register the thread in tRemoteAdmin::threads set and also starts
the thread itself.
*/
tRACReceiver::tRACReceiver(Cabal::TCPSocket *aSock) : tReceiver(aSock)
{
	// Lock the threads set
	tRemoteAdmin::instance->threads_m.lock();
	threads.insert(this);
	tRemoteAdmin::instance->threads_m.unlock();
	// Unlock the threads set
	
	start();
}

/*!
\brief Receiving loop function

This function does all the dirt work for tRACReceiver thread, getting lines and
parsing them.

This function also register the socket as dead in tKiller after the socket is
closed.

\todo Missing all the work in this :)
*/
void *tRACReceiver::run()
{
	if ( ! sock ) return NULL;
	
	while ( ! sock->closed() )
	{
		
	}
	
	// Lock the threads set
	tRemoteAdmin::instance->threads_m.lock();
	threads.erase(this);
	tRemoteAdmin::instance->threads_m.unlock();
	// Unlock the threads set
	
	tKiller::instance->deadSockets->push(this);
}
