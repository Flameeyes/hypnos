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
#include "particles.h"
#include "debug.h"
#include "basics.h"
#include "sndpkg.h"
#include "sregions.h"
#include "itemid.h"
#include "srvparms.h"
#include "amx/amxcback.h"
#include "tmpeff.h"
#include "speech.h"

#include "layer.h"
#include "weight.h"
#include "accounts.h"
#include "scp_parser.h"


#include "inlines.h"
#include "basics.h"
#include "skills.h"
#include "range.h"
#include "classes.h"
#include "utils.h"
#include "scripts.h"

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
		NXWCLIENT ps_i=sw.getClient();
		if(ps_i==NULL) continue;
		pChar pc=ps_i->currChar();
		NXWSOCKET s = ps_i->toInt();
		if( pc && pc->IsGM())
		{
			SendUnicodeSpeechMessagePkt(s, 0x01010101, 0x0101, 1, 0x0040, 0x0003, lang, sysname, unicodetext,  ucl);
		}
	}

	Network->ClearBuffers();
}


//keep the target highlighted so that we know who we're attacking =)
//26/10/99//new packet
void SndAttackOK(NXWSOCKET  s, int serial)
{
	uint8_t attackok[5]={ 0xAA, 0x00, };
	LongToCharPtr(serial, attackok +1);
	Xsend(s, attackok, 5);
//AoS/	Network->FlushBuffer(s);
}

void SndDyevat(NXWSOCKET  s, int serial, short id)
{
	uint8_t dyevat[9] ={ 0x95, 0x00, };
	LongToCharPtr(serial, dyevat +1);
	ShortToCharPtr(0x0000, dyevat +5);	// ignored on send ....
	ShortToCharPtr(id, dyevat +7);		// def. on send 0x0FAB
	Xsend(s, dyevat, 9);
//AoS/	Network->FlushBuffer(s);
}

void SndUpdscroll(NXWSOCKET  s, short txtlen, const char* txt)
{
	uint8_t updscroll[10]={ 0xA6, };

	ShortToCharPtr(txtlen+10, updscroll +1);
	updscroll[3]=2;				// type: 0x00 tips window, 0x01 ignored, 0x02 updates
	LongToCharPtr(0 , updscroll +4);	// Tip numb.
	ShortToCharPtr(txtlen, updscroll +8);
	Xsend(s, updscroll, 10);
	Xsend(s, txt, txtlen);
//AoS/	Network->FlushBuffer(s);
}

void SndShopgumpopen(NXWSOCKET  s, uint32_t serial)	//it's really necessary ? It is used 1 time, perhaps replace it with the scriptable vers. :/
{
	uint8_t shopgumpopen[7]={ 0x24, 0x00, };
	LongToCharPtr(serial, shopgumpopen +1);		// ItemID
	ShortToCharPtr(0x0030, shopgumpopen +5);	// GumpID
	Xsend(s, shopgumpopen, 7);
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
\param s the character index
\todo convert to pChar or add to cChar class
*/
void bgsound(CHARACTER s)
{
    pChar pc_curr=MAKE_CHAR_REF(s);
	VALIDATEPC(pc_curr);

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

void soundeffect3(pItem pi, uint16_t sound)
{
	if ( ! pi ) return;

	Location pos = pi->getPosition();

	pos.z = 0;

	NxwSocketWrapper sw;
	sw.fillOnline( pi );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		NXWCLIENT ps_i=sw.getClient();
		if(ps_i==NULL) continue;
		pChar pc_j=ps_i->currChar();
		if( pc_j )
		{
			SendPlaySoundEffectPkt(ps_i->toInt(), 0x01, sound, 0x0000, pos);
		}
	}
}

void soundeffect4(NXWSOCKET s, pItem pi, uint16_t sound)
{
	if ( ! pi ) return;

	Location pos = pi->getPosition();

	pos.z = 0;

	SendPlaySoundEffectPkt(s, 0x01, sound, 0x0000, pos);
}

//xan : fast weather function.. maybe we should find a more complete system like the
//old one below!
void weather(NXWSOCKET  s, unsigned char bolt)
{
	uint8_t packet[4] = { 0x65, 0xFF, 0x40, 0x20 };

	if (wtype==0) packet[2] = 0x00;
	if (wtype==1) packet[1] = 0x00;
	if (wtype==2) { packet[1] = 0x02; packet[3] = 0xEC; }

	Xsend(s, packet, 4);
//AoS/	Network->FlushBuffer(s);
}

void pweather(NXWSOCKET  s)
{
	pChar pc=MAKE_CHAR_REF(currchar[s]);
	if ( ! pc ) return;

	uint8_t packet[4] = { 0x65, 0xFF, 0x40, 0x20 };

	if (region[pc->region].wtype==0) packet[2] = 0x00;
	if (region[pc->region].wtype==1) packet[1] = 0x00;
	if (region[pc->region].wtype==2) { packet[1] = 0x02; packet[3] = 0xEC; }

	Xsend(s, packet, 4);
//AoS/	Network->FlushBuffer(s);
}

void sysbroadcast(char *txt, ...) // System broadcast in bold text
//Modified by N6 to use UNICODE packets
{
	uint8_t unicodetext[512];

	va_list argptr;
	char msg[512];
	va_start( argptr, txt );
	vsprintf( msg, txt, argptr );
	va_end( argptr );

	int ucl = ( strlen ( msg ) * 2 ) + 2 ;

	char2wchar(msg);
	memcpy(unicodetext, Unicode::temp, ucl);

	uint32_t lang = calcserial(server_data.Unicodelanguage[0], server_data.Unicodelanguage[1], server_data.Unicodelanguage[2], 0);
	uint8_t sysname[30]={ 0x00, };
	strcpy((char *)sysname, "System");

	NxwSocketWrapper sw;
	sw.fillOnline();
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		NXWSOCKET sock=sw.getSocket();
		if( sock!=INVALID )
		{
			SendUnicodeSpeechMessagePkt(sock, 0x01010101, 0x0101, 6, 0x084D /*0x0040*/, 0x0000, lang, sysname, unicodetext,  ucl);
		}
	}
}


