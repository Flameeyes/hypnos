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
#include "networking/tpllistener.h"

#include <wefts_thread.h>
#include <cabal_tcpsocket.h>

/*!
\class tUOReceiver tuoreceiver.h "networking/tuoreceiver.h"
\brief Thread which receive data from a socket

This class is spawned by tUOListener when a new connection is accepted, and
takes care of receive the buffer and then call the right functions to mangle
it.
Instances of this class are deleted by tKiller thread, see tUOReceiver::run()
method for more information.
*/
class tUOReceiver : public tReceiver
{
protected:
	Cabal::TCPSocket *sock;
public:
	tUOReceiver(Cabal::TCPSocket *aSock);
	
	void *run();
};

typedef tplListener<tUOReceiver> tUOListener;

#endif
