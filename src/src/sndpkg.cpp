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
#include "basics.h"
#include "sndpkg.h"
#include "sregions.h"
#include "tmpeff.h"
#include "speech.h"
#include "inlines.h"
#include "basics.h"
#include "skills.h"
#include "utils.h"

void gmyell(char *txt)
{
	uint8_t unicodetext[512];
	int ucl = ( strlen ( txt ) * 2 ) + 2 ;

	char2wchar(txt);
	memcpy(unicodetext, Unicode::temp, ucl);

	uint32_t lang = calcserial(server_data.Unicodelanguage[0], server_data.Unicodelanguage[1], server_data.Unicodelanguage[2], 0);
	uint8_t sysname[30]={ 0x00, };
	strcpy((char *)sysname, "[WebAdmin - GM Only]");


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
			SendUnicodeSpeechMessagePkt(s, 0x01010101, 0x0101, 1, 0x0040, 0x0003, lang, sysname, unicodetext,  ucl);
		}
	}

	Network->ClearBuffers();
}


void SndDyevat(pClient client, int serial, short id)
{
	uint8_t dyevat[9] ={ 0x95, 0x00, };
	LongToCharPtr(serial, dyevat +1);
	ShortToCharPtr(0x0000, dyevat +5);	// ignored on send ....
	ShortToCharPtr(id, dyevat +7);		// def. on send 0x0FAB
	Xsend(s, dyevat, 9);
//AoS/	Network->FlushBuffer(s);
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


void itemmessage(pClient client, char *txt, int serial, short color)
{
// The message when an item is clicked (new interface, Duke)
//Modified by N6 to use UNICODE packets

	uint8_t unicodetext[512];
	uint16_t ucl = ( strlen ( txt ) * 2 ) + 2 ;

	pItem pi=cSerializable::findItemBySerial(serial);
	if ( ! pi ) return;

	if ((pi->type == ITYPE_CONTAINER && color == 0x0000)||
		(pi->type == ITYPE_SPELLBOOK && color == 0x0000)||
		(pi->getId()==0x1BF2 && color == 0x0000))
		color = 0x03B2;

	char2wchar(txt);
	memcpy(unicodetext, Unicode::temp, ucl);

	color = 0x0481; // UOLBR patch to prevent client crash by Juliunus

	uint32_t lang = calcserial(server_data.Unicodelanguage[0], server_data.Unicodelanguage[1], server_data.Unicodelanguage[2], 0);
	uint8_t sysname[30]={ 0x00, };
	strcpy((char *)sysname, "System");

	SendUnicodeSpeechMessagePkt(s, serial, 0x0101, 6, color, 0x0003, lang, sysname, unicodetext,  ucl);

}

void backpack2(pClient client, uint32_t serial) // Send corpse stuff
{
	int count=0, count2;
	uint8_t display1[7]={ 0x89, 0x00, };
	uint8_t display2[5]={ 0x00, };
	uint8_t bpopen2[5]={ 0x3C, 0x00, };

	pItem cont=cSerializable::findItemBySerial( serial );

	NxwItemWrapper si;
	si.fillItemsInContainer( cont, false );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi=si.getItem();
		if( pi && (pi->layer!=0) )
		{
			count++;
		}
	}
	count2=(count*5)+7 + 1 ; // 5 bytes per object, 7 for this header and 1 for terminator
	ShortToCharPtr(count2, display1+1);
	LongToCharPtr(serial, display1+3);
	Xsend(s, display1, 7);

	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi=si.getItem();
		if( pi && (pi->layer!=0) )
		{
			display2[0]= pi->layer;
			LongToCharPtr(pi->getSerial(), display2+1);
			Xsend(s, display2, 5);
		}
	}
	uint8_t nul = 0;
	Xsend(s, &nul, 1);	// Terminate with a 0
