  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "common_libs.h"
#include "network.h"
#include "sndpkg.h"
#include "layer.h"
#include "npcai.h"
#include "scp_parser.h"
#include "set.h"


#include "inlines.h"
#include "utils.h"
#include "range.h"
#include "scripts.h"

std::map< int32_t, int32_t > mountinfo;

void loadmounts()
{

	cScpIterator* iter = NULL;
	std::string script1;
	std::string script2;
	int32_t id=INVALID;
	int32_t anim=INVALID;
	int mount=0;

	do
	{
		safedelete(iter);
		iter = Scripts::Mountable->getNewIterator("SECTION MOUNT %i", mount++);
		if( iter == 0 ) continue;

		id=INVALID;
		anim=INVALID;

		do
		{
			iter->parseLine(script1, script2);
			if	( script1 == "ANIM" )	anim = str2num(script2);
			else if ( script1 == "ID" )
			{
				id = str2num(script2);
				//ConOut("Mount %d", id );
			}
		}
		while ( script1[0] != '}' );

		if((id!=INVALID) && (anim!=INVALID))
			mountinfo[anim]=id;
	}
	while ( iter != 0 );

	safedelete(iter);

}

/*!
\brief Mount a creature
\author Flameeyes (based on Endymion's one)
\param mount the creature
\note remove creature and give player a creature item
*/
void cChar::mountHorse( pChar mount )
{
	if (
		!mount ||
		( !hasInRange(mount, 2) &&
		  ( !client || client->currAccount()->getPrivLevel() < cAccount::privCounselor )
		)
	   )
		return;

	PyObject *pArgs, *pValue;
	if (events[eventCharOnMount] && PyCallable_Check(events[eventCharOnMount]) )
	{
		pArgs = PyTurple_New(2);
		pValue = PyLong_FromVoidPtr(this);
		if ( ! pValue )
		{
			Py_DECREF(pArgs);
			LogError("Error adding parameters for character mount - ignoring");
			goto jump_mountevent;
		}
		PyTurple_SetItem(pArgs, 0, pValue)

		pValue = PyLong_FromVoidPtr(mount);
		if ( ! pValue )
		{
			Py_DECREF(pArgs);
			LogError("Error adding parameters for character mount - ignoring");
			goto jump_mountevent;
		}
		PyTurple_SetItem(pArgs, 1, pValue)

		pValue = PyObject_CallObject(events[eventCharOnMount], pArgs);
		Py_DECREF(pArgs);

		if ( ! pValue )
		{
			LogError("Call of event handler for character mount failed");
			goto jump_mountevent;
		}

		int res = PyInt_AsInt(pValue);
		Py_DECREF(pValue);

		if ( res == pyEat )
			return;
	}

jump_mountevent:

	if ( client->currAccount()->getPrivLevel() < cAccount::privCounselor && mount->getOwner() != this )
	{
		sysmsg(TRANSLATE("You dont own that creature."));
		return;
	}

	if ( body->isMounted() )
	{
		sysmsg( TRANSLATE("You are already on a mount."));
		return;
	}

	std::map<int32_t,int32_t>::iterator iter = mountinfo.find(mount->getId());
	if( iter==mountinfo.end() ) { //not mountable creature
		sysmsg( "This is not a mountable creature" );
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
#ifdef SPAR_C_LOCATION_MAP
	pointers::delFromLocationMap( mount );
#else
	mapRegions->remove( mount );
#endif
	pointers::pMounted.insert( make_pair( getSerial(), mount ) );

	sw.clear();
	sw.fillOnline( this, false );

	uint32_t mount_serial = mount->getSerial();

	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		NXWSOCKET si=sw.getSocket();
		if(si!=INVALID)
		{
			SendDeleteObjectPkt(si, mount_serial);
		}
	}

	sysmsg( "Now you are riding %s", mount->getCurrentName().c_str());

	mount->war = 0;
	mount->attackerserial=INVALID;

	mount->time_unused = 0;
	mount->timeused_last = uiCurrentTime;
}

/*!
\brief make char unmount horse (Remove horse item and spawn new horse)
\return 1 on error, 0 else
\remark only online players can unmount
*/
int cChar::unmountHorse()
{
	NXWCLIENT ps = getClient();
	if(ps==NULL)
		return 1;


	if(this->amxevents[EVENT_CHR_ONDISMOUNT]) // Unavowed
	{
		g_bByPass=false;
		this->amxevents[EVENT_CHR_ONDISMOUNT]->Call(this->getSerial(),INVALID);
		if(g_bByPass) return 1;
	}
	/*
	runAmxEvent( EVENT_CHR_ONDISMOUNT, getSerial() );
	if(g_bByPass)
		return 1;
	*/

	NxwItemWrapper si;
	si.fillItemWeared( this, true, true, false );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi=si.getItem();
		if( pi && pi->layer == LAYER_MOUNT)
		{

			onhorse = false;


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
					return 0;
				}
			}

			pi->Delete();
			teleport( TELEFLAG_SENDWORNITEMS );
			InfoOut("Horse not found");
			return 1;

		}
	}
	return INVALID;
}
