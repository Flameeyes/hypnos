/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "networking/sent.h"
#include "objects/cparty.h"

/*!
\brief Utility function for 0xBF subcommands
\param[out] buffer Buffer of the instance, will be allocated
\param[out] size Size of the instance, will be set
\param subcmd Code of the subcommand
\param subsize Size of the subcommand part
\return Pointer to the sixth byte of the buffer, so the subcommand part

This function creates the buffer, and sets the common bytes of the packet
*/
inline char* createBufferBF(char *&buffer, uint16_t &size, uint16_t subcmd, uint16_t subsize)
{
	size = 5 + subsize;
	buffer = new uint8_t[size];
	
	buffer[0] = 0xBF;
	ShortToCharPtr(size, buffer+1);
	ShortToCharPtr(subcmd, buffer+3);
	
	return buffer + 5;
}

void nPackets::Sent::PartyAddMember::prepare()
{
	assert(party);
	PCSList members = party->getMembersList();
	
	char *subpkg = createBufferBF(buffer, size, 0x06, 2 + members.size()*4);
	
	subpkg[0] = 0x01;
	subpkg[1] = members.size();
	
	char *currser = subpkg+2;
	for( PCSList::iterator it = members.begin(); it != members.end(); it++ )
	{
		LongToCharPtr((*it)->getSerial(), currser);
		currser += 4;
	}
}

void nPackets::Sent::PartyRemoveMember::prepare()
{
	assert(party);
	PCSList members = party->getMembersList();
	
	char *subpkg = createBufferBF(buffer, size, 0x06, 6 + members.size()*4);
	
	subpkg[0] = 0x02;
	subpkg[1] = members.size();
	LongToCharPtr(removed->getSerial(), subpkg+2);
	
	char *currser = subpkg+6;
	for( PCSList::iterator it = members.begin(); it != members.end(); it++ )
	{
		LongToCharPtr((*it)->getSerial(), currser);
		currser += 4;
	}
}

void nPackets::Sent::PartyPrivateMessage::prepare()
{
	char *subpkg = createBufferBF(buffer, size, 0x06, 7 + message.size()*2);

	subpkg[0] = 0x03;
	LongToCharPtr(target->getSerial(), subpkg+1);
	
	message.setPacketByteOrder();
	memcpy(subpkg + 5, message.rawBytes(), (message.size()+1)*2);
}

void nPackets::Sent::PartyBroadcast::prepare()
{
	char *subpkg = createBufferBF(buffer, size, 0x06, 7 + message.size()*2);
	
	subpkg[0] = 0x04;
	LongToCharPtr(sender->getSerial(), subpkg+1);
	
	message.setPacketByteOrder();
	memcpy(subpkg + 5, message.rawBytes(), (message.size()+1)*2);
}
