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

#ifndef __NETWORKING_TRACRECEIVER_H__
#define __NETWORKING_TRACRECEIVER_H__

#include "common_libs.h"
#include <wefts_thread.h>
#include <cabal_tcpsocket.h>

/*!
\class tRACReceiver tracreceiver.h "networking/tracreceiver.h"
\brief Thread which receive data from a socket

This class is spawned by tRemoteAdmin when a new connection is accepted, and
takes care of receive the buffer and then call the right functions to mangle
it.
Instances of this class are deleted by tKiller thread, see tRACReceiver::run()
method for more information.
*/
class tRACReceiver : public tReceiver
{
protected:
	Cabal::TCPSocket *sock;
public:
	tRACReceiver(Cabal::TCPSocket *aSock);
	
	void *run();
};

typedef tplListener<tRACReceiver> tRemoteAdmin;

#endif
