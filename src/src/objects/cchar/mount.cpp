/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief cChar's mount methods
*/

#include "objects/cchar.h"

/*!
\brief Mount a creature
\author Flameeyes (based on Endymion's one)
\param mount the creature
\note remove creature and give player a creature item
*/
void cChar::mountHorse( pNPC mount )
{
	if ( ! mount || ( !hasInRange(mount, 2) && ( !client || client->currAccount()->getPrivLevel() < cAccount::privCounselor ) ) )
		return;
		
	if ( events[eventCharOnMount] ) {
		tVariantVector params = tVariantVector(2);
		params[0] = getSerial(); params[1] = mount->getSerial();
		events[eventCharOnMount]->setParams(params);
		events[eventCharOnMount]->execute();
		if ( events[eventCharOnMount]->bypassed() )
			return;
	}

	if ( client->currAccount()->getPrivLevel() < cAccount::privCounselor && mount->getOwner() != this )
	{
		sysmsg("You dont own that creature.");
		return;
	}

	if ( body->isMounted() )
	{
		sysmsg("You are already on a mount.");
		return;
	}

	std::map<int32_t,int32_t>::iterator iter = mountinfo.find(mount->getId());
	if( iter==mountinfo.end() ) { //not mountable creature
		sysmsg("This is not a mountable creature");
		return;
	}

	pItem pi = cItem::createFromScript( "$item_mount", this );

	if ( ! pi )
		return;

	pi->setId( iter->second );
	pi->setColor( mount->getColor() );
	pi->setCurrentName( mount->getCurrentName() );

	/*pi->id1 = 0x3E;

	switch ( mount->id2)
	{
		case 0xC8:
			pi->id2 = (unsigned char)'\x9F';
			break; // Horse or Mare (based on color currently)
		case 0xE2:
			pi->id2 = (unsigned char)'\xA0';
			break; // Horse
		case 0xE4:
			pi->id2 = (unsigned char)'\xA1';
			break; // Horse
		case 0xCC:
			pi->id2 = (unsigned char)'\xA2';
			break; // Horse
		case 0xD2:
			pi->id2 = (unsigned char)'\xA3';
			break; // Desert Ostard
		case 0xDA:
			pi->id2 = (unsigned char)'\xA4';
			break; // Frenzied Ostard
		case 0xDB:
			pi->id2 = (unsigned char)'\xA5';
			break; // Forest Ostard
		case 0xDC:
			pi->id2 = (unsigned char)'\xA6';
			break; // llama
		// New mountable creatures
		case 0x7A:
			pi->id2 = (unsigned char)'\xB4'; // unicorn
			break;

		case 0xBB:
			pi->id2 = (unsigned char)'\xB8';
			break; // Ridgeback
		case 0x17:
			pi->id2 = (unsigned char)'\xBC'; // Giant Beetle
			break;
		case 0x19:
			pi->id2 = (unsigned char)'\xBB'; // Skeletal Mount
			break;
		case 0x1a:
			pi->id2 = (unsigned char)'\xBD'; // swamp dragon
			break;
		case 0x1f:
			pi->id2 = (unsigned char)'\xBE'; // armor dragon
			break;
		default :
			break; // llama

	}

	*//* XAN -- known horse valuez :
	9F -> A2 : horses
	A3->A5 : ostards
	A6 : llama
	A7 : nightmare
	A8 : dream
	A9 : nightmare again
	AA : ethereal horse
	AB : ethereal llama
	AC : ethereal ostard
	AD : bronze horse
	AE : invisible mount
	AF : dark bronze horse
	B0 : nightmare again 2
	B1 : light nightmare
	B2 : violet nightmare
	B3 : dark dream
	B4 : white horse
	B5 : darker nightmare
	B6 : dark nightmare
	B7 : black nightmare
	B8 : golden ostard

	***/

	pi->setContainer(body);
	pi->layer = layMount;
	body->setItemLayer(layMount, pi);

	// v-- is not cheched for decay, so useless
	if (mount->summontimer != 0) {
		pi->setDecayTime( mount->summontimer );
	} else { //Luxor bug fix
		pi->setDecay(false);
	}

	NxwSocketWrapper sw;
	sw.fillOnline( this, false );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		NXWSOCKET si=sw.getSocket();
		if( si!=INVALID )
			wornitems(si, this );
	}

	// if this is a gm lets tame the animal in the process
	if (IsGM())
	{
		mount->setOwnerSerial32( getSerial() );
		mount->tamed = true;
		mount->npcaitype = NPCAI_GOOD;
	}
	mount->mounted=true;
	pointers::delFromLocationMap( mount );
	pointers::pMounted.insert( make_pair( getSerial(), mount ) );

	sw.clear();
	sw.fillOnline( this, false );

	uint32_t mount_serial = mount->getSerial();

	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		NXWSOCKET si=sw.getSocket();
		if(si!=INVALID)
		{
			cPacketSendDeleteObj pk(mount);
			si->sendPacket(&pk);
		}
	}

	sysmsg( "Now you are riding %s", mount->getCurrentName().c_str());

	mount->war = 0;
	mount->attackerserial=INVALID;

	mount->time_unused = 0;
	mount->timeused_last = getClock();
}

/*!
\brief make char unmount horse (Remove horse item and spawn new horse)
\return true if success, else false
\todo Need rewrite
*/
bool cChar::unmountHorse()
{
	pFunctionHandle evt = src->getEvent(evtChrOnDismount);
	if ( evt )
	{
		tVariantVector params = tVariantVector(2);
		params[0] = getSerial(); params[1] = INVALID;
		evt->setParams(params);
		evt->execute();
		if ( evt->bypassed() )
			return;
	}
	
	pEquippable pe = getBody()->getLayerItem(layMount);
	if ( ! pe )
		return false;
	
	setOnHorse(false);

#if 0
	std::map< uint32_t, pChar >::iterator iter( pointers::pMounted.find( getSerial() ) );

	if( ( iter!=pointers::pMounted.end() ) ) {

		pChar p_pet=iter->second;
		pointers::pMounted.erase( iter );

		if( p_pet ) {


			p_pet->timeused_last = getclock();
			p_pet->time_unused = 0;

			p_pet->mounted=false;

			p_pet->MoveTo( getPosition() );
			p_pet->dir = dir; // Luxor
			p_pet->teleport();

			pi->Delete();
			teleport( TELEFLAG_SENDWORNITEMS );
			return true;
		}
	}
#endif
	pi->Delete();
	teleport( TELEFLAG_SENDWORNITEMS );
	InfoOut("Horse not found");
	return false;
}
