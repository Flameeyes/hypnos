/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "network.h"
#include "particles.h"
#include "sndpkg.h"
#include "sregions.h"
#include "tmpeff.h"
#include "speech.h"
#include "inlines.h"
#include "skills/skills.h"
#include "misc.h"

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
			cSpeech speech(std::string(txt));	//we must use string constructor or else it is supposed to be an unicode packet
			speech.setColor(0x40);
			speech.setFont(0x03);		// normal font
			speech.setMode(0x01);		// broadcast

			nPackets::Sent::UnicodeSpeech pk(speech);
			client->sendPacket(&pk);
		}
	}

	Network->ClearBuffers();
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


	cSpeech speech(std::string(txt));	//we must use string constructor or else it is supposed to be an unicode packet
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

	cSpeech speech(std::string(temp));	//we must use string constructor or else it is supposed to be an unicode packet
	speech.setColor(0x35);
	speech.setFont(0x03);		// normal font
	speech.setMode(0x00);		// normal speech

	nPackets::Sent::UnicodeSpeech pk(speech);
	s->sendPacket(&pk);
	i->sendPacket(&pk);
	free(temp);
}

// particleSystem core functions, LB 2-April 2001

// sta_str layout:

// 0..3 already used in 2d-staticeffect
// effect 4  -> tile1
// effect 5  -> tile2
// effect 6  -> speed1
// effect 7  -> speed1
// effect 8  -> effect1
// effect 9  -> effect2
// effect 10 -> reserved, dont use
// effect 11 ->
// effect 12 ->


void staticeffectUO3D(SERIAL player, ParticleFx *sta)
{

   pChar pc_cs=cSerializable::findCharBySerial(player);
   if(!pc_cs) return;

   sLocation charpos= pc_cs->getPosition();

   // please no optimization of p[...]=0's yet :)

   unsigned char particleSystem[49];
   particleSystem[0]= 0xc7;
   particleSystem[1]= 0x3;

   LongToCharPtr( pc_cs->getSerial(), particleSystem +2);

   LongToCharPtr( 0x00000000, particleSystem +6); // always 0 for this type

   particleSystem[10]= sta->effect[4]; // tileid1
   particleSystem[11]= sta->effect[5]; // tileid2

   particleSystem[12]= (charpos.x)>>8;
   particleSystem[13]= (charpos.x)%256;
   particleSystem[14]= (charpos.y)>>8;
   particleSystem[15]= (charpos.y)%256;
   particleSystem[16]= (charpos.z);

   particleSystem[17]= (charpos.x)>>8;
   particleSystem[18]= (charpos.x)%256;
   particleSystem[19]= (charpos.y)>>8;
   particleSystem[20]= (charpos.y)%256;
   particleSystem[21]= (charpos.z);

   particleSystem[22]= sta->effect[6]; // unkown1
   particleSystem[23]= sta->effect[7]; // unkown2

   particleSystem[24]=0x0; // only non zero for type 0
   particleSystem[25]=0x0;

   particleSystem[26]=0x1;
   particleSystem[27]=0x0;

   particleSystem[28]=0x0;
   particleSystem[29]=0x0;
   particleSystem[30]=0x0;
   particleSystem[31]=0x0;
   particleSystem[32]=0x0;
   particleSystem[33]=0x0;
   particleSystem[34]=0x0;
   particleSystem[35]=0x0;

   particleSystem[36]=sta->effect[8]; // effekt #
   particleSystem[37]=sta->effect[9];

   particleSystem[38]=sta->effect[11];
   particleSystem[39]=sta->effect[12];

   particleSystem[40]=0x00;
   particleSystem[41]=0x00;

   LongToCharPtr( pc_cs->getSerial(), particleSystem +42);

   particleSystem[46]=0; // layer, gets set afterwards for multi layering

   particleSystem[47]=0x0; // has to be always 0 for all types
   particleSystem[48]=0x0;


}

// ParticleFx layout:
// 0..4 already used in 2d-move_effect

// effect 5  -> tile1
// effect 6  -> tile2
// effect 7  -> speed1
// effect 8  -> speed2
// effect 9  -> effect1
// effect 10 -> effect2
// effect 11 -> impact effect1
// effect 12 -> impact effect2
// effect 13 -> unkown1, does nothing, but gets set on OSI shards
// effect 14 -> unkown2
// effect 15 -> adjust
// effect 16 -> explode on impact