void sysmessage(NXWSOCKET  s, const char *txt, ...) // System message (In lower left corner)
{
	if(s < 0)
		return;

	uint8_t unicodetext[512];

	va_list argptr;
	char msg[512];
	va_start( argptr, txt );
    vsprintf( msg, txt, argptr );
	va_end( argptr );

	uint32_t spyTo = clientInfo[s]->spyTo;
	if( spyTo!=INVALID ) { //spy client
		pChar pc=pointers::findCharBySerial( spyTo );
		if( pc ) {
			NXWCLIENT gm = pc->getClient();
			if( gm!=NULL )
				gm->sysmsg( "spy %s : %s", pc->getCurrentNameC(), msg );
			else
				clientInfo[s]->spyTo=INVALID;
		}
		else
			clientInfo[s]->spyTo=INVALID;
	}

	int ucl = ( strlen ( msg ) * 2 ) + 2 ;

	char2wchar(msg);
	memcpy(unicodetext, Unicode::temp, ucl);

	uint32_t lang = calcserial(server_data.Unicodelanguage[0], server_data.Unicodelanguage[1], server_data.Unicodelanguage[2], 0);
	uint8_t sysname[30]={ 0x00, };
	strcpy((char *)sysname, "System");

	SendUnicodeSpeechMessagePkt(s, 0x01010101, 0x0101, 6, 0x0387 /* Color - Previous default was 0x0040 - 0x03E9*/, 0x0003, lang, sysname, unicodetext,  ucl);

}


void sysmessage(NXWSOCKET  s, short color, const char *txt, ...) // System message (In lower left corner)
{
	if( s < 0)
		return;

	uint8_t unicodetext[512];

	va_list argptr;
	char msg[512];
	va_start( argptr, txt );
	//vsnprintf( msg, sizeof(msg)-1, txt, argptr );
        vsprintf( msg, txt, argptr );
	va_end( argptr );
	uint16_t ucl = ( strlen ( msg ) * 2 ) + 2 ;

	char2wchar(msg);
	memcpy(unicodetext, Unicode::temp, ucl);

	uint32_t lang = calcserial(server_data.Unicodelanguage[0], server_data.Unicodelanguage[1], server_data.Unicodelanguage[2], 0);
	uint8_t sysname[30]={ 0x00, };
	strcpy((char *)sysname, "System");

	SendUnicodeSpeechMessagePkt(s, 0x01010101, 0x0101, 0, color, 0x0003, lang, sysname, unicodetext,  ucl);

}

void itemmessage(NXWSOCKET  s, char *txt, int serial, short color)
{
// The message when an item is clicked (new interface, Duke)
//Modified by N6 to use UNICODE packets

	uint8_t unicodetext[512];
	uint16_t ucl = ( strlen ( txt ) * 2 ) + 2 ;

	pItem pi=pointers::findItemBySerial(serial);
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

void backpack2(NXWSOCKET s, uint32_t serial) // Send corpse stuff
{
	int count=0, count2;
	uint8_t display1[7]={ 0x89, 0x00, };
	uint8_t display2[5]={ 0x00, };
	uint8_t bpopen2[5]={ 0x3C, 0x00, };

	pItem cont=pointers::findItemBySerial( serial );

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

void MakeGraphicalEffectPkt_(uint8_t pkt[28], uint8_t type, uint32_t src_serial, uint32_t dst_serial, uint16_t model_id, Location src_pos, Location dst_pos, uint8_t speed, uint8_t duration, uint8_t adjust, uint8_t explode )
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

Location pos1={ x, y, z, 0}, pos2={ 0, 0, 0, 0};

MakeGraphicalEffectPkt_(effect, 0x02, 0, 0, eff, pos1, pos2, speed, loop, 1, 0);

pos1.z=0;

	NxwSocketWrapper sw;
	sw.fillOnline( pos1 );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		NXWSOCKET sock=sw.getSocket();
		if( sock!=INVALID )
		{
			Xsend(sock, effect, 28);
//AoS/			Network->FlushBuffer(sock);
		}
	}

}

void senditem(NXWSOCKET  s, pItem pi) // Send items (on ground)
{
	if ( ! pi ) return;

	pChar pc=MAKE_CHAR_REF(currchar[s]);
	if ( ! pc ) return;

	bool pack;
	uint16_t len;
	uint8_t itmput[20]={ 0x1A, 0x00, };

	if ( pi->visible>=1 && !(pc->IsGM()) )
	return;

	// meaning of the item's attribute visible
	// Visible 0 -> visible to everyone
	// Visible 1 -> only visible to owner and gm's (for owners normal for gm's grayish/hidden color)
	// visible 2 -> only visble to gm's (greyish/hidden color)

	if (!pi->isInWorld())
	{
		pack=true;
		if (isCharSerial(pi->getContSerial()))
		{
			pChar pj=pointers::findCharBySerial(pi->getContSerial());
			if (pj)
				pack=false;
		}
		if (pack)
		{
			if (pi->getId()<0x4000) 			// LB client crashfix, dont show multis in BP
								// we should better move it out of pack, but thats
								// only a first bannaid
			{
				sendbpitem(s,pi);
				return;
			}
		}
	}
	else
	if( pc->hasInRange(pi) )
	{
		Location pos = pi->getPosition();

		LongToCharPtr(pi->getSerial() | 0x80000000, itmput +3);

		//if player is a gm, this item
		//is shown like a candle (so that he can move it),
		//....if not, the item is a normal
		//invisible light source!
		if(pc->IsGM() && pi->getId()==0x1647)
		{///let's show the lightsource like a candle
			ShortToCharPtr(0x0A0F, itmput +7);
		} else
		{//else like a normal item
			ShortToCharPtr(pi->animid(), itmput +7); // elcabesa animation tryyy
		}

		ShortToCharPtr(pi->amount, itmput +9);
		ShortToCharPtr(pos.x, itmput +11);
		ShortToCharPtr(pos.y | 0xC000, itmput +13);
		itmput[15]= pos.z;

		if(pc->IsGM() && pi->getId()==0x1647)
		{///let's show the lightsource like a blue item
			ShortToCharPtr(0x00C6, itmput +16);
		} else
		{
			ShortToCharPtr(pi->getColor(), itmput +16);
		}

		itmput[18]=0;

		bool dontsendcandidate=0;
		if (pi->visible==1)
		{
			if (pc->getSerial()!=pi->getOwnerSerial32())
			{
				dontsendcandidate=1;
				itmput[18]|=0x80;
			}
		}

		if (dontsendcandidate && !pc->IsGM())
			return; // LB 9-12-99, client 1.26.2 visibility correction

		if (pi->visible==2)
		{
			itmput[18]|=0x80;
		}


		if (pi->magic==1)
			itmput[18]|=0x20;
		if (pc->canAllMove())
			itmput[18]|=0x20;
		if ((pi->magic==3 || pi->magic==4) && pc->getSerial()==pi->getOwnerSerial32())
			itmput[18]|=0x20;

		if (pc->canViewHouseIcon())
		{
			if (pi->getId()>=0x4000 && pi->getId()<=0x40FF) // LB, 25-dec-1999 litle bugfix for treasure multis, ( == changed to >=)
			{
				ShortToCharPtr(0x14F0, itmput +7);
			}
		}

		len = 19;
		if (pi->dir)
		{
			itmput[19]=itmput[18];
			itmput[18]=itmput[17];
			itmput[17]=itmput[16];
			itmput[16]=itmput[15];
			itmput[15]=static_cast<unsigned char>(pi->dir);
			itmput[11]|=0x80;
			len = 20;
		}

		ShortToCharPtr(len, itmput +1);
		Xsend(s, itmput, len);
//AoS/		Network->FlushBuffer(s);
		//pc->sysmsg( "sent item %s %i", pi->getCurrentNameC(), pi->magic );

		if (pi->IsCorpse())
		{
			backpack2(s, pi->getSerial());
		}
	}

}

