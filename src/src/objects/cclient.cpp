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

/*!
\brief Show a container to player
\author GHisha - rewrote by Flameeyes
\param pCont the container
*/
void cClient::showContainer(pItem pCont)
{
	if ( ! pCont )
		return;

	NxwItemWrapper si;
	si.fillItemsInContainer( pCont, false, false );
	SI32 count=si.size();

	cPacketSendDrawContainer pk(pCont->getSerial(), pCont->getContGump());
	sendPacket(&pk);

	cPacketSendContainerItem pk2;

	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi=si.getItem();
		if ( ! pi )
			continue;

		//fix location of items if they mess up. (needs tweaked for container types)
		//! \todo The position of the items should be bound to client-specific protocol
		if (pi->getPosition("x") > 150) pi->setPosition("x", 150);
		if (pi->getPosition("y") > 140) pi->setPosition("y", 140);

		pk2.addItem(pi);
	}

	sendPacket(&pk2);
}

