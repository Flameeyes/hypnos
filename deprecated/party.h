/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#ifndef __PARTY_H__
#define __PARTY_H__

#include "common_libs.h"
#include "oldmenu.h"
#include "target.h"

#include <list>

#define PARTY_BROADCAST_COLOR 0
#define PARTY_PRIVATE_COLOR 0

class cParty
{
	
	private:
		void sendToAllMember( P_SERVER_PACKET pkg );
	
	public:
		std::vector< P_PARTY_MEMBER >	members;	//!< all members
		std::vector< uint32_t > candidates;	//!< all candidates

		uint32_t serial;	//!< party serial

		cParty();
		~cParty();
			
		void addMember( pChar member );
		void removeMember( pChar member );
		P_PARTY_MEMBER getMember( uint32_t member );

		uint32_t getLeader();

		void addCandidate( pChar leader, pChar candidate );
		void removeCandidate( uint32_t serial );
		bool isCandidate( uint32_t serial );

		void privateMessage( uint32_t from, SERIAL to, std::wstring& s, COLOR color=PARTY_PRIVATE_COLOR );
		void talkToOthers( uint32_t from, std::wstring& s, COLOR color=PARTY_BROADCAST_COLOR );
		void talkToAll( std::wstring& s, COLOR color=PARTY_BROADCAST_COLOR );
			
};

class cPartyMember {
	
public:

	uint32_t serial;
	bool canLoot;

	cPartyMember( uint32_t member );

};

class cPartys {

	private:
		std::map< uint32_t, P_PARTY > partys;	//!< all partys
		uint32_t currentSerial;	//!< current serial
	public:
		bool shareKarma;
		bool shareFame;
		bool canPMsg;
		bool canBroadcast;

		P_PARTY	createParty( );
		P_PARTY	getParty( uint32_t serial );
		void removeParty( uint32_t serial );
		void receive( pClient ps );

		cPartys();
		~cPartys();

};

extern cPartys Partys;

#endif //__PARTY_H__
