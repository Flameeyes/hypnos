/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.hpp"
#include "particles.hpp"
#include "sndpkg.hpp"
#include "sregions.hpp"
#include "tmpeff.hpp"
#include "speech.hpp"
#include "inlines.hpp"
#include "misc.hpp"
#include "networking/network.hpp"
#include "objects/citem.hpp"
#include "skills/skills.hpp"

void gmyell(char *txt)
{

/*
	verify if this name should be used instead of "System"
	uint8_t sysname[30]={ 0x00, };
	strcpy((char *)sysname, "[WebAdmin - GM Only]");
*/


	NxwSocketWrapper sw;
	sw.fillOnline();
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient ps_i=sw.getClient();
		if(ps_i==NULL) continue;
		pChar pc=ps_i->currChar();
		pClient client = ps_i->toInt();
		if( pc && pc->IsGM())
		{
			cSpeech speech(string(txt));	//we must use string constructor or else it is supposed to be an unicode packet
			speech.setColor(0x40);
			speech.setFont(0x03);		// normal font
			speech.setMode(0x01);		// broadcast

			nPackets::Sent::UnicodeSpeech pk(speech);
			client->sendPacket(&pk);
		}
	}
}


/*!
\brief play a sound based on item id

added to do easy item sound effects based on an
items id1 and id2 fields in struct items. Then just define the CASE statement
with the proper sound function to play for a certain item as shown.

\author Dupois Duke
\date 09/10/1998 creation
	  25/03/2001 new interface by duke
\param item the item
\return soundfx for the item
\remarks \remark Use the DEFAULT case for ranges of items (like all ingots make the same thump).
		 \remark Sounds:
			\li coins dropping (all the same no matter what amount because all id's equal 0x0EED
			\li ingot dropping (makes a big thump - used the heavy gem sound)
			\li gems dropping (two type broke them in half to make some sound different then others)
*/
uint16_t itemsfx(uint16_t item)
{
	uint16_t sound = 0x0042;				// play default item move sfx // 00 48

	if( item == ITEMID_GOLD )
		sound = goldsfx(2);

	else if( (item>=0x0F0F) && (item<=0x0F20) )	// Any gem stone (typically smaller)
		sound = 0x0032;

	else if( (item>=0x0F21) && (item<=0x0F30) )	// Any gem stone (typically larger)
		sound = 0x0034;

	else if( (item>=0x1BE3) && (item<=0x1BFA) )	// Any Ingot
		sound = 0x0033;

	return sound;
}

/*!
\brief Plays background sounds of the game
\author LB & Duke
\param pc_curr Playing Character
*/
void bgsound(pChar pc_curr)
{
	if(!pc_curr) return;

	pChar inrange[15];
	int y=0;

	NxwCharWrapper sc;
	sc.fillCharsNearXYZ( pc_curr->getPosition(), VISRANGE+5, true, false );
	for( sc.rewind(); !sc.isEmpty(); sc++ ) {
		pChar pc=sc.getChar();
		if( pc->npc && !pc->dead && !pc->war )
		{
			inrange[y++]=pc;
			if( y==15 )
				return;
		}
	}

	if (y>0)
	{
		pChar pc_inr=inrange[ rand()%y ];
        if( chance(20) )
			pc_inr->playMonsterSound(SND_IDLE);
	}

	// play random mystic-sounds also if no creature is in range ...

	if(rand()%3333==33)
	{
		SOUND basesound=INVALID;
		switch(rand()%7)
		{
			case 0: basesound=595; break; // gnome sound
			case 1: basesound=287; break; // bigfoot 1
			case 2: basesound=288; break; // bigfoot 2
			case 3: basesound=639; break; // old snake sound
			case 4: basesound=179; break; // lion sound
			case 5: basesound=246; break; // mystic
			case 6: basesound=253; break; // mystic II
		}


		pc_curr->playSFX( basesound, true );
	}
}


void itemmessage(pClient client, char *txt, pItem pi, short color)
{
// The message when an item is clicked (new interface, Duke)
//Modified by N6 to use UNICODE packets

	if ( ! pi ) return;


	if ((pi->type == ITYPE_CONTAINER && color == 0x0000)||
		(pi->type == ITYPE_SPELLBOOK && color == 0x0000)||
		(pi->getId()==0x1BF2 && color == 0x0000))
		color = 0x03B2;

	color = 0x0481; // UOLBR patch to prevent client crash by Juliunus


	cSpeech speech(string(txt));	//we must use string constructor or else it is supposed to be an unicode packet
	speech.setColor(color);
	speech.setFont(0x03);		// normal font
	speech.setMode(0x06);		// label
	speech.setSpeaker(pi);

	nPackets::Sent::UnicodeSpeech pk(speech);
	client->sendPacket(&pk);
}



//! \todo this funcion is called apparently only by a function in the deprecated cmdtable.cpp. Delete or update it when that command is revised
void tellmessage(int i, int s, char *txt)
//Modified by N6 to use UNICODE packets
{
	pChar pc=cSerializable::findCharBySerial(currchar[s]);
	if ( ! pc ) return;

	char temp;

	asprintf(&temp, "GM tells %s: %s", pc->getCurrentName().c_str(), txt);

	cSpeech speech(string(temp));	//we must use string constructor or else it is supposed to be an unicode packet
	speech.setColor(0x35);
	speech.setFont(0x03);		// normal font
	speech.setMode(0x00);		// normal speech

	nPackets::Sent::UnicodeSpeech pk(speech);
	s->sendPacket(&pk);
	i->sendPacket(&pk);
	free(temp);
}