// sends item in differnt color and position than it actually is
// used for LSd potions now, LB 5'th nov 1999
void senditem_lsd(NXWSOCKET  s, ITEM i,char color1, char color2, int x, int y, signed char z)
{
	const pItem pi=MAKE_ITEM_REF(i);
	if ( ! pi ) return;

	pChar pc=MAKE_CHAR_REF(currchar[s]);
	if ( ! pc ) return;

	uint16_t color = (color1<<8)|(color2%256);
	uint16_t len;
	uint8_t itmput[20]={ 0x1A, 0x00, };

	if ( pi->visible>=1 && !(pc->IsGM()) ) return; // workaround for missing gm-check client side for visibity since client 1.26.2
	// for lsd we dont need extra work for type 1 as in send_item

	if (pi->isInWorld())
	{
		Location pos = pi->getPosition();

		LongToCharPtr(pi->getSerial() | 0x80000000, itmput +3);
		ShortToCharPtr(pi->getId(), itmput +7);
		ShortToCharPtr(pi->amount, itmput +9);
		ShortToCharPtr(pos.x, itmput +11);
		ShortToCharPtr(pos.y | 0xC000, itmput +13);
		itmput[15]=z;
		ShortToCharPtr(color, itmput +16);
		itmput[18]=0;

		if (pi->visible==1)
		{
			if (pc->getSerial()!=pi->getOwnerSerial32())
			{
				itmput[18]|=0x80;
			}
		}
		if (pi->visible==2)
		{
			itmput[18]|=0x80;
		}

		if (pi->visible==3)
		{
			if ((pc->getId() == BODY_GMSTAFF) || !pc->IsGM())
				itmput[18]|=0x80;
		}

		if (pi->magic==1) itmput[18]|=0x20;

		if (pc->canAllMove()) itmput[18]|=0x20;

		if ((pi->magic==3 || pi->magic==4) && pc->getSerial()==pi->getOwnerSerial32())
			itmput[18]|=0x20;

		if (pc->canViewHouseIcon())
		{
			if (pi->getId()>=0x4000 && pi->getId()<=0x40FF)
			{
				ShortToCharPtr(0x14F0, itmput +7);
			}
		}

		len = 19;

		if (pi->dir)
		{
			itmput[19]=itmput[18];
			itmput[18]=itmput[17];
			itmput[17]=itmput[16];
			itmput[16]=itmput[15];
			itmput[15]=static_cast<unsigned char>(pi->dir);
			itmput[11]|=0x80;
			len = 20;
		}

		ShortToCharPtr(len, itmput +1);
		Xsend(s, itmput, len);
//AoS/		Network->FlushBuffer(s);

		if (pi->IsCorpse() )
		{
			backpack2(s, pi->getSerial());
		}
	}
}

void chardel (NXWSOCKET  s) // Deletion of character
{
/* PkG 0x85,
 *      0x00 => That character password is invalid.
 *      0x01 => That character doesn't exist.
 *      0x02 => That character is being played right now.
 *      0x03 => That charater is not old enough to delete.
                The character must be 7days old before it can be deleted.
 *      0x04 => That character is currently queued for backup and cannot be
 *              deleted.
 *      0x05 => Couldn't carry out your request.
 */

	int i;
	uint8_t delete_error_msg[2] = {0x85, 0x05};
	uint8_t delete_resend_char_1[6]={0x86, 0x01, 0x30, 0x00}; // 1 + 2 + 1 + 5*60 = 304 = 0x0130
	uint8_t delete_resend_char_2[61];

	pChar TrashMeUp = NULL;
	NxwCharWrapper sc;

	Accounts->GetAllChars( acctno[s], sc );

	for ( i=0, sc.rewind(); !sc.isEmpty(); sc++)
	{
		pChar pc_a=sc.getChar();
		if(! pc_a )
			continue;

		if(i == buffer[s][0x22])
			TrashMeUp = pc_a;

		i++;
	}

	if (ServerScp::g_nPlayersCanDeleteRoles != 0) {
	/// Do Character Deletion ... and return if all ok
		if(!TrashMeUp) {
			delete_error_msg[1] = 0x01;
			Xsend(s, delete_error_msg, 2);
//AoS/			Network->FlushBuffer(s);
			return;
		}

		if ( TrashMeUp )
		{
			if( SrvParms->checkcharage &&
			   (getclockday() < TrashMeUp->GetCreationDay() + 7) ) {
				delete_error_msg[1] = 0x03;
				Xsend(s, delete_error_msg, 2);
//AoS/				Network->FlushBuffer(s);
				return;
			}

			if(TrashMeUp->IsOnline()) {
				delete_error_msg[1] = 0x02;
				Xsend(s, delete_error_msg, 2);
//AoS/				Network->FlushBuffer(s);
				return;
			}

			TrashMeUp->Delete();

			Accounts->GetAllChars( acctno[s], sc );

			delete_resend_char_1[3] = sc.size();

			Xsend(s, delete_resend_char_1, 4);

			for ( i=0, sc.rewind(); !sc.isEmpty(); sc++) {
				pChar pc_a=sc.getChar();
				if(! pc_a )
					continue;

				strcpy((char *)delete_resend_char_2, pc_a->getCurrentNameC());
				Xsend(s, delete_resend_char_2, 60);

				i++;
			}

			memset(delete_resend_char_2, 0, 60);

			for (;i<5;i++) {
				Xsend(s, delete_resend_char_2, 60);
			}
//AoS/			Network->FlushBuffer(s);

			return; // All done ;]
		}
	}
// Any possible error ....
	Xsend(s, delete_error_msg, 2);
//AoS/	Network->FlushBuffer(s);
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
			NXWSOCKET i=sw.getSocket();
			if( i!=INVALID )
			{
				Xsend(i, updater, 9);
//AoS/				Network->FlushBuffer(i);
			}
		}
	} else {
		NXWSOCKET s = getSocket();
		if (s != INVALID)
		{
			Xsend(s, updater, 9);
//AoS/			Network->FlushBuffer(s);
		}
	}
}

