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
\brief Receiving thread
*/

#ifndef __NETWORKING_TRECEIVING_H__
#define __NETWORKING_TRECEIVING_H__

#include "common_libs.h"
#include <wefts_thread.h>
#include <cabal_tcpsocket.h>

/*!
\class tUOReceiver treceiving.h "networking/treceiving.h"
\brief Thread which receive data from a socket

This class is spawned by tListening when a new connection is accepted, and
takes care of receive the buffer and then call the right functions to mangle
it.
Instances of this class are deleted by tKilling thread, see tListening::run()
method for more information.
*/
class tUOReceiver : public Wefts::Thread
{
protected:
	Cabal::TCPSocket *sock;
public:
	tUOReceiver(Cabal::TCPSocket *aSock);
	
	void *run();
};

#endif
