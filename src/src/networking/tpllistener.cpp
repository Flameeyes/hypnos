/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "networking/tpllistener.h"
#include "networking/tkiller.h"
#include "settings.h"

template <class tHandler> tplListener<tHandler> *tplListener<tHandler>::instance = NULL;

/*!
\brief Constructor for a listening thread
\param localHost host where to open the listening socket
\param localPort port where to open the listening socket

\todo Throw an exception if there's already an instance.
*/
template <class tHandler> tplListener<tHandler>::tplListener(std::string localHost, uint16_t localPort) : Wefts::Thread()
{
	instance = this;
	
	// Create the socket
	sock = new Cabal::ServerSocket(localHost, localPort, 2000);
}

/*!
\brief Loop function for tplListener thread

This function does all the dirt work for tplListener thread, accepting the
connection until the socket is closed, and spawning new tHandler threads for
every new connection.
*/
template <class tHandler> void *tplListener<tHandler>::run()
{
	while(!sock->closed())
	{
		Cabal::TCPSocket *accepted = sock->accept();
		if ( accepted )
			new tHandler(accepted);
	}
	
	return NULL;
}

/*!
\brief Desctructor for tplListener thread

This function closes all the sockets in the tplListener::threads sets to be
deleted by tKiller, and then destroy itself.
*/
template <class tHandler> tplListener<tHandler>::~tplListener()
{
	for(std::set<tHandler*>::iterator it = threads.begin(): it != threads.end(); it++)
		(*it)->close();
}
