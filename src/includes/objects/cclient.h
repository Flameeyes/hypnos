/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declaration of cClient class
*/

#ifndef __CCLIENT_H__
#define __CCLIENT_H__

class cClient;
typedef cClient *pClient;

#include "cchar.h"

/*!
\author Flameeyes
\brief Client access class

This class represent the client connection and is an high-end socket class.
Sending packages to be sent here, will call cSocket raw-level socket for send
them to the client itself.
Is this class which provides client compression & encryption.
*/
class cClient
{
protected:
	pChar pc;	//!< Current char used by the client
	pAccount acc;	//!< Current account logged in by the client
	pSocket sock;	//!< Current socket used by the client

public:
	cClient(SI32 sd, struct sockaddr_in* addr);
	~cClient();

	void sendPacket(pPacketSend ps);

	//! Gets the current char owned by the client
	inline pChar currChar() const
	{ return pc; }

	inline pAccount currAccount() const
	{ return acc; }
};

#endif