void updates(NXWSOCKET  s) // Update Window
{
	int x, y, j;
	char temp[512];
    cScpIterator* iter = NULL;
    char script1[1024];

    iter = Scripts::Misc->getNewIterator("SECTION MOTD");
    if (iter==NULL) return;
	strcpy(script1, iter->getEntry()->getFullLine().c_str()); //discard the {

	x=-1;
	y=-2;
	int loopexit=0;
	do
	{
		strcpy(script1, iter->getEntry()->getFullLine().c_str());
		x++;
		y+=strlen(script1)+1;
	}
	while ( (strcmp(script1, "}")) && (++loopexit < MAXLOOPS) );
	y+=10;
	iter->rewind();
	strcpy(script1, iter->getEntry()->getFullLine().c_str());

	uint8_t updscroll[10]={ 0xA6, 0x00, };
	ShortToCharPtr(y, updscroll +1); 		// len of pkt.
	updscroll[3]=2; 				// MOTD ? Type: 0x00 tips, 0x02 updates
	LongToCharPtr(0, updscroll +4);			// tip num.
	ShortToCharPtr(y-10, updscroll +8);		// len of only mess.
	Xsend(s, updscroll, 10); 		// Send 1st part (header)

	for (j=0;j<x;j++)
	{
		strcpy(script1, iter->getEntry()->getFullLine().c_str());
		sprintf(temp, "%s ", script1);
		Xsend(s, temp, strlen(temp)); 	// Send the rest
	}
	safedelete(iter);

//AoS/	Network->FlushBuffer(s);
}

void tips(NXWSOCKET s, uint16_t i, uint8_t want_next) // Tip of the day window
{
	int x, y, j;
	char temp[512];

	cScpIterator* iter = NULL;
	char script1[1024];
	char script2[1024];

	if(want_next) i = i+1;
	else i = i-1;

	if (i==0) i=1;

	iter = Scripts::Misc->getNewIterator("SECTION TIPS");
	if (iter==NULL) return;

	x=i;
	int loopexit=0;
	do
	{
		iter->parseLine(script1, script2);
		if (!(strcmp("TIP", script1))) x--;
	}
	while ((x>0)&&script1[0]!='}'&&script1[0]!=0 && (++loopexit < MAXLOOPS) );

	safedelete(iter);

	if (!(strcmp("}", script1)))
	{
		tips(s, 1, want_next);
		return;
	}

	sprintf(temp, "SECTION TIP %i", str2num(script2));
    iter = Scripts::Misc->getNewIterator(temp);

	if (iter==NULL) return;
	strcpy(script1, iter->getEntry()->getFullLine().c_str());//discards the {

	x=-1;
	y=-2;
	loopexit=0;
	do
	{
		strcpy(script1, iter->getEntry()->getFullLine().c_str());
		x++;
		y+=strlen(script1)+1;
	}
	while ( (strcmp(script1, "}")) && (++loopexit < MAXLOOPS) );
	y+=10;
	iter->rewind();
	strcpy(script1, iter->getEntry()->getFullLine().c_str());//discards the {

	uint8_t updscroll[10]={ 0xA6, 0x00, };
	ShortToCharPtr(y, updscroll +1); 		// len of pkt.
	updscroll[3]=0; 				// Type: 0x00 tips, 0x02 updates
	LongToCharPtr(i, updscroll +4);			// tip num.
	ShortToCharPtr(y-10, updscroll +8);		// len of only mess.
	Xsend(s, updscroll, 10); 		// Send 1st part (header)

	for (j=0;j<x;j++)
	{
		strcpy(script1, iter->getEntry()->getFullLine().c_str());//discards the {
		sprintf(temp, "%s ", script1);
		Xsend(s, temp, strlen(temp)); // Send the rest
	}
	safedelete(iter);

//AoS/	Network->FlushBuffer(s);
}


void deny(NXWSOCKET  s, pChar pc, int sequence)
{
	cPacketWalkReject walkdeny;
	walkdeny.sequence= sequence;
	walkdeny.x= pc->getPosition().x;
	walkdeny.y= pc->getPosition().y;
	walkdeny.direction=pc->dir;
	walkdeny.z= pc->getPosition().dispz;
	walkdeny.send( pc->getClient() );
	walksequence[s]=INVALID;
}

void broadcast(int s) // GM Broadcast (Done if a GM yells something)
//Modified by N6 to use UNICODE packets
{
	pChar pc=MAKE_CHAR_REF(currchar[s]);
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
			strcpy((char *)name, pc->getCurrentNameC());

			NxwSocketWrapper sw;
			sw.fillOnline();
			for( sw.rewind(); !sw.isEmpty(); sw++ )
			{
				NXWSOCKET i=sw.getSocket();

				SendSpeechMessagePkt(i, id, model, 1, color, font, name, (char*)&buffer[s][8]);
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
			strcpy((char *)name, pc->getCurrentNameC());

			NxwSocketWrapper sw;
			sw.fillOnline();
			for( sw.rewind(); !sw.isEmpty(); sw++ )
			{
				NXWSOCKET i=sw.getSocket();
				SendUnicodeSpeechMessagePkt(i, id, model, 1, color, font, lang, name, unicodetext,  ucl);
			}
		}
}

