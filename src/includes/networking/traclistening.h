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

#ifndef __NETWORKING_TRACLISTENING_H__
#define __NETWORKING_TRACLISTENING_H__

#include "common_libs.h"
#include <wefts_thread.h>
#include <cabal_ssocket.h>

class tRemoteAdmin;

/*!
\class tRACListening traclistening.h "networking/traclistening.h"
\brief Listening remote administration console loop

This thread listens for new requests for remote access, then spawns a new
tRemoteAdmin thread which takes care of parsing user input.
*/
class tRACListening : public Wefts::Thread
{
friend class tRemoteAdmin;

private:
	Cabal::ServerSocket *sock;	//!< Server socket for the loop
	std::set<tReceiving *> threads;	//!< List of currently operating receiving thredas
	Wefts::Mutex threads_m;		//!< Mutex which prevents double access to cListenLoop::threads
public:
	static tRACListening *instance;

	tRACListening();
	
	void *run();
};

#endif
