/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "networking/tlistening.h"
#include "networking/tkilling.h"
#include "networking/treceiving.h"
#include "settings.h"

tListening *tListening::instance = NULL;

/*!
\brief Constructor for tListening thread

This function creates the listening server loop, opens the server socket and
creates the tKilling thread.
\todo Throw an exception if there's already an instance.
*/
tListening::tListening() : Wefts::Thread()
{
	instance = this;
	
	// Create the socket
	new Cabal::ServerSocket(nSettings::Server::getLocalHostname(), nSettings::Server::getLocalPort(), 2000);
	
	new tKilling();
}

/*!
\brief Loop function for tListening thread

This function does all the dirt work for tListening thread, accepting the
connection until the socket is closed, and spawning new tReceiving threads for
every new connection.

\note This function also starts tKilling thread
*/
void *tListening::run()
{
	tKilling::instance->start();
	
	while(!sock->closed())
	{
		Cabal::TCPSocket *accepted = sock->accept();
		if ( accepted )
			new tReceiving(accepted);
	}
}

/*!
\brief Desctructor for tListening thread

This function closes all the sockets in the tListening::threads sets to be
deleted by tKilling, and then destroy itself.
*/
tListening::~tListening()
{
	for(std::set<tReceiving*>::iterator it = threads.begin(): it != threads.end(); it++)
		(*it)->close();
}
