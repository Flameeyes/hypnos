/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "logsystem.hpp"
#include "archs/tinterface.hpp"
#include "extras/loginserver.hpp"

#include "libhypnos/cvariant.hpp"

#include <mxml.h>
#include <wefts_mutex.h>
#include <cabal_address.h>

/*!
\brief Loads Login Servers List data from loginserver.xml configuration file.
*/
void nLoginServer::loadServers()
{
	servers.clear();
	outPlain("Loading login server data...\t\t");
	
	ifstream xmlfile("config/loginserver.xml");
	try {
		MXML::Document doc(xmlfile);
		
		MXML::Node *n = doc.main()->child();
		if ( ! n ) return;
		do {
			if ( n->name() != "server" )
			{
				LogWarning("Unknown element in loginserver.xml: %s", n->name().c_str());
				continue;
			}
			
			sServer srv;
			srv.title = n->getAttribute("title");
			srv.hostname = n->getAttribute("hostname");
			srv.port = cVariant(n->getAttribute("port")).toUInt16();
			
			servers.push_back(srv);
			
		} while( (n = n->next()) );
		outPlain("[   OK   ]\n");
	} catch ( MXML::MalformedError e) {
		outPlain("[ Failed ]\n");
		LogCritical("loginserver.xml file not well formed.");
	}
}

/*!
\brief Gets the IP address for the login server structure server.
\return The IP-address in long form of the server, in host byte order.

This function actually uses the Cabal::Address class to do the dirt work of
resolving hostnames and IPs.
*/
uint32_t nLoginServer::sServer::getIPAddress() const
{
	Cabal::Address serv(hostname);
	return serv.address().sin_addr.s_addr;
}
