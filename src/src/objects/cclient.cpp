/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cClient class
*/

#include "cclient.h"

cClient::cClient(SI32 sd, struct sockaddr_in* addr)
{
	sock = new cSocket(sd, addr);
	pc = NULL;
	acc = NULL:
}

cClient::~cClient()
{
	if ( pc )
		pc->setClient(NULL);
	if ( acc )
		acc->setClient(NULL);

	delete sock;
}
