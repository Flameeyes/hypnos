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
\brief Login Server handling stuff

The methods and the structures declared in that unit are used in the handling
of packet 0xA8.

\todo Added reference to the packet class
*/

#ifndef __EXTRAS_LOGINSERVER_H__
#define __EXTRAS_LOGINSERVER_H__

#include "common_libs.h"

namespace nLoginServer {
	
	/*!
	\brief Single server element
	
	This struct is used to store data about the servers at which the
	clients can connect to after had their account authenticated.
	*/
	struct sServer {
		string title;	//!< Title of the server
		string hostname;	//!< Hostname of the server
		uint16_t port;		//!< TCP/IP port of the server
		
		uint32_t getIPAddress() const;
	};
	
	typedef vector<sServer> ServerVector;	//!< Vector of server (type)
	
	extern ServerVector servers;	//!< Servers which clients can login to
	
	void loadServers();
	
} // namespace

#endif
