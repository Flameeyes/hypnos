/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypnos/cvariant.h"
#include "extras/loginserver.h"
#include "logsystem.h"

#include <mxml.h>
#include <wefts_mutex.h>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*!
\brief Loads Login Servers List data from loginserver.xml configuration file.
*/
void nLoginServer::loadServers()
{
	servers.clear();
	ConOut("Loading login server data...\t\t");
	
	std::ifstream xmlfile("config/loginserver.xml");
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
		ConOut("[   OK   ]\n");
	} catch ( MXML::MalformedError e) {
		ConOut("[ Failed ]\n");
		LogCritical("loginserver.xml file not well formed.");
	}
}

/*!
\brief Gets the IP address for the login server structure server.
\return The IP-address in long form of the server, in network byte order.

Please note, before call this function in a sequence, plese call
\c sethostent(1) to use TCP connection instead of UDP when executing
DNS queries.
Then when completed, call \c endhostent() to close the TCP connection and
restore UDP queries.

\note This function is thread-safe, mutexed because \c gethostbyname() function
	may return pointer to static data which can be overwritten.
*/
uint32_t nLoginServer::sServer::getIPAddress() const
{
	// Mutex used to prevent gethostbyname() returned pointer to be
	// overwritten by new calls.
	static Wefts::Mutex m;
	m.lock();
	
	struct hostent *ret = gethostbyname(hostname.c_str());
	
	if ( ! ret )
		switch(h_errno)
		{
		case HOST_NOT_FOUND:
			LogWarning("Hostname %s not found. Returning invalid IP.", hostname.c_str());
			m.unlock();
			return 0;
		case NO_ADDRESS:
			LogWarning("Hostname %s is valid, but no IP address is defined for it. Returning invalid IP.", hostname.c_str());
			m.unlock();
			return 0;
		case NO_RECOVERY:
		case TRY_AGAIN:
		default:
			LogWarning("Error connecting to name server. Returning invalid IP.");
			m.unlock();
			return 0;
		}

	static struct in_addr addr;
	if ( ! inet_aton(ret->h_addr_list[0], &addr) )
	{
		LogWarning("Error resolving the address. Returning invalid IP.");
		m.unlock();
		return 0;
	}
	
	m.unlock();
	return addr.s_addr;
}
