  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "common_libs.h"
#include "debug.h"
#include "sndpkg.h"
#include "speech.h" //for unicode conversions
#include "party.h"
#include "menu.h"
#include "oldmenu.h"


#include "inlines.h"


class cPartys Partys;

cParty::cParty( )
{
	this->serial=INVALID;
}


cParty::~cParty() 
{
}



void cParty::addMember( pChar member )
{
	this->members.push_back( new cPartyMember( member->getSerial() ) );
	member->party=this->serial;
	csPacketAddPartyMembers pkg;
	pkg.members = &members;
	sendToAllMember( &pkg );
}


void cParty::removeMember( pChar member )
{
	std::vector<P_PARTY_MEMBER>::iterator iter( members.begin() ), end( members.end() );
	for( ; iter!=end; ++iter) {
		if( (*iter)->serial==member->getSerial() ) {
			members.erase( iter );
			csPacketRemovePartyMembers pkg;
			pkg.members = &members;
			pkg.member = member->getSerial();
			if( !members.empty() ) {
				sendToAllMember( &pkg );
			}
			member->party=INVALID;
			pkg.send( member->getClient() );
			if( members.empty() )
				Partys.removeParty( this->serial );
			return;
		}
	}

}

void cParty::sendToAllMember( P_SERVER_PACKET pkg )
{
	std::vector<P_PARTY_MEMBER>::iterator iter( members.begin() ), end( members.end() );
	for( ; iter!=end; ++iter) {
		pChar pc = pointers::findCharBySerial( (*iter)->serial );
		pkg->send( pc );
	}
}





uint32_t cParty::getLeader()
{
	return members.empty()? INVALID : members[0]->serial;
}


void cParty::addCandidate( pChar leader, pChar candidate )
{
	candidates.push_back( candidate->getSerial() );
	csPacketPartyInvite pkg;
	pkg.leader=leader->getSerial();
	pkg.send( candidate->getClient() );
}

void cParty::removeCandidate( uint32_t serial )
{
	std::vector<uint32_t>::iterator iter( candidates.begin() ), end( candidates.end() );
	for( ; iter!=end; ++iter ) {
		if( (*iter)==serial ) {
			candidates.erase( iter );
			return;
		}
	}
}

bool cParty::isCandidate( uint32_t serial )
{
	return find( candidates.begin(), candidates.end(), serial )!=candidates.end();
}

P_PARTY_MEMBER cParty::getMember( uint32_t member )
{
	std::vector<P_PARTY_MEMBER>::iterator iter( members.begin() ), end( members.end() );
	for ( ; iter!=end; ++iter )
		if ( (*iter)->serial==member )
			return (*iter);
	return NULL;
}

void cParty::talkToAll( std::wstring& s, COLOR color )
{
	std::string m;
	wstring2string( s, m );
	
	std::vector<P_PARTY_MEMBER>::iterator iter( members.begin() ), end( members.end() );
	for ( ; iter!=end; ++iter ) {
		pChar pc = pointers::findCharBySerial( (*iter)->serial );
		if( pc ) {
			NXWCLIENT ps = pc->getClient();
			if( ps!=NULL )
				ps->sysmsg( color, (char*)m.c_str() );
		}
	}
}

void cParty::privateMessage( uint32_t from, SERIAL to, std::wstring& s, COLOR color )
{
	csPacketPartyTellMessage pkg;
	pkg.member=from;
	pkg.message=&s;
	pChar pc = pointers::findCharBySerial( to );
	if ( ! pc ) return;
	pkg.send( pc->getClient() );	
}

void cParty::talkToOthers( uint32_t from, std::wstring& s, COLOR color )
{
	csPacketPartyTellAllMessage pkg;
	pkg.from=from;
	pkg.message=&s;
	
	std::vector<P_PARTY_MEMBER>::iterator iter( members.begin() ), end( members.end() );
	for ( ; iter!=end; ++iter ) {
		pChar pc = pointers::findCharBySerial( (*iter)->serial );
		if ( ! pc ) return;
			pkg.send( pc->getClient() );	
	}
			
}




