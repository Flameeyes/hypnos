/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __OBJECTS_CSOCKET_H__
#define __OBJECTS_CSOCKET_H__

#include "objects/cclient.h"

//! Raw Packet
struct sRawPacket
{
	uint8_t *buffer;	//!< Pointer to the raw packet
	uint16_t length;	//!< Length of the packet
};

/*!
\author Flameeyes
\brief Socket class

This class integrate all functions to be used with socket, so read, write
and other.
It also contains the buffer read from the socket

\todo This should be reprojected
*/
class cSocket
{
protected:
	//! Actual socket
	int32_t s;
	//! Queue of the packets to be sent to the client
	ZThread::BlockingQueue<sRawPacket> sendQueue;
	//! List of received packets
	std::list<sRawPacket> receiveQueue;
	//! Socket address
	struct sockaddr_in *saddr;
	//! Client object
	pClient client;
	//! Receiving task
	cReceiver receiver;
	//! Sending task
	cSender sender;

public:
	cSocket(int32_t sd, struct sockaddr_in *addr);
	~cSocket();

	//! Gets the current character owned by the socket
	inline pChar currChar() const
	{ return pc; }

	//! Gets the socket descriptor
	inline const int32_t getSocket() const
	{ return s; }

	//! Gets the client
	inline const pClient getClient() const
	{ return client; }

	void clean();

};

#endif