void itemtalk(pItem pi, char *txt)
// Item "speech"
//Modified by N6 to use UNICODE packets
{

	if ( ! pi ) return;

	NxwSocketWrapper sw;
	sw.fillOnline( pi );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		NXWSOCKET s=sw.getSocket();
		if(s==INVALID) continue;

		uint8_t unicodetext[512];
		uint16_t ucl = ( strlen ( txt ) * 2 ) + 2 ;

		char2wchar(txt);
		memcpy(unicodetext, Unicode::temp, ucl);

		uint32_t lang = calcserial(server_data.Unicodelanguage[0], server_data.Unicodelanguage[1], server_data.Unicodelanguage[2], 0);
		uint8_t name[30]={ 0x00, };
		strcpy((char *)name, pi->getCurrentNameC());

		SendUnicodeSpeechMessagePkt(s, pi->getSerial(), pi->getId(), 0, 0x0481, 0x0003, lang, name, unicodetext,  ucl);

	}
}

// Last touch: LB 8'th April 2001 for particleSystem

// if UO_3DonlyEffect is true, sta has to be valid and contain particleSystem data (if not, crash)
// for particleSystem data layout see staticeffectUO3d and updated packetDoku
// for old 2d staticeffect stuff , the new (3d client) pararamters UO_3Donlyeffect, sta and skip_old are defaulted in such a way that they behave like they did before
// simply dont set them in that case
// the last parameter is for particlesystem optimization only (dangerous). don't use unless you know 101% what you are doing.

void staticeffect(CHARACTER player, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop,  bool UO3DonlyEffekt, ParticleFx *sta, bool skip_old)
{
	pChar pc=MAKE_CHAR_REF(player);
	if ( ! pc ) return;

	uint16_t eff = (eff1<<8)|(eff2%256);
	uint8_t effect[28]={ 0x70, 0x00, };

    	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	 int a0,a1,a2,a3,a4;
	 Location charpos= pc->getPosition();

	 if (!skip_old)
	 {
Location pos2;
pos2.x = 0; pos2.y = 0; pos2.z = 0;
MakeGraphicalEffectPkt_(effect, 0x03, pc->getSerial(), 0, eff, charpos, pos2, speed, loop, 1, 0);
	 }

	 if (!UO3DonlyEffekt) // no UO3D effect ? lets send old effect to all clients
	 {

		 NxwSocketWrapper sw;
		 sw.fillOnline( pc, false );
		 for( sw.rewind(); !sw.isEmpty(); sw++ )
		 {
			NXWSOCKET s = sw.getSocket();
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
			 NXWSOCKET j=sw.getSocket();
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
				//sprintf(temp, "a0: %x a1: %x a2: %x a3: %x a4: %x \n",a0,a1,a2,a3,a4);
				//ConOut(temp);
			 }
			 else if (clientDimension[j] != 2 && clientDimension[j] !=3 ) { sprintf(temp, "Invalid Client Dimension: %i\n",clientDimension[j]); LogError(temp); } // attention: a simple else is wrong !

	   } // end for
	} // end UO:3D effect

	// remark: if a UO:3D effect is send and ALL clients are UO:3D ones, the pre-calculation of the 2-d packet
	// is redundant. but we can never know, and probably it will take years till the 2d cliet dies.
	// I think it's too infrequnet to consider this as optimization.
}


void movingeffect(CHARACTER source, CHARACTER dest, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode, bool UO3DonlyEffekt, ParticleFx *str, bool skip_old )
{

	pChar src=MAKE_CHAR_REF(source);
	if ( ! src ) return;
	pChar dst=MAKE_CHAR_REF(dest);
	VALIDATEPC(dst);

	uint16_t eff = (eff1<<8)|(eff2%256);
	uint8_t effect[28]={ 0x70, 0x00, };

 	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	Location srcpos= src->getPosition();
	Location destpos= dst->getPosition();

	if (!skip_old)
	{
MakeGraphicalEffectPkt_(effect, 0x00, src->getSerial(), dst->getSerial32(), eff, srcpos, destpos, speed, loop, 0, explode);
	}

	 if (!UO3DonlyEffekt) // no UO3D effect ? lets send old effect to all clients
	 {

		 NxwSocketWrapper sw;
		 sw.fillOnline( );
		 for( sw.rewind(); !sw.isEmpty(); sw++ )
		 {
			 NXWSOCKET j=sw.getSocket();
			 pChar pj = MAKE_CHAR_REF(currchar[j]);
			 if ( src->hasInRange(pj) && pj->hasInRange(dst) && clientInfo[j]->ingame )
			 {
				Xsend(j, effect, 28);
//AoS/				Network->FlushBuffer(j);
			 }
		 }
	   return;
	}
	else
	{
		// UO3D effect -> let's check which client can see it

		NxwSocketWrapper sw;
		sw.fillOnline();
		 for( sw.rewind(); !sw.isEmpty(); sw++ )
		 {
			 NXWSOCKET j=sw.getSocket();
			 pChar pj = MAKE_CHAR_REF(currchar[j]);
			 if ( src->hasInRange(pj) && pj->hasInRange(dst) && clientInfo[j]->ingame )
			 {
				 if (clientDimension[j]==2 && !skip_old) // 2D client, send old style'd
				 {
					 Xsend(j, effect, 28);
//AoS/					Network->FlushBuffer(j);
				 } else if (clientDimension[j]==3) // 3d client, send 3d-Particles
				 {

					movingeffectUO3D(source, dest, str);
					unsigned char particleSystem[49];
					Xsend(j, particleSystem, 49);
//AoS/					Network->FlushBuffer(j);
				}
				else if (clientDimension[j] != 2 && clientDimension[j] !=3 ) { sprintf(temp, "Invalid Client Dimension: %i\n",clientDimension[j]); LogError(temp); }
			}
		}
	}
}

// staticeffect2 is for effects on items
void staticeffect2(pItem pi, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode, bool UO3DonlyEffekt,  ParticleFx *str, bool skip_old )
{
	if ( ! pi ) return;

	uint16_t eff = (eff1<<8)|(eff2%256);
	uint8_t effect[28]={ 0x70, 0x00, };

 	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var

	Location pos = pi->getPosition();

	if (!skip_old)
	{
		MakeGraphicalEffectPkt_(effect, 0x02, pi->getSerial(), pi->getSerial32(), eff, pos, pos, speed, loop, 1, explode);
	}

	if (!UO3DonlyEffekt) // no UO3D effect ? lets send old effect to all clients
	{
		 NxwSocketWrapper sw;
		 sw.fillOnline( pi );
		 for( sw.rewind(); !sw.isEmpty(); sw++ )
		 {
			 NXWSOCKET j=sw.getSocket();
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
			 NXWSOCKET j=sw.getSocket();
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
				{ sprintf(temp, "Invalid Client Dimension: %i\n",clientDimension[j]); LogError(temp); }
			}
		}
	}
}