//AoS/	Network->FlushBuffer(s);

	ShortToCharPtr(count, bpopen2+3);
	count2=(count*19)+5;
	ShortToCharPtr(count2, bpopen2+1);
	Xsend(s, bpopen2, 5);

	uint8_t bpitem[20]={ 0x00, };

	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi=si.getItem();
		if( pi && (pi->layer!=0) )
		{
			LongToCharPtr(pi->getSerial(), bpitem);
			ShortToCharPtr(pi->animid(), bpitem +4);
			bpitem[6]=0x00;
			ShortToCharPtr(pi->amount, bpitem +7);
			ShortToCharPtr(pi->getPosition().x, bpitem +9);
			ShortToCharPtr(pi->getPosition().y, bpitem +11);
			LongToCharPtr(serial, bpitem +13);
			ShortToCharPtr(pi->getColor(), bpitem +17);
			Xsend(s, bpitem, 19);
		}
	}

//AoS/	Network->FlushBuffer(s);
}

void MakeGraphicalEffectPkt_(uint8_t pkt[28], uint8_t type, uint32_t src_serial, uint32_t dst_serial, uint16_t model_id, sLocation src_pos, sLocation dst_pos, uint8_t speed, uint8_t duration, uint8_t adjust, uint8_t explode )
{
	pkt[1]=type;
	LongToCharPtr(src_serial, pkt +2);
	LongToCharPtr(dst_serial, pkt +6);
	ShortToCharPtr(model_id, pkt +10);
	ShortToCharPtr(src_pos.x, pkt +12);
	ShortToCharPtr(src_pos.y, pkt +14);
	pkt[16]=src_pos.z;
	ShortToCharPtr(dst_pos.x, pkt +17);
	ShortToCharPtr(dst_pos.y, pkt +19);
	pkt[21]=dst_pos.z;
	pkt[22]=speed;
	pkt[23]=duration;
	ShortToCharPtr(0, pkt +24);		//[24] to [25] are not applicable here.
	pkt[26]=adjust; // LB possible client crashfix
	pkt[27]=explode;
}

void tileeffect(int x, int y, int z, char eff1, char eff2, char speed, char loop)
{//AntiChrist

	uint16_t eff = (eff1<<8)|(eff2%256);
	uint8_t effect[28]={ 0x70, 0x00, };

	sLocation pos1={ x, y, z, 0}, pos2={ 0, 0, 0, 0};
	
	MakeGraphicalEffectPkt_(effect, 0x02, 0, 0, eff, pos1, pos2, speed, loop, 1, 0);
	
	pos1.z=0;

	NxwSocketWrapper sw;
	sw.fillOnline( pos1 );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient sock=sw.getSocket();
		if( sock!=INVALID )
		{
			Xsend(sock, effect, 28);
//AoS/			Network->FlushBuffer(sock);
		}
	}

}

/*!
\author Xanathar
\brief Updates stats to nearbye players
\param stat stat which changed
*/
void cChar::updateStats(int32_t stat)
{
	checkSafeStats();

	int a = 0, b = 0;

	uint8_t updater[9]={ 0xA1, 0x00, };

	switch (stat)
	{
	case 0:
		a=getStrength();
		b=hp;
		break;
	case 1:
		a=in;
		b=mn;
		break;
	case 2:
		a=dx;
		b=stm;
		break;
	}

	updater[0]=0xA1+stat;
	LongToCharPtr(getSerial(), updater +1);
	ShortToCharPtr(a, updater +5);
	ShortToCharPtr(b, updater +7);

	if (stat == 0)  //Send to all, only if it's Health change
	{
		NxwSocketWrapper sw;
		sw.fillOnline( this, false );
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			pClient i=sw.getSocket();
			if( i!=INVALID )
			{
				Xsend(i, updater, 9);
//AoS/				Network->FlushBuffer(i);
			}
		}
	} else {
		pClient client = getSocket();
		if (s != INVALID)
		{
			Xsend(s, updater, 9);
//AoS/			Network->FlushBuffer(s);
		}
	}
}

