/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "networking/treceiving.h"
#include "networking/tkilling.h"

/*!
\brief Constructor for tUOReceiver thread

This function register the thread in tListening::threads set and also starts
the thread itself.
*/
tUOReceiver::tUOReceiver() : Wefts::Thread(), sock(aSock)
{
	// Lock the threads set
	tListening::instance->threads_m.lock();
	
	threads.insert(this);
	
	tListening::instance->threads_m.unlock();
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
	tListening::instance->threads_m.lock();
	
	threads.erase(this);
	
	tListening::instance->threads_m.unlock();
	// Unlock the threads set
	
	tKiller::instance->deadSockets->push(this);
}
