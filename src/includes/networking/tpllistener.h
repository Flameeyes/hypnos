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
\brief Listening loop
*/

#ifndef __NETWORKING_TPLLISTENER_H__
#define __NETWORKING_TPLLISTENER_H__

#include "common_libs.h"
#include <wefts_thread.h>
#include <cabal_ssocket.h>

class tReceiver;

/*!
\class tplListener tpllistener.h "networking/tpllistener.h"
\brief Abstraction for listening threads

This template abstractize the listening threads, like the old tUOListener and
tRACListening threads (from SVN revision 704), using a single template to do
all the work.
*/
template<class tHandler> class tplListener : public Wefts::Thread
{
friend class tReceiver;

private:
	Cabal::ServerSocket *sock;	//!< Server socket for the loop
	std::set<tHandler *> threads;	//!< List of currently operating receiving threads
	Wefts::Mutex threads_m;		//!< Mutex which prevents double access to tplListener::threads
public:
	static tplListener *instance;

	tplListener(std::string localHost, uint16_t localPort);
	~tplListener();
	
	void *run();
	
	void closeServer()
	{ sock->close(); }
};

/*!
\class tReceiver tpllistener.h "networking/tpllistener.h"
\brief Base class to receiving threads

This class is a base class for the threads spawned by tplListener classes.
*/
class tReceiver : public Wefts::Thread
{
protected:
	Cabal::TCPSocket *sock;
public:
	tReceiver(Cabal::TCPSocket *aSock) : sock(aSock)
	{ }
	
	virtual ~tReceiver();
	
	virtual void *run() = 0;
};

#endif
