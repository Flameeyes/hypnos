/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cSocket class
*/

#include "csocket.h"

#include <sys/types.h>
#include <sys/socket.h>

cSocket::cSocket(SI32 sd, struct sockaddr_in *addr) :
	receiver(this), sender(this)
{
	s = sd;
	saddr = addr;
}

cSocket::~cSocket()
{
}

cSocket::clean()
{
	Network::cleanSocket(this);
}

void cSocket::cReceiver::run()
{
	UI08 buf[1024];
	size_t read;

	while( (read = recv(sock, buf, 1024, 0)) > 0 )
	{
		pPacketReceive pr = cPacketReceived::fromBuffer(buf, read);
		pr->execute(sock->getClient());
	}
}