void broadcast(int s) // GM Broadcast (Done if a GM yells something)
//Modified by N6 to use UNICODE packets
{
	//! \todo For my sake, can someone try to figure out how works this?! - Flame
	pChar pc=cSerializable::findCharBySerial(currchar[s]);
	if ( ! pc ) return;

	int i;
	char nonuni[512];

	if(pc->unicode)
		for (i=13;i<ShortFromCharPtr(buffer[s] +1);i=i+2)
		{
			nonuni[(i-13)/2]=buffer[s][i];
		}
	if(!(pc->unicode))
	{
		uint32_t id;
		uint16_t model,font, color;

		id = pc->getSerial();
		model = pc->getId();
		color = ShortFromCharPtr(buffer[s] +4);		// use color from client
		font = (buffer[s][6]<<8)|(pc->fonttype%256);	// use font ("not only") from  client

		uint8_t name[30]={ 0x00, };
		strcpy((char *)name, pc->getCurrentName().c_str());

		NxwSocketWrapper sw;
		sw.fillOnline();
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			pClient i=sw.getSocket();

			//!\todo redo adding to cpeech all the data and verifying
			nPackets::Sent::Speech pk(cSpeech(buffer+8));
			sw->sendPacket(&pk);

			//SendSpeechMessagePkt(i, id, model, 1, color, font, name, (char*)&buffer[s][8]);
		}
	} // end unicode IF
	else
	{
		uint32_t id;
		uint16_t model,font, color;
		uint8_t unicodetext[512];
		uint16_t ucl = ( strlen ( &nonuni[0] ) * 2 ) + 2 ;

		char2wchar(&nonuni[0]);
		memcpy(unicodetext, Unicode::temp, ucl);

		id = pc->getSerial();
		model = pc->getId();
		color = ShortFromCharPtr(buffer[s] +4);		// use color from client
		font = (buffer[s][6]<<8)|(pc->fonttype%256);	// use font ("not only") from  client

		uint32_t lang =  LongFromCharPtr(buffer[s] +9);
		uint8_t name[30]={ 0x00, };
		strcpy((char *)name, pc->getCurrentName().c_str());

		NxwSocketWrapper sw;
		sw.fillOnline();
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			pClient i=sw.getSocket();
			SendUnicodeSpeechMessagePkt(i, id, model, 1, color, font, lang, name, unicodetext,  ucl);
		}
	}
}

// Last touch: LB 8'th April 2001 for particleSystem

// if UO_3DonlyEffect is true, sta has to be valid and contain particleSystem data (if not, crash)
// for particleSystem data layout see staticeffectUO3d and updated packetDoku
// for old 2d staticeffect stuff , the new (3d client) pararamters UO_3Donlyeffect, sta and skip_old are defaulted in such a way that they behave like they did before
// simply dont set them in that case
// the last parameter is for particlesystem optimization only (dangerous). don't use unless you know 101% what you are doing.

