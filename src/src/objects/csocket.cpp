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

cSocket::cSocket(int32_t sd, struct sockaddr_in *addr) :
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
	uint8_t buf[1024];
	size_t read;

	while( (read = recv(sock, buf, 1024, 0)) > 0 )
	{
		pPacketReceive pr = cPacketReceive::fromBuffer(buf, read);
		if (pr != NULL)
                {
                        if(!pr->execute(sock->getClient()))   //execute actually returns a bool, and if true the packet has been correctly handled
                        {
                                ;       //error message to console??
                        }
                        //! after execution (and probably ending the buffer usefulness) pr should be deleted or we'll run out of memory :D
                        //! but since PacketReceive is not yet fully done, there may be need (in future packets) of keeping it, even with the buffer no longer valid
                        delete pr;
                }
	}
}
