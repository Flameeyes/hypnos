/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Killing dead sockets
*/

#ifndef __NETWORKING_TKILLING_H__
#define __NETWORKING_TKILLING_H__

#include "common_libs.h"
#include <wefts_thread.h>

class tReceiver;

/*!
\class tKiller tkilling.h "networking/tkilling.h"
\brief Thread killing dead sockets

This class is used to cleanup the sockets which are closed by the server (for
timeout, errors, or requested disconnection), both for UO and RAC sockets.

It has a FIFO of sets to delete, the thread blocks for a new element, then
takes it, joins it and then delete it (the joining is to avoid still-running
deletions).

\note This class is a singleton
*/
class tKiller : public Wefts::Thread
{
friend class tUOReceiver;

protected:
	Wefts::Queue<tReceiver *> deadSockets;	//!< Sockets to delete
public:
	static tKiller *instance;		//!< Instance of the thread (is a singleton)

	tKiller();
	
	void *run();
};

#endif