void staticeffect(pChar player, uint16_t eff, unsigned char speed, unsigned char loop,  bool UO3DonlyEffekt, ParticleFx *sta, bool skip_old)
{
	if ( ! pc ) return;

	uint8_t effect[28]={ 0x70, 0x00, };

	 int a0,a1,a2,a3,a4;
	 sLocation charpos= pc->getPosition();

	 if (!skip_old)
	 {
		sLocation pos2;
		pos2.x = 0; pos2.y = 0; pos2.z = 0;
		MakeGraphicalEffectPkt_(effect, 0x03, pc->getSerial(), 0, eff, charpos, pos2, speed, loop, 1, 0);
	 }

	 if (!UO3DonlyEffekt) // no UO3D effect ? lets send old effect to all clients
	 {

		 NxwSocketWrapper sw;
		 sw.fillOnline( pc, false );
		 for( sw.rewind(); !sw.isEmpty(); sw++ )
		 {
			pClient client = sw.getSocket();
			Xsend(s, effect, 28);
//AoS/			Network->FlushBuffer(s);
		 }

	   return;
	}
	else
	{
		// UO3D effect -> let's check which client can see it
	   unsigned char particleSystem[49];


		NxwSocketWrapper sw;
	    sw.fillOnline( pc, false );
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			 pClient j =sw.getSocket();
			 if(j==INVALID) continue;
			 if (clientDimension[j]==2 && !skip_old) // 2D client, send old style'd
			 {
				 Xsend(j, effect, 28);
//AoS/				Network->FlushBuffer(j);
			 } else if (clientDimension[j]==3) // 3d client, send 3d-Particles
			 {
				staticeffectUO3D(player, sta);

				// allow to fire up to 4 layers at same time (like on OSI servers)
				a0 = sta->effect[10];

				a1 = ( ( a0 & 0x000000ff )       );
				a2 = ( ( a0 & 0x0000ff00 ) >> 8  );
				a3 = ( ( a0 & 0x00ff0000 ) >> 16 );
				a4 = ( ( a0 & 0xff000000 ) >> 24 );

				if (a1!=0xff) { particleSystem[46] = a1; Xsend(j, particleSystem, 49); }
				if (a2!=0xff) { particleSystem[46] = a2; Xsend(j, particleSystem, 49); }
				if (a3!=0xff) { particleSystem[46] = a3; Xsend(j, particleSystem, 49); }
				if (a4!=0xff) { particleSystem[46] = a4; Xsend(j, particleSystem, 49); }

//AoS/				Network->FlushBuffer(j);
			 }
			else if (clientDimension[j] != 2 && clientDimension[j] !=3 )
				LogError("Invalid Client Dimension: %i\n",clientDimension[j]);
	   } // end for
	} // end UO:3D effect

	// remark: if a UO:3D effect is send and ALL clients are UO:3D ones, the pre-calculation of the 2-d packet
	// is redundant. but we can never know, and probably it will take years till the 2d cliet dies.
	// I think it's too infrequnet to consider this as optimization.
}

// staticeffect2 is for effects on items
void staticeffect2(pItem pi, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode, bool UO3DonlyEffekt,  ParticleFx *str, bool skip_old )
{
	if ( ! pi ) return;

	uint16_t eff = (eff1<<8)|(eff2%256);
	uint8_t effect[28]={ 0x70, 0x00, };

	sLocation pos = pi->getPosition();

	if (!skip_old)
	{
		MakeGraphicalEffectPkt_(effect, 0x02, pi->getSerial(), pi->getSerial(), eff, pos, pos, speed, loop, 1, explode);
	}

	if (!UO3DonlyEffekt) // no UO3D effect ? lets send old effect to all clients
	{
		 NxwSocketWrapper sw;
		 sw.fillOnline( pi );
		 for( sw.rewind(); !sw.isEmpty(); sw++ )
		 {
			 pClient j =sw.getSocket();
			 if( j!=INVALID )
			 {
				Xsend(j, effect, 28);
//AoS				Network->FlushBuffer(j);
			 }
		}
		return;
	}
	else
	{
		// UO3D effect -> let's check which client can see it
		 NxwSocketWrapper sw;
		 sw.fillOnline( pi );
		 for( sw.rewind(); !sw.isEmpty(); sw++ )
		 {
			 pClient j =sw.getSocket();
			 if( j!=INVALID )
			 {
				if (clientDimension[j]==2 && !skip_old) // 2D client, send old style'd
				{
					Xsend(j, effect, 28);
//AoS/					Network->FlushBuffer(j);
				}
				else if (clientDimension[j]==3) // 3d client, send 3d-Particles
				{
					itemeffectUO3D(pi, str);
					unsigned char particleSystem[49];
					Xsend(j, particleSystem, 49);
//AoS/					Network->FlushBuffer(j);
				}
				else if (clientDimension[j] != 2 && clientDimension[j] !=3 )
					LogError("Invalid Client Dimension: %i\n", clientDimension[j]);
			}
		}
	}
}