cPartyMember::cPartyMember( uint32_t serial )
{
	this->serial=serial;
	this->canLoot=false;
}



cPartys::cPartys()
{
	currentSerial = 0; 
	shareKarma = true;
	shareFame = true;
	canPMsg = true;
	canBroadcast = true;
}

cPartys::~cPartys()
{
	std::map<uint32_t,P_PARTY>::iterator iter( partys.begin() ), end( partys.end() );
	for( ; iter!=end; ++iter )
		delete iter->second;
}

P_PARTY cPartys::createParty( ) 
{
	P_PARTY p = new cParty();
	p->serial=++currentSerial;
	partys.insert( make_pair( p->serial, p ) );
	return p;
}

P_PARTY cPartys::getParty( uint32_t serial )
{
	std::map< uint32_t, P_PARTY >::iterator iter( partys.find( serial ) );
	return ( iter!=partys.end() )? iter->second : NULL;
}

void cPartys::removeParty( uint32_t serial )
{
	std::map< uint32_t, P_PARTY >::iterator iter( partys.find( serial ) );
	if( iter!=partys.end() ) {
		delete iter->second;
		partys.erase( iter );
	}
}


void cPartys::receive( NXWCLIENT ps )
{

	static AmxFunction* addMem, *delMem, *lootMode, *accept, *decline = NULL;
	if( decline==NULL ) {
		addMem = new AmxFunction("party_onAddMember");
		delMem = new AmxFunction("party_onDelMember");
		lootMode = new AmxFunction("party_onCanLoot");
		accept = new AmxFunction("party_onAccept");
		decline = new AmxFunction("party_onDecline");
	}	
	
	
	cSubPacketParty<cClientPacket> pkg;
	pkg.receive( ps );

	switch( pkg.subsubcommand ) {
		case PARTY_SUBCMD_ADD: {
			clPacketAddPartyMember subpkg;
			subpkg.receive( ps );
			if( subpkg.member.get()==0 ) {
				addMem->Call( ps->currCharIdx() );
			}
			break;
		}
		case PARTY_SUBCMD_REMOVE: {
			clPacketRemovePartyMember subpkg;
			subpkg.receive( ps );
			uint32_t toRemove = subpkg.member.get();
			if( toRemove==0 ) {
				delMem->Call( ps->currCharIdx(), INVALID );
			}
			else {
				pChar pc = pointers::findCharBySerial( toRemove );
				if ( ! pc ) return;

				delMem->Call( ps->currCharIdx(), pc->getSerial() );
			}
			break;
		}
		case PARTY_SUBCMD_MESSAGE: {
			clPacketPartyTellMessage subpkg;
			subpkg.receive( ps );
			pChar pc = ps->currChar();
			P_PARTY party = getParty( pc->party );
			if( party!=NULL ) {
				party->privateMessage( ps->currCharIdx(), subpkg.member.get(), subpkg.message );
				if( ps->currCharIdx()!=subpkg.member.get() )
					party->privateMessage( ps->currCharIdx(), ps->currCharIdx(), subpkg.message );
			}
			break;
		}
		case PARTY_SUBCMD_BROADCAST: {
			clPacketPartyTellAllMessage subpkg;
			subpkg.receive( ps );
			pChar pc = ps->currChar();
			P_PARTY party = getParty( pc->party );
			if( party!=NULL )
				party->talkToOthers( ps->currCharIdx(), subpkg.message );
			break;
		}
		case PARTY_SUBCMD_CANLOOT: {
			clPacketPartyCanLoot subpkg;
			subpkg.receive( ps );
			lootMode->Call( ps->currCharIdx(), subpkg.canLoot );
			break;
		}
		case PARTY_SUBCMD_ACCEPT: {
			clPacketPartyAccept subpkg;
			subpkg.receive( ps );
			accept->Call( ps->currCharIdx(), subpkg.leader.get() );
			break;
		}
		case PARTY_SUBCMD_DECLINE: {
			clPacketPartyDecline subpkg;
			subpkg.receive( ps );
			decline->Call( ps->currCharIdx(), subpkg.leader.get() );
			break;
		}
	}

}


