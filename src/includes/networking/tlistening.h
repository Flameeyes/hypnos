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
\brief Listening server loop
*/

#ifndef __NETWORKING_TLISTENING_H__
#define __NETWORKING_TLISTENING_H__

#include "common_libs.h"
#include <wefts_thread.h>
#include <cabal_ssocket.h>

class tReceiving;

/*!
\class tListening tlistening.h "networking/tlistening.h"
\brief Listening server loop

This class is used to accept the incoming connections of the clients, spawning
a new receiving thread for every client connected.

The creation of an instance of this class will request to nSettings the values
for local hostname and local port for the server, and then creates the socket,
but only the running thread can accept connections.

Please also note that this thread will spawn in parallel a tKilling which
deletes threads instances for already closed sockets (see cReceiveThread).

tKilling must be killed by the server only when all the rest of the server
is completely shutdown.

\note This is a singleton.
*/
class tListening : public Wefts::Thread
{
friend class tReceiving;

private:
	Cabal::ServerSocket *sock;	//!< Server socket for the loop
	std::set<tReceiving *> threads;	//!< List of currently operating receiving thredas
	Wefts::Mutex threads_m;		//!< Mutex which prevents double access to cListenLoop::threads
public:
	tListening *instance;		//!< Instance of the singleton

	tListening();
	
	void *run();
};

#endif