//	- Movingeffect3 is used to send an object from a char
//    to another object (like purple potions)
void movingeffect3(SERIAL source, unsigned short x, unsigned short y, signed char z, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode)
{

	pChar src=cSerializable::findCharBySerial(source);
	if ( ! src ) return;

	uint16_t eff = (eff1<<8)|(eff2%256);
	uint8_t effect[28]={ 0x70, 0x00, };

	sLocation srcpos= src->getPosition(), pos2 = { x, y, z, 0};

	MakeGraphicalEffectPkt_(effect, 0x00, src->getSerial(), 0, eff, srcpos, pos2, speed, loop, 0, explode);

	 NxwSocketWrapper sw;
	 sw.fillOnline( src );
	 for( sw.rewind(); !sw.isEmpty(); sw++ )
	 {
		pClient j =sw.getSocket();
		if( j!=INVALID )
		{
			Xsend(j, effect, 28);
//AoS/			Network->FlushBuffer(j);
		}
	 }

}

/*!
\brief Item effects
\param pos sLocation where to send the effect
\param eff ID of the effect
\todo Replace with a nPackets::Sent:: class, maybe move it in a better place
*/
void staticeffect3(sLocation pos, uint16_t eff, uint8_t speed, uint8_t loop, uint8_t explode)
{
	uint8_t effect[28]={ 0x70, 0x00, };

	MakeGraphicalEffectPkt_(effect, 0x02, 0, 0, eff, pos, pos, speed, loop, 1, explode);
	pos.z = 0; //!< \todo verify this...

	NxwSocketWrapper sw;
	sw.fillOnline( pos );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient j =sw.getSocket();
		if( j!=INVALID )
		{
			Xsend(j, effect, 28);
//AoS/			Network->FlushBuffer(j);
		}
	}
}

void movingeffect3(SERIAL source, SERIAL dest, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode,unsigned char unk1,unsigned char unk2,unsigned char ajust,unsigned char type)
{
	pChar src=cSerializable::findCharBySerial(source);
	if (!src) return;

	pChar dst=cSerializable::findCharBySerial(dest);
	if (!dst) return;


	//0x0f 0x42 = arrow 0x1b 0xfe=bolt
	uint16_t eff = (eff1<<8)|(eff2%256);
	uint8_t effect[28]={ 0x70, 0x00, };

	sLocation srcpos= src->getPosition();
	sLocation destpos= dst->getPosition();

	MakeGraphicalEffectPkt_(effect, type, src->getSerial(), dst->getSerial(), eff, srcpos, destpos, speed, loop, ajust, explode);

	 NxwSocketWrapper sw;
	 sw.fillOnline( );
	 for( sw.rewind(); !sw.isEmpty(); sw++ )
	 {
		pClient j =sw.getSocket();
		if( j!=INVALID )
		{
			Xsend(j, effect, 28);
//AoS/			Network->FlushBuffer(j);
		}
	}
}

