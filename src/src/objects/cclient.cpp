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

static cClients cClient::clients;               //!< this keeps all online clients

cClient::cClient(SI32 sd, struct sockaddr_in* addr)
{
	sock = new cSocket(sd, addr);
	pc = NULL;
	acc = NULL:
        clients.push_back(this);
}

cClient::~cClient()
{
	if ( pc )
		pc->setClient(NULL);
	if ( acc )
		acc->setClient(NULL);

	delete sock;
        clients.erase(find(clients.begin(), clients.end(), this));
}

/*!
\brief Compress packet buffer
\param out_buffer buffer to send
\param out_len size of buffer to send
*/
void cClient::compress(UI08& *out_buffer, UI32& out_len)
{
UI08 *new_buffer = new UI08[out_len];
UI32 new_len=0, tmp_len=out_len;

	if(out_len <= 0)
		return;

	for(i = 0; i < out_len; i++ ) {

		n_bits = bit_table[out_buffer[i]][0];
		value = bit_table[out_buffer[i]][1];

		while(n_bits--) {
//			new_buffer[new_len] = (new_buffer[new_len] << 1) | (UI08)((value >> n_bits) & 1);
			new_buffer[new_len] <<= 1;
			new_buffer[new_len] |= (UI08)((value >> n_bits) & 1);

			bit_4_byte++;
			if(bit_4_byte / 8) {
				new_len++;
				bit_4_byte %= 8;
			}
		}
	}

	n_bits = bit_table[256][0];
	value = bit_table[256][1];

	if(n_bits <= 0) {
		out_len = 0;
		return;
	}

	while(n_bits--) {
//		new_buffer[new_len] = (new_buffer[new_len] << 1) | (UI08)((value >> n_bits) & 1);
		new_buffer[new_len] <<= 1;
		new_buffer[new_len] |= (UI08)((value >> n_bits) & 1);

		bit_4_byte++;
		if(bit_4_byte / 8) {
			new_len++;
			bit_4_byte %= 8;
		}
	}

	if(bit_4_byte) {
		while(bit_4_byte < 8) {
			new_buffer[new_len] <<= 1;
			bit_4_byte++;
		}
		new_len++;
	}

	delete out_buffer;
	out_buffer = new_buffer;
	out_len = new_len;
}

/*!
\brief Show a container to player
\author Kheru - rewrote by Flameeyes
\param pCont the container
*/
void cClient::showContainer(pItem pCont)
{
	if ( ! pCont )
		return;

	NxwItemWrapper si;
	si.fillItemsInContainer( pCont, false, false );
	SI32 count=si.size();

	cPacketSendDrawContainer pk(pCont->getSerial(), pCont->getGump());
	sendPacket(&pk);

	cPacketSendContainerItem pk2;

	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi=si.getItem();
		if ( ! pi )
			continue;

		//fix location of items if they mess up. (needs tweaked for container types)
		//! \todo The position of the items should be bound to client-specific protocol
		if (pi->getPosition().x > 150) pi->setPosition("x", 150);
		if (pi->getPosition().y > 140) pi->setPosition("y", 140);

		pk2.addItem(pi);
	}

	sendPacket(&pk2);
}

/*!
\brief Show an item into a container
\author Flameeyes
\param item item to show
*/
void cClient::addItemToContainer(pItem item)
{
	if ( ! item || pc->distFrom(pi) > VISRANGE )
		return;

	cPacketSendAddContainerItem pk( item, item->getContainer()->getSerial() );

	sendPacket(&pk);

	weights::NewCalc(pc);
}

/*!
\brief Play a random midi file adapt to the current status
\author Flameeyes
\todo Write it after get working XML-Support
*/
void cClient::playMidi()
{
/*
    cScpIterator* iter = NULL;
    char script1[1024];
    char script2[1024];

	char sect[512];

	if (pc->war)
		strcpy(sect, "MIDILIST COMBAT");
	else
		sprintf(sect, "MIDILIST %i", region[pc->region].midilist);

	iter = Scripts::Regions->getNewIterator(sect);
	if (iter==NULL) return;

	char midiarray[50];
	int i=0;
	int loopexit=0;
	do
	{
		iter->parseLine(script1, script2);
		if ((script1[0]!='}')&&(script1[0]!='{'))
		{
			if (!(strcmp("MIDI",script1)))
			{
				midiarray[i]=str2num(script2);
				i++;
			}
		}
	}
	while ((script1[0]!='}') && (++loopexit < MAXLOOPS) );

	safedelete(iter);

	if (i!=0)
	{
		i=rand()%(i);
		playmidi(s, 0, midiarray[i]);
	}
*/
}

/*!
\brief Play a sound effect
\author Flameeyes
\todo Fix the set support after get working new sets
*/
void cClient::playSFX(UI16 sound, bool onlyMe)
{
	cPacketSendSoundFX pk(sound, pc->getPosition());

	if(onlyMe) {
		client->send(&pk);
		return;
	}

/*
	NxwSocketWrapper sw;
	sw.fillOnline( pc, false );

	for( sw.rewind(); !sw.isEmpty(); sw++ ) {
		NXWCLIENT ps=sw.getClient();
		if(ps!=NULL)
			ps->send(&pk);
	}
*/
}

/*!
\brief Set the light level
\param level base light level
*/
void cClient::light(UI08 level)
{
	if ( ! pc ) return;

	if (worldfixedlevel != 0xFF)
		level = worldfixedlevel;
	else if (pc->getFixedLight() != 0xFF)
		level = pc->getFixedLight();
	else if (pc->inDungeon())
		level = dungeonlightlevel;

	cPacketSendOverallLight pk(level);

	sendPacket(&pk);
}

void cClient::showBankBox(pPC dest)
{
	if ( ! dest || (dest != pc && acc->getPrivLevel() < privSeer ) )
		return;

	pItem bank = dest->getBankBox();
	if ( ! bank )
		return;

	showClient(bank);
}

/*!
\brief region specific bankbox
\author Endymion
\param pc character owner of bank

If activated, you can only put golds into normal banks
and there are special banks (for now we still use normal bankers,
but u have to say the SPECIALBANKTRIGGER word to open it)
where u can put all the items: one notice: the special bank
is caracteristic of regions....so in Britain you don't find
the items you leaved in Minoc!
All this for increasing pk-work and commerce! :)
(and surely the Mercenary work, so now have to pay strong
warriors to escort u during your travels!)
*/
void cClient::showSpecialBankBox(pPC dest)
{
	if ( ! dest || (dest != pc && acc->getPrivLevel() < privSeer ) )
		return;

	pItem bank = dest->getSpecialBankBox();
	if ( ! bank )
		return;

	showClient(bank);
}

void cClient::statusWindow(pChar sorg, bool extended, bool canrename)
{
	UI08 ext = 0x01;
	if ( flags & flagClientIsUO3D )
		ext = 0x03;
	if ( flags & flagClientIsAoS )
		ext = 0x04;

	cPacketSendStatus pk(sorg, ext, canrename);
	sendPacket(&pk);
}