void bolteffect2(CHARACTER player,char a1,char a2)	// experimenatal, lb
{
	pChar pc=MAKE_CHAR_REF(player);
	if ( ! pc ) return;

	uint16_t eff = (a1<<8)|(a2%256);
	uint8_t effect[28]={ 0x70, 0x00, };

	int x,y;
	Location charpos = pc->getPosition(), pos2;

	y=rand()%36;
	x=rand()%36;

	if (rand()%2==0) x=x*-1;
	if (rand()%2==0) y=y*-1;
	pos2.x = charpos.x + x;
	pos2.y = charpos.y + y;
	if (pos2.x<0) pos2.x=0;
	if (pos2.y<0) pos2.y=0;
	if (pos2.x>6144) pos2.x=6144;
	if (pos2.y>4096) pos2.y=4096;

charpos.z = 0; pos2.z = 127;
MakeGraphicalEffectPkt_(effect, 0x00, pc->getSerial(), 0, eff, charpos, pos2, 0, 0, 1, 0);

	// ConOut("bolt: %i %i %i %i %i %i\n",x2,y2,chars[player].x,chars[player].y,x,y);

	 NxwSocketWrapper sw;
	 sw.fillOnline( pc );
	 for( sw.rewind(); !sw.isEmpty(); sw++ )
	 {
		NXWSOCKET j=sw.getSocket();
		if( j!=INVALID )
		{
			Xsend(j, effect, 28);
//AoS/			Network->FlushBuffer(j);
		}
	}
}

//	- Movingeffect3 is used to send an object from a char
//    to another object (like purple potions)
void movingeffect3(CHARACTER source, unsigned short x, unsigned short y, signed char z, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode)
{

	pChar src=MAKE_CHAR_REF(source);
	if ( ! src ) return;

	uint16_t eff = (eff1<<8)|(eff2%256);
	uint8_t effect[28]={ 0x70, 0x00, };

	Location srcpos= src->getPosition(), pos2 = { x, y, z, 0};

MakeGraphicalEffectPkt_(effect, 0x00, src->getSerial(), 0, eff, srcpos, pos2, speed, loop, 0, explode);

	 NxwSocketWrapper sw;
	 sw.fillOnline( src );
	 for( sw.rewind(); !sw.isEmpty(); sw++ )
	 {
		NXWSOCKET j=sw.getSocket();
		if( j!=INVALID )
		{
			Xsend(j, effect, 28);
//AoS/			Network->FlushBuffer(j);
		}
	 }

}

// staticeffect3 is for effects on items
void staticeffect3(uint16_t x, uint16_t y, int8_t z, unsigned char eff1, unsigned char eff2, char speed, char loop, char explode)
{
	uint16_t eff = (eff1<<8)|(eff2%256);
	uint8_t effect[28]={ 0x70, 0x00, };

Location pos = { x, y, z, 0};

MakeGraphicalEffectPkt_(effect, 0x02, 0, 0, eff, pos, pos, speed, loop, 1, explode);

pos.z = 0;

	 NxwSocketWrapper sw;
	 sw.fillOnline( pos );
	 for( sw.rewind(); !sw.isEmpty(); sw++ )
	 {
		NXWSOCKET j=sw.getSocket();
		if( j!=INVALID )
		{
			Xsend(j, effect, 28);
//AoS/			Network->FlushBuffer(j);
		}
	}
}

void movingeffect3(CHARACTER source, CHARACTER dest, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode,unsigned char unk1,unsigned char unk2,unsigned char ajust,unsigned char type)
{
	pChar src=MAKE_CHAR_REF(source);
	if ( ! src ) return;
	pChar dst=MAKE_CHAR_REF(dest);
	VALIDATEPC(dst);


	//0x0f 0x42 = arrow 0x1b 0xfe=bolt
	uint16_t eff = (eff1<<8)|(eff2%256);
	uint8_t effect[28]={ 0x70, 0x00, };

	Location srcpos= src->getPosition();
	Location destpos= dst->getPosition();

MakeGraphicalEffectPkt_(effect, type, src->getSerial(), dst->getSerial32(), eff, srcpos, destpos, speed, loop, ajust, explode);

	 NxwSocketWrapper sw;
	 sw.fillOnline( );
	 for( sw.rewind(); !sw.isEmpty(); sw++ )
	 {
		NXWSOCKET j=sw.getSocket();
		if( j!=INVALID )
		{
			Xsend(j, effect, 28);
//AoS/			Network->FlushBuffer(j);
		}
	}
}



//	- Movingeffect2 is used to send an object from a char
//	to another object (like purple potions)
void movingeffect2(CHARACTER source, int dest, unsigned char eff1, unsigned char eff2, unsigned char speed, unsigned char loop, unsigned char explode)
{
	//0x0f 0x42 = arrow 0x1b 0xfe=bolt

	const pItem pi=MAKE_ITEM_REF(dest);
	if ( ! pi ) return;
	pChar pc_source = MAKE_CHAR_REF(source);
	VALIDATEPC(pc_source);

	uint16_t eff = (eff1<<8)|(eff2%256);
	uint8_t effect[28]={ 0x70, 0x00, };

	Location srcpos= pc_source->getPosition(), pos2 = pi->getPosition();

MakeGraphicalEffectPkt_(effect, 0x00, pc_source->getSerial(), pi->getSerial32(), eff, srcpos, pos2, speed, loop, 0, explode);

	 NxwSocketWrapper sw;
	 sw.fillOnline( );
	 for( sw.rewind(); !sw.isEmpty(); sw++ )
	 {
		NXWSOCKET j=sw.getSocket();
		if( j!=INVALID )
		{
			Xsend(j, effect, 28);
//AoS/			Network->FlushBuffer(j);
		}
	}
}

void SendPauseResumePkt(NXWSOCKET s, uint8_t flag)
{
/* Flag: 0=pause, 1=resume */ // uhm.... O_o ... or viceversa ? -_-;
	uint8_t m2[2]={ 0x33, 0x00 };

	m2[1]=flag;
	Xsend(s, m2, 2);
//AoS/	Network->FlushBuffer(s);
}