void SendDrawObjectPkt(pClient client, pChar pc, int z)
{
	pChar pc_currchar=cSerializable::findCharBySerial(currchar[s]);
	if(!pc_currchar) return;

	uint32_t k;
	uint8_t oc[1024]={ 0x78, 0x00, };

	sLocation charpos = pc->getPosition();

	LongToCharPtr(pc->getSerial(), oc +3);
	ShortToCharPtr(pc->getId(), oc +7); 	// Character art id
	ShortToCharPtr(charpos.x, oc+9);
	ShortToCharPtr(charpos.y, oc+11);
	if (z)
		oc[13]= charpos.dispz; 			// Character z position
	else
		oc[13]= charpos.z;
	oc[14]= pc->dir; 				// Character direction
	ShortToCharPtr(pc->getColor(), oc +15);	// Character skin color
	oc[17]=0; 					// Character flags
	if (pc->IsHidden() || !(pc->IsOnline()||pc->npc))
		oc[17]|=0x80; 				// .... show hidden state correctly
	if (pc->poisoned)
		oc[17]|=0x04; //AntiChrist -- thnx to SpaceDog

	k=19;
	int guild;
	guild=Guilds->Compare(pc_currchar,pc);
	if (guild==1)					//Same guild (Green)
		oc[18]=2;
	else if (guild==2) 				// Enemy guild.. set to orange
		oc[18]=5;
	else if (pc->IsGrey()) oc[18] = 3;
	else switch(pc->flag)
	{//1=blue 2=green 5=orange 6=Red 7=Transparent(Like skin 66 77a)
		case 0x01: oc[18]=6; break;// If a bad, show as red.
		case 0x04: oc[18]=1; break;// If a good, show as blue.
		case 0x08: oc[18]=2; break; //green (guilds)
		case 0x10: oc[18]=5; break;//orange (guilds)
		default: oc[18]=3; break;//grey (Can be pretty much any number.. I like 3 :-)
	}

	//!\todo Rewrite this with the new layer system
#if 0
// layers was a global variable, and global variables are evil. Please rewrite this code from scratch

	NxwItemWrapper si;
	si.fillItemWeared( pc, true, true, false );
	for( si.rewind(); !si.isEmpty(); si++ ) {

		pItem pj=si.getItem();
		if (pj)
			if ( layers[pj->layer] == 0 )
			{
				LongToCharPtr(pj->getSerial(), oc+k+0);
				ShortToCharPtr(pj->getId(), oc+k+4);
				oc[k+6]=pj->layer;
				k += 7;
				if (pj->getColor() != 0)
				{
					oc[k-3]|=0x80;
					ShortToCharPtr(pj->getColor(), oc+k);
					k+= 2;
				}
				layers[pj->layer] = 1;
			}
	}
#endif
	uint32_t ser = 0; 	// Not well understood. It's a serial number. I set this to my serial number,
			// and all of my messages went to my paperdoll gump instead of my character's
			// head, when I was a character with serial number 0 0 0 1.
	LongToCharPtr(ser, oc+k);
	k=k+4;
	// unimportant remark: its a packet "terminator" !!! LB

	ShortToCharPtr(k, oc +1);
	Xsend(s, oc, k);
//AoS/	Network->FlushBuffer(s);
}


void SendUnicodeSpeechMessagePkt(pClient client, uint32_t id, uint16_t model, uint8_t type, uint16_t color, uint16_t fonttype, uint32_t lang, uint8_t sysname[30], uint8_t *unicodetext, uint16_t unicodelen)
{
	uint16_t tl;
	uint8_t talk2[18]={ 0xAE, 0x00, };

	tl = 18 + 30 + unicodelen;

	ShortToCharPtr(tl, talk2 +1);
	LongToCharPtr(id, talk2 +3);
	ShortToCharPtr(model, talk2 +7);
	talk2[9]=type;
	ShortToCharPtr(color, talk2 +10);
	ShortToCharPtr(fonttype, talk2 +12);
	LongToCharPtr(lang, talk2 +14);

	Xsend(s, talk2, 18);
	Xsend(s, sysname, 30);
	Xsend(s, unicodetext, unicodelen);
//AoS/	Network->FlushBuffer(s);
}

void impowncreate(pClient client, pChar pc, int z) //socket, player to send
{
        if(!client)
		return;

	pChar pc_currchar = client->currChar();
	if(!pc_currchar) return;

	if (pc->isStabled() || pc->mounted)
		return; // dont **show** stabled pets

	bool sendit = true; //Luxor bug fix
	if (pc->IsHidden() && pc->getSerial()!=pc_currchar->getSerial() && !pc_currchar->IsGM())
		sendit=false;

	if( !pc->npc && !pc->IsOnline()  && !pc_currchar->IsGM() )
	{
		sendit=false;
		nPackets::Sent::DeleteObj pk(pc);
		client->sendPacket(&pk);
	}
	// hidden chars can only be seen "grey" by themselves or by gm's
	// other wise they are invisible=dont send the packet
	if (!sendit)
		return;

	SendDrawObjectPkt(s, pc, z);
	//pc_currchar->sysmsg( "sended %s", pc->getCurrentName().c_str() );
}

