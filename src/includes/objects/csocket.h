/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
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

#include "char.h"
#include <zthread/Thread.h>
#include <zthread/BlockingQueue.h>

//! Raw Packet
struct sRawPacket
{
	UI08 *buffer;	//!< Pointer to the raw packet
	UI16 length;	//!< Length of the packet
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
	SI32 s;
	//! Queue of the packets to be sent to the client
	ZThread::BlockingQueue<sRawPacket> sendQueue;
	//! List of received packets
	std::list<sRawPacket> receiveQueue;
	//! Socket address
	struct sockaddr_in *saddr;

	/*!
	\brief Class for receiving task

	Used by ZThread::Thread as Runnable object.
	*/
	class cReceiver : public Runnable
	{
	private:
		pSocket sock;
	public:
		inline cReceiver(pSocket s)
		{ sock = s; }

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
		inline cSender(pSocket s)
		{ sock = s; }

		void run();
	};

public:
	cSocket(SI32 sd, struct sockaddr_in *addr);
	~cSocket();

	//! Gets the current character owned by the socket
	inline pChar currChar() const
	{ return pc; }

	//! Gets the socket descriptor
	inline const SI32 getSocket() const
	{ return s; }

};

//! Pointer to a socket item
typedef cSocket *pSocket;

#endif
