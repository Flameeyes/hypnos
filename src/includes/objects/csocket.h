/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declaration of cSocket class
*/

#ifndef __CSOCKET_H__
#define __CSOCKET_H__

#include <zthread/Thread.h>
#include <zthread/BlockingQueue.h>

class cSocket;
typedef cSocket *pSocket;

#include "cclient.h"

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

	/*!
	\brief Class for receiving task

	Used by ZThread::Thread as Runnable object.
	*/
	class cReceiver : public Runnable
	{
	private:
		pSocket sock;
	public:
		inline cReceiver(pSocket s) :
			sock(s)
		{ }

		void run();
	};

	/*!
	\brief Class for sending task

	Used by ZThread::Thread as Runnable object.
	*/
	class cSender : public Runnable
	{
	private:
		pSocket sock;
	public:
		inline cSender(pSocket s) :
			sock(s)
		{ }

		void run();
	};

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