void sendshopinfo(int s, pChar pc, pItem pi)
{
	if ( ! pi ) return;

	char cFoundItems=0;
	int k, m1t, m2t, value,serial;

	uint8_t itemname[256]={ 0x00, };

	uint8_t m1[6096]={ 0x3C, 0x00, };	// Container content message

	uint8_t m2[6096]={ 0x74, 0x00, };	// Buy window details message

	LongToCharPtr(pi->getSerial(), m2+3); //Container serial number
	m2[7]=0; // Count of items;
	m1t=5;
	m2t=8;
	serial= pi->getSerial();

	NxwItemWrapper si;
	si.fillItemsInContainer( pi, false );
	int loopexit=0;
	for( si.rewind(); !si.isEmpty(); si++, ++loopexit )
	{
		pItem pj=si.getItem();
		if (pj)
			if ((m2[7]!=255) && (pj->amount!=0) ) // 255 items max per shop container
			{
				uint8_t namelen;
				if (m2t>6000 || m1t>6000) break;

				LongToCharPtr(pj->getSerial(), m1+m1t+0);//Item serial number
				ShortToCharPtr(pj->getId(), m1+m1t+4);
				m1[m1t+6]=0;			//Always zero
				ShortToCharPtr(pj->amount, m1+m1t+7); //Amount for sale
				ShortToCharPtr(loopexit, m1+m1t+9);
				ShortToCharPtr(loopexit, m1+m1t+11);
				LongToCharPtr(pi->getSerial(), m1+m1t+13); //Container serial number
				ShortToCharPtr(pj->getColor(), m1+m1t+17);
				m1[4]++; // Increase item count.
				m1t += 19;
				value = pj->value;
				value = pj->calcValue(value);
				
				if ( nSettings::Server::isEnabledTradeSystem() )
					value=calcGoodValue(pc, pj, value, 0); // by Magius(CHE)
				
				LongToCharPtr(value, m2+m2t+0);		// Item value/price
				namelen = pj->getName((char *)itemname);
				m2[m2t+4]=namelen; 			// Item name length

				for(k=0;k<namelen;k++)
				{
				  	m2[m2t+5+k]=itemname[k];
				}

				m2t += namelen +5;
				m2[7]++;
				cFoundItems=1; //we found items so send message
			}
	}

	ShortToCharPtr(m1t, m1 +1); // Size of message
	ShortToCharPtr(m2t, m2 +1); // Size of message

	if (cFoundItems==1)
	{
		Xsend(s, m1, m1t);
		Xsend(s, m2, m2t);
//AoS/		Network->FlushBuffer(s);
	}
}