void movingeffectUO3D(SERIAL source, SERIAL dest, ParticleFx *sta)
{


   pChar pc_cs=cSerializable::findCharBySerial(source);
   if(!pc_cs) return;

   pChar pc_cd=cSerializable::findCharBySerial(dest);
   if(!pc_cd) return;

   sLocation srcpos= pc_cs->getPosition();
   sLocation destpos= pc_cd->getPosition();

   unsigned char particleSystem[49];
   particleSystem[0]=0xc7;
   particleSystem[1]=0x0;

   LongToCharPtr( pc_cs->getSerial(), particleSystem +2);

   LongToCharPtr( pc_cd->getSerial(), particleSystem +6);

   particleSystem[10]=sta->effect[5]; // tileid1
   particleSystem[11]=sta->effect[6]; // tileid2

   particleSystem[12]= (srcpos.x)>>8;
   particleSystem[13]= (srcpos.x)%256;
   particleSystem[14]= (srcpos.y)>>8;
   particleSystem[15]= (srcpos.y)%256;
   particleSystem[16]= (srcpos.z);

   particleSystem[17]= (destpos.x)>>8;
   particleSystem[18]= (destpos.x)%256;
   particleSystem[19]= (destpos.y)>>8;
   particleSystem[20]= (destpos.y)%256;
   particleSystem[21]= (destpos.z);

   particleSystem[22]= sta->effect[7]; // speed1
   particleSystem[23]= sta->effect[8]; // speed2

   particleSystem[24]=0x0;
   particleSystem[25]=0x0;

   particleSystem[26]=sta->effect[15]; // adjust
   particleSystem[27]=sta->effect[16]; // explode

   particleSystem[28]=0x0;
   particleSystem[29]=0x0;
   particleSystem[30]=0x0;
   particleSystem[31]=0x0;
   particleSystem[32]=0x0;
   particleSystem[33]=0x0;
   particleSystem[34]=0x0;
   particleSystem[35]=0x0;

   particleSystem[36]=sta->effect[9]; //  moving effekt
   particleSystem[37]=sta->effect[10];
   particleSystem[38]=sta->effect[11]; // effect on explode
   particleSystem[39]=sta->effect[12];

   particleSystem[40]=sta->effect[13]; // ??
   particleSystem[41]=sta->effect[14];

   LongToCharPtr( 0x00000000, particleSystem +42);

   particleSystem[46]=0xff; // layer, has to be 0xff in that modus

   particleSystem[47]=sta->effect[17];
   particleSystem[48]=0x0;


}

// same sta-layout as staticeffectuo3d
void itemeffectUO3D(pItem pi, ParticleFx *sta)
{
	// please no optimization of p[...]=0's yet :)

	if ( ! pi ) return;

	unsigned char particleSystem[49];
	particleSystem[0]=0xc7;
	particleSystem[1]=0x2;

	if ( !sta->effect[11] )
		LongToCharPtr( pi->getSerial(), particleSystem +2);
	else
		LongToCharPtr( 0x00000000, particleSystem +2);

	LongToCharPtr( 0x00000000, particleSystem +6); // always 0 for this type

	particleSystem[10]=sta->effect[4]; // tileid1
	particleSystem[11]=sta->effect[5]; // tileid2

	ShortToCharPtr(particleSystem+12, pi->getPosition().x);
	ShortToCharPtr(particleSystem+14, pi->getPosition().y);
	particleSystem[16]= reinterpret_cast<uint8_t>(pi->getPosition().z);

	ShortToCharPtr(particleSystem+17, pi->getPosition().x);
	ShortToCharPtr(particleSystem+19, pi->getPosition().y);
	particleSystem[21]= reinterpret_cast<uint8_t>(pi->getPosition().z);

	particleSystem[22]= sta->effect[6]; // unkown1
	particleSystem[23]= sta->effect[7]; // unkown2

	particleSystem[24]=0x0; // only non zero for type 0
	particleSystem[25]=0x0;

	particleSystem[26]=0x1;
	particleSystem[27]=0x0;

	particleSystem[28]=0x0;
	particleSystem[29]=0x0;
	particleSystem[30]=0x0;
	particleSystem[31]=0x0;
	particleSystem[32]=0x0;
	particleSystem[33]=0x0;
	particleSystem[34]=0x0;
	particleSystem[35]=0x0;

	particleSystem[36]=sta->effect[8]; // effekt #
	particleSystem[37]=sta->effect[9];

	particleSystem[38]=0; // unknown
	particleSystem[39]=1;

	particleSystem[40]=0x00;
	particleSystem[41]=0x00;

	LongToCharPtr(pi->getSerial(), particleSystem +42);

	particleSystem[46]=0xff;

	particleSystem[47]=0x0;
	particleSystem[48]=0x0;

}

void wornitems(pClient client, pChar pc) // Send worn items of player
{
	if ( ! pc ) return;

	NxwItemWrapper si;
	si.fillItemWeared( pc, true, true, false );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi=si.getItem();
		if(pi)
			wearIt(s,pi);
	}
}