void SendDeleteObjectPkt(NXWSOCKET s, uint32_t serial)
{
	uint8_t removeitem[5] = { 0x1D, 0x00, };
	LongToCharPtr(serial, removeitem +1);

	Xsend(s, removeitem, 5);
//AoS/	Network->FlushBuffer(s);
}

void SendDrawGamePlayerPkt(NXWSOCKET s, uint32_t player_id, uint16_t model, uint8_t unk1, uint16_t color, uint8_t flag, Location pos, uint16_t unk2, uint8_t dir, bool useDispZ)
{
	uint8_t goxyz[19]={ 0x20, 0x00, };

	LongToCharPtr(player_id, goxyz +1);
	ShortToCharPtr(model, goxyz +5);
	goxyz[7] = unk1;
	ShortToCharPtr(color, goxyz +8);
	goxyz[10] = flag;
	ShortToCharPtr(pos.x, goxyz +11);
	ShortToCharPtr(pos.y, goxyz +13);
	ShortToCharPtr(unk2, goxyz +15);
	goxyz[17]= dir;
	goxyz[18]= (useDispZ)? pos.dispz : pos.z;
	Xsend(s, goxyz, 19);
//AoS/	Network->FlushBuffer(s);
}

void SendDrawObjectPkt(NXWSOCKET s, pChar pc, int z)
{
	pChar pc_currchar=MAKE_CHAR_REF(currchar[s]);
	VALIDATEPC(pc_currchar);
	uint32_t k;
	uint8_t oc[1024]={ 0x78, 0x00, };

	Location charpos = pc->getPosition();

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

	for (int j=0;j<MAXLAYERS;j++) layers[j] = 0;

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



void SendSpeechMessagePkt(NXWSOCKET s, uint32_t id, uint16_t model, uint8_t type, uint16_t color, uint16_t fonttype, uint8_t sysname[30],  char *text)
{
        uint16_t tl, len = strlen((char *)text) + 1;
        uint8_t talk[14]={ 0x1C, 0x00, };

        tl = 14 + 30  + len;  // 44(header) + len + null term.

	ShortToCharPtr(tl, talk +1);
	LongToCharPtr(id, talk +3);
	ShortToCharPtr(model, talk +7);
	talk[9]=type;
	ShortToCharPtr(color, talk +10);
	ShortToCharPtr(fonttype, talk +12);

	Xsend(s, talk, 14);
	Xsend(s, sysname, 30);
	Xsend(s, text, len);
//AoS/	Network->FlushBuffer(s);
}


void SendUnicodeSpeechMessagePkt(NXWSOCKET s, uint32_t id, uint16_t model, uint8_t type, uint16_t color, uint16_t fonttype, uint32_t lang, uint8_t sysname[30], uint8_t *unicodetext, uint16_t unicodelen)
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

void SendPlaySoundEffectPkt(NXWSOCKET s, uint8_t mode, uint16_t sound_model, uint16_t unkn, Location pos, bool useDispZ)
{
	uint8_t sfx[12]={ 0x54, 0x00, };
	int16_t Z;

	Z = (useDispZ)? pos.dispz : pos.z;

	sfx[1] = mode;					// Mode: 0x00 repeating, 0x01 single
	ShortToCharPtr(sound_model, sfx +2);		// Sound model
	ShortToCharPtr(unkn, sfx +4);			// unkn, (speed/volume modifier? Line of sight stuff?)
	ShortToCharPtr(pos.x, sfx +6);			// POS:  X
	ShortToCharPtr(pos.y, sfx +8);			//       Y
	ShortToCharPtr(Z , sfx +10);			//       Z
	Xsend(s, sfx, 12);
//AoS/	Network->FlushBuffer(s);
}

void impowncreate(NXWSOCKET s, pChar pc, int z) //socket, player to send
{
        if ( s < 0 || s > now ) // Luxor
		return;
	pChar pc_currchar=MAKE_CHAR_REF(currchar[s]);
	VALIDATEPC(pc_currchar);

	if (pc->isStabled() || pc->mounted)
		return; // dont **show** stabled pets

	bool sendit = true; //Luxor bug fix
	if (pc->IsHidden() && pc->getSerial()!=pc_currchar->getSerial32() && !pc_currchar->IsGM())
		sendit=false;

	if( !pc->npc && !pc->IsOnline()  && !pc_currchar->IsGM() )
	{
		sendit=false;
		SendDeleteObjectPkt(s, pc->getSerial());
	}
	// hidden chars can only be seen "grey" by themselves or by gm's
	// other wise they are invisible=dont send the packet
	if (!sendit)
		return;

	SendDrawObjectPkt(s, pc, z);
	//pc_currchar->sysmsg( "sended %s", pc->getCurrentNameC() );
}

void sendshopinfo(int s, int c, pItem pi)
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
				value=pj->value;
				value=calcValue(DEREF_pItem(pj), value);
				if (SrvParms->trade_system==1)
					value=calcGoodValue(c,DEREF_pItem(pj),value,0); // by Magius(CHE)
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

int sellstuff(NXWSOCKET s, CHARACTER i)
{
	if (s < 0 || s >= now) return 0; //Luxor
    pChar pc = MAKE_CHAR_REF(i);
	VALIDATEPCR(pc, 0);
	pChar pcs = MAKE_CHAR_REF(currchar[s]);
	VALIDATEPCR(pcs,0);

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
	VALIDATEPIR(pp,0);

	SendPauseResumePkt(s, 0x01);

	pItem pack= pcs->getBackpack();
	VALIDATEPIR(pack, 0);

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
		if (pj)
		{

			for( s_pack.rewind(); !s_pack.isEmpty(); s_pack++ )
			{
				if (m1[8] >= 50) continue;

				pItem pj1 = s_pack.getItem();
				if ( pj1 ) // LB crashfix
				{
					sprintf(ciname,"'%s'",pj1->getCurrentNameC()); // Added by Magius(CHE)
					sprintf(cinam2,"'%s'",pj->getCurrentNameC()); // Added by Magius(CHE)
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
						value=calcValue(DEREF_pItem(pj1), value);
						if (SrvParms->trade_system==1)
							value=calcGoodValue(i,DEREF_pItem(pj1),value,1); // by Magius(CHE)
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
			pc->talkAll(TRANSLATE("Thou dont posses nothing of interest to me."),0);
		}
	}
	else
	{
			pc->talkAll( TRANSLATE("Sorry i cannot take so many items.."),0);
	}

	SendPauseResumePkt(s, 0x00);

	return 1;
}



void tellmessage(int i, int s, char *txt)
//Modified by N6 to use UNICODE packets
{

	pChar pc=MAKE_CHAR_REF(currchar[s]);
	if ( ! pc ) return;

	uint8_t unicodetext[512];
 	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var

	sprintf(temp, TRANSLATE("GM tells %s: %s"), pc->getCurrentNameC(), txt);

	uint16_t ucl = ( strlen ( temp ) * 2 ) + 2 ;
	char2wchar(temp);
	memcpy(unicodetext, Unicode::temp, ucl);

	uint32_t lang = calcserial(server_data.Unicodelanguage[0], server_data.Unicodelanguage[1], server_data.Unicodelanguage[2], 0);
	uint8_t sysname[30]={ 0x00, };
	strcpy((char *)sysname, "System");

	SendUnicodeSpeechMessagePkt(s, 0x01010101, 0x0101, 0, 0x0035, 0x0003, lang, sysname, unicodetext,  ucl);
	SendUnicodeSpeechMessagePkt(i, 0x01010101, 0x0101, 0, 0x0035, 0x0003, lang, sysname, unicodetext,  ucl); //So Person who said it can see too

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


void staticeffectUO3D(CHARACTER player, ParticleFx *sta)
{

   PC_CHAR pc_cs=MAKE_CHAR_REF(player);
   VALIDATEPC(pc_cs);
   Location charpos= pc_cs->getPosition();

   // please no optimization of p[...]=0's yet :)

   unsigned char particleSystem[49];
   particleSystem[0]= 0xc7;
   particleSystem[1]= 0x3;

   particleSystem[2]= pc_cs->getSerial().ser1;
   particleSystem[3]= pc_cs->getSerial().ser2;
   particleSystem[4]= pc_cs->getSerial().ser3;
   particleSystem[5]= pc_cs->getSerial().ser4;

   particleSystem[6]= 0x0; // always 0 for this type
   particleSystem[7]= 0x0;
   particleSystem[8]= 0x0;
   particleSystem[9]= 0x0;

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

   particleSystem[42]=pc_cs->getSerial().ser1;
   particleSystem[43]=pc_cs->getSerial().ser2;
   particleSystem[44]=pc_cs->getSerial().ser3;
   particleSystem[45]=pc_cs->getSerial().ser4;

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

void movingeffectUO3D(CHARACTER source, CHARACTER dest, ParticleFx *sta)
{


   PC_CHAR pc_cs=MAKE_CHAR_REF(source);
   VALIDATEPC(pc_cs);
   PC_CHAR pc_cd=MAKE_CHAR_REF(dest);
   VALIDATEPC(pc_cd);

   Location srcpos= pc_cs->getPosition();
   Location destpos= pc_cd->getPosition();

   unsigned char particleSystem[49];
   particleSystem[0]=0xc7;
   particleSystem[1]=0x0;

   particleSystem[2]=pc_cs->getSerial().ser1;
   particleSystem[3]=pc_cs->getSerial().ser2;
   particleSystem[4]=pc_cs->getSerial().ser3;
   particleSystem[5]=pc_cs->getSerial().ser4;

   particleSystem[6]=pc_cd->getSerial().ser1;
   particleSystem[7]=pc_cd->getSerial().ser2;
   particleSystem[8]=pc_cd->getSerial().ser3;
   particleSystem[9]=pc_cd->getSerial().ser4;

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

   particleSystem[42]=0x00;
   particleSystem[43]=0x00;
   particleSystem[44]=0x00;
   particleSystem[45]=0x00;

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
	{
		particleSystem[2]= pi->getSerial().ser1;
		particleSystem[3]= pi->getSerial().ser2;
		particleSystem[4]= pi->getSerial().ser3;
		particleSystem[5]= pi->getSerial().ser4;
	}
	else
	{
		particleSystem[2]=0x00;
		particleSystem[3]=0x00;
		particleSystem[4]=0x00;
		particleSystem[5]=0x00;
	}

	particleSystem[6]=0x0; // always 0 for this type
	particleSystem[7]=0x0;
	particleSystem[8]=0x0;
	particleSystem[9]=0x0;

	particleSystem[10]=sta->effect[4]; // tileid1
	particleSystem[11]=sta->effect[5]; // tileid2

	particleSystem[12]= pi->getPosition("x") >> 8;
	particleSystem[13]= pi->getPosition("x") % 256;
	particleSystem[14]= pi->getPosition("y") >> 8;
	particleSystem[15]= pi->getPosition("y") % 256;
	particleSystem[16]= pi->getPosition("z");

	particleSystem[17]= pi->getPosition("x") >> 8;
	particleSystem[18]= pi->getPosition("x") % 256;
	particleSystem[19]= pi->getPosition("y") >> 8;
	particleSystem[20]= pi->getPosition("y") % 256;
	particleSystem[21]= pi->getPosition("z") ;

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

	particleSystem[42]= pi->getSerial().ser1;
	particleSystem[43]= pi->getSerial().ser2;
	particleSystem[44]= pi->getSerial().ser3;
	particleSystem[45]= pi->getSerial().ser4;

	particleSystem[46]=0xff;

	particleSystem[47]=0x0;
	particleSystem[48]=0x0;

}

void sysmessageflat(NXWSOCKET  s, short color, const char *txt)
// System message (In lower left corner)
//Modified by N6 to use UNICODE packets
{
	uint8_t unicodetext[512];
	uint16_t ucl = ( strlen ( txt ) * 2 ) + 2 ;

	char2wchar(txt);
	memcpy(unicodetext, Unicode::temp, ucl);

	uint32_t lang = calcserial(server_data.Unicodelanguage[0], server_data.Unicodelanguage[1], server_data.Unicodelanguage[2], 0);
	uint8_t sysname[30]={ 0x00, };
	strcpy((char *)sysname, "System");

	SendUnicodeSpeechMessagePkt(s, 0x01010101, 0x0101, 6, color, 0x0003, lang, sysname, unicodetext,  ucl);

}

void wornitems(NXWSOCKET  s, pChar pc) // Send worn items of player
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