bool sellstuff(pClient client, pChar pc)
{
	if (s < 0 || s >= now) return 0; //Luxor

	pChar pcs = cSerializable::findCharBySerial(currchar[s]);

	if(!pcs || !pc) return 0;

	char itemname[256];
	int m1t, z, value;
	int serial,serial1;
	char ciname[256]; // By Magius(CHE)
	char cinam2[256]; // By Magius(CHE)

	serial=pc->getSerial();
	/*for (ci=0;ci<pointers::pContMap[serial].size();ci++)
	{*/
	//<Luxor>

	pItem pp=pc->GetItemOnLayer(LAYER_TRADE_BOUGHT);
	if(!pp) return false;

	pItem pack= pcs->getBackpack();
	if(!pack) return false;

	// Pause the client only after the validity tests are completed
	// else we can have a deadlock on the client
	client->pause();

	uint8_t m1[2048]={ 0x9E, 0x00, };

	LongToCharPtr(pc->getSerial(), m1 +3);
	ShortToCharPtr(0, m1 +7);	// Num items  m1[7],m1[8]

	m1t=9;

	serial= pp->getSerial();
	serial1= pack->getSerial();

	NxwItemWrapper s_pack;
	s_pack.fillItemsInContainer( pack, false );

	NxwItemWrapper si;
	si.fillItemsInContainer( pp, false );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pj=si.getItem();
		if ( ! pj ) continue;
			
		for( s_pack.rewind(); !s_pack.isEmpty(); s_pack++ )
		{
			if (m1[8] >= 50) continue;

			pItem pj1 = s_pack.getItem();
			if ( ! pj1 ) continue;
			
			sprintf(ciname,"'%s'",pj1->getCurrentName().c_str()); // Added by Magius(CHE)
			sprintf(cinam2,"'%s'",pj->getCurrentName().c_str()); // Added by Magius(CHE)
			strupr(ciname); // Added by Magius(CHE)
			strupr(cinam2); // Added by Magius(CHE)

			if (pj1->getId()==pj->getId()  &&
				pj1->type==pj->type &&
				((SrvParms->sellbyname==0)||(SrvParms->sellbyname==1 && (!strcmp(ciname,cinam2))))) // If the names are the same! --- Magius(CHE)
			{
				uint8_t namelen;
				LongToCharPtr(pj1->getSerial(), m1+m1t+0);
				ShortToCharPtr(pj1->getId(),m1+m1t+4);
				ShortToCharPtr(pj1->getColor(),m1+m1t+6);
				ShortToCharPtr(pj1->amount,m1+m1t+8);
				value=pj->value;
				value = pj1->calcValue(value);
				if ( nSettings::Server::isEnabledTradeSystem() )
					value=calcGoodValue(pc, pj1,value,1); // by Magius(CHE)
				ShortToCharPtr(value, m1+m1t+10);
				namelen = pj1->getName(itemname);
				m1[m1t+12]=0;// Unknown... 2nd length byte for string?
				m1[m1t+13] = namelen;
				m1t += 14;
				for(z=0;z<namelen;z++)
				{
					m1[m1t+z]=itemname[z];
				}
				m1t += namelen;
				m1[8]++;
			}
		}
	}

	ShortToCharPtr(m1t, m1 +1);

	if (m1[8]<51) //With too many items, server crashes
	{
		if (m1[8]!=0)
		{
			Xsend(s, m1, m1t);
//AoS/			Network->FlushBuffer(s);
		}
		else
		{
			pc->talkAll("Thou dont posses nothing of interest to me.", false);
		}
	}
	else
	{
			pc->talkAll("Sorry i cannot take so many items.."), false);
	}

	client->resume();

	return true;
}

void tellmessage(int i, int s, char *txt)
//Modified by N6 to use UNICODE packets
{
	pChar pc=cSerializable::findCharBySerial(currchar[s]);
	if ( ! pc ) return;

	uint8_t unicodetext[512];
	char temp;

	asprintf(&temp, "GM tells %s: %s", pc->getCurrentName().c_str(), txt);

	uint16_t ucl = ( strlen ( temp ) * 2 ) + 2 ;
	char2wchar(temp);
	memcpy(unicodetext, Unicode::temp, ucl);

	uint32_t lang = calcserial(server_data.Unicodelanguage[0], server_data.Unicodelanguage[1], server_data.Unicodelanguage[2], 0);
	uint8_t sysname[30]={ 0x00, };
	strcpy((char *)sysname, "System");

	SendUnicodeSpeechMessagePkt(s, 0x01010101, 0x0101, 0, 0x0035, 0x0003, lang, sysname, unicodetext,  ucl);
	SendUnicodeSpeechMessagePkt(i, 0x01010101, 0x0101, 0, 0x0035, 0x0003, lang, sysname, unicodetext,  ucl); //So Person who said it can see too
	
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
