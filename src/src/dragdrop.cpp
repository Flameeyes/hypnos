  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

// dragdrop.cpp: implementation of dragging and dropping
// cut from NoX-Wizard.cpp by Duke, 23.9.2000
//////////////////////////////////////////////////////////////////////

/* 									CHANGELOG
	--------------------------------------------------------------------------------------------------------------------
	Date		Developer	Description
	--------------------------------------------------------------------------------------------------------------------
 	20-02-2002  	Sparhawk 	Fixed disappearing items after dragging them on paperdoll, set checks in item_bouncex()
					from id1 >= 0x40 to id1 < 0x40
					Removed obsolete item checks in wear_item()
					Added bounce_item call when non gm player tries to drag item on paperdoll of gm.
	24-02-2002	Sparhawk	Fixed	compile error on void return values in CheckWhereItem and cleaned code
	15-03-2002	Sparhawk	Optimized abstract container size() in loops

*/


#include "nxwcommn.h"
#include "network.h"
#include "debug.h"
#include "basics.h"
#include "sndpkg.h"
#include "speech.h"
#include "itemid.h"
#include "bounty.h"
#include "srvparms.h"
#include "set.h"
#include "layer.h"
#include "npcai.h"
#include "nxw_utils.h"
#include "weight.h"
#include "data.h"
#include "books.h"
#include "boats.h"
#include "archive.h"
#include "trade.h"
#include "map.h"
#include "items.h"
#include "chars.h"
#include "inlines.h"
#include "skills.h"
#include "nox-wizard.h"
#include "party.h"



typedef struct _PKGx08
{
//0x08 Packet
//Drop Item(s) (14 bytes)
//* BYTE cmd
//* BYTE[4] item id
	long Iserial;
//* BYTE[2] xLoc
	short TxLoc;
//* BYTE[2] yLoc
	short TyLoc;
//* BYTE zLoc
	signed char TzLoc;
//* BYTE[4] Move Into (FF FF FF FF if normal world)
	long Tserial;
} PKGx08;

void UpdateStatusWindow(NXWSOCKET socket, P_ITEM pi)
{
	P_CHAR pc = MAKE_CHAR_REF( currchar[socket] );
	VALIDATEPC( pc );
	VALIDATEPI( pi );
	P_ITEM pack = pc->getBackpack();
	VALIDATEPI( pack );

	if( pi->getContainer() != pack || pi->getContainer() == pc )
		statwindow( pc, pc );
}

static void Sndbounce5( NXWSOCKET socket )
{
	if ( socket >= 0 && socket < now)
	{
		UI08 bounce[2]= { 0x27, 0x00 };
		bounce[1] = 5;
		Xsend(socket, bounce, 2);
	}
}

// Name:	item_bounce3
// Purpose:	holds some statements that were COPIED some 50 times
// Remarks:	temporary functions to revamp the 30 occurences of the 'bouncing bugfix'
// History:	init Duke, 10.8.2000 / bugfix for bonus stats, Xanathar, 05-aug-2001
static void item_bounce3(const P_ITEM pi)
{
	VALIDATEPI( pi );
	pi->setContainer( pi->getOldContainer() );
	pi->setPosition( pi->getOldPosition() );
	pi->layer=pi->oldlayer;

	P_CHAR pc = (pChar) pi->getOldContainer();
	if(pc)
		return ;

	if ( pi->layer > 0 )
	{
		// Xanathar -- add BONUS STATS given by equipped special items
		pc->setStrength( pc->getStrength() + pi->st2, true );
		//pc->st += pi->st2;
		pc->dx += pi->dx2;
		pc->in += pi->in2;
		// Xanathar -- for poisoned items
		if (pi->poisoned)
		{
			pc->poison += pi->poisoned;
			if ( pc->poison < 0)
				pc->poison = 0;
		}
	}
}

static void item_bounce4(const NXWSOCKET  socket, const P_ITEM pi)
{
	VALIDATEPI( pi );
	item_bounce3(pi);
	if( (pi->getId() >>8) < 0x40)
		senditem( socket, pi );
}

static void item_bounce5(const NXWSOCKET socket, const P_ITEM pi)
{
	VALIDATEPI( pi );
	item_bounce3(pi);
	senditem(socket, pi);
}

static void item_bounce6(const NXWCLIENT client, const P_ITEM pi)
{
	if ( client != NULL )
	{
		VALIDATEPI(pi);
		Sndbounce5( client->toInt() );
		if ( client->isDragging() )
		{
			client->resetDragging();
			item_bounce4( client->toInt(), pi );
		}
	}
}



void wear_item(NXWCLIENT ps) // Item is dropped on paperdoll
{
	if ( ps == NULL )
		return;
	NXWSOCKET s = ps->toInt();
	if (s < 0)
		return;
	P_CHAR pc=ps->currChar();
	VALIDATEPC( pc );
	P_CHAR pck = pointers::findCharBySerPtr(buffer[s]+6);
	VALIDATEPC( pck );
	if( pck->dead )  //Exploit fix: Dead ppl can't equip anything.
		return;
	P_ITEM pi=pointers::findItemBySerPtr(buffer[s]+1);
	VALIDATEPI(pi);

	bool resetDragging = false;

	if( (pi->getId()>>8) >= 0x40)  // LB, client crashfix if multi-objects are moved to PD
		resetDragging = true;

	tile_st tile;
	int serial/*, letsbounce=0*/; // AntiChrist (5) - new ITEMHAND system

	data::seekTile(pi->getId(), tile);

	if( ( clientDimension[s]==3 ) &&  (tile.quality==0) )
	{
		ps->sysmsg(TRANSLATE("You can't wear that"));
		resetDragging = true;
	}
	else {
		P_ITEM outmost = pi->getOutMostCont();
		P_CHAR vendor = (pChar) outmost->getContainer();
		if( ISVALIDPC( vendor ) && ( vendor->getOwnerSerial32() != pc->getSerial() ) )
		{
			resetDragging = true;
		}
	}

	if( resetDragging ) {
		Sndbounce5(s);
		if (ps->isDragging())
		{
			ps->resetDragging();
			item_bounce4(s,pi);
			UpdateStatusWindow(s,pi);
		}
		return;
	}

	if ( pck == pc || pc->IsGM() )
	{

		if ( !pc->IsGM() && pi->st > pck->getStrength() && !pi->isNewbie() ) // now you can equip anything if it's newbie
		{
			ps->sysmsg(TRANSLATE("You are not strong enough to use that."));
			resetDragging = true;
		}
		else if ( !pc->IsGM() && !pi->checkItemUsability(pc, ITEM_USE_WEAR) )
		{
			resetDragging = true;
		}
		else if ( (pc->getId() == BODY_MALE) && ( pi->getId()==0x1c00 || pi->getId()==0x1c02 || pi->getId()==0x1c04 || pi->getId()==0x1c06 || pi->getId()==0x1c08 || pi->getId()==0x1c0a || pi->getId()==0x1c0c ) ) // Ripper...so males cant wear female armor
		{
			ps->sysmsg(TRANSLATE("You cant wear female armor!"));
			resetDragging = true;
		}
		else if ((((pi->magic==2)||((tile.weight==255)&&(pi->magic!=1))) && !pc->canAllMove()) ||
				( (pi->magic==3|| pi->magic==4) && !(pi->getOwnerSerial32() == pc->getSerial)) )
		{
			resetDragging = true;
		}

		if( resetDragging ) {
			Sndbounce5(s);
			if (ps->isDragging())
			{
				ps->resetDragging();
				item_bounce4(s,pi);
				UpdateStatusWindow(s,pi);
			}
			return;
		}



		// - AntiChrist (4) - checks for new ITEMHAND system
		// - now you can't equip 2 hnd weapons with 1hnd weapons nor shields!!
		serial= pck->getSerial32(); //xan -> k not cc :)

		P_ITEM pj = NULL;
 		P_CHAR pc_currchar= pck;
// 		P_ITEM pack= pc_currchar->getBackpack();
                //<Luxor>

		P_ITEM pW = pc_currchar->getWeapon();
		if (tile.quality == 1 || tile.quality == 2)
		{ //weapons layers
			if ( (pi->layer == LAYER_2HANDWEAPON && ISVALIDPI(pc_currchar->getShield())) )
			{
				ps->sysmsg("You cannot wear two weapons.");
				Sndbounce5(s);
				if (ps->isDragging())
				{
        			ps->resetDragging();
					UpdateStatusWindow(s,pi);
	        	}
				pi->setContainer( pi->getOldContainer() );
				pi->setPosition( pi->getOldPosition() );
				pi->layer = pi->oldlayer;
				pi->Refresh();
				return;
			}
			if (ISVALIDPI(pW))
			{
				if (pi->itmhand != 3 && pi->lodamage != 0 && pi->itmhand == pW->itmhand)
				{
					ps->sysmsg("You cannot wear two weapons.");
					Sndbounce5(s);
					if (ps->isDragging())
					{
						ps->resetDragging();
						UpdateStatusWindow(s,pi);
					}
					pi->setContainer( pi->getOldContainer() );
					pi->setPosition( pi->getOldPosition() );
					pi->layer = pi->oldlayer;
					pi->Refresh();
					return;
				}
			}
		}
		//</Luxor>

		if ( ServerScp::g_nUnequipOnReequip )
		{
			int drop[2]= {-1, -1};	// list of items to drop
									// there no reason for it to be larger
			int curindex= 0;

			NxwItemWrapper si;
			si.fillItemWeared( pc_currchar, false, true, true );
			for( si.rewind(); !si.isEmpty(); si++ )
			{
				// we CANNOT directly bounce the item, or the containersearch() function will not work
				// so we store the item ID in letsbounce, and at the end we bounce the item

				pj=si.getItem();
				if(!ISVALIDPI(pj))
					continue;

				if ((tile.quality == 1) || (tile.quality == 2))// weapons
				{
					if (pi->itmhand == 2) // two handed weapons or shield
					{
						if (pj->itmhand == 2)
							drop[curindex++]= DEREF_P_ITEM(pj);

						if ( (pj->itmhand == 1) || (pj->itmhand == 3) )
							drop[curindex++]= DEREF_P_ITEM(pj);
					}

					if (pi->itmhand == 3)
					{
						if ((pj->itmhand == 2) || pj->itmhand == 3)
							drop[curindex++]= DEREF_P_ITEM(pj);
					}

					if ((pi->itmhand == 1) && ((pj->itmhand == 2) || (pj->itmhand == 1)))
						drop[curindex++]= DEREF_P_ITEM(pj);
				}
				else	// not a weapon
				{
					if (pj->layer == tile.quality)
						drop[curindex++]= DEREF_P_ITEM(pj);
				}
			}

			if (ServerScp::g_nUnequipOnReequip)
			{
				if (drop[0] > -1)	// there is at least one item to drop
				{
					for (int i= 0; i< 2; i++)
					{
						if (drop[i] > -1)
						{
							P_ITEM p_drop=MAKE_ITEM_REF(drop[i]);
							if(ISVALIDPI(p_drop))
								pc_currchar->UnEquip( p_drop, 1);
						}
					}
				}
				pc->playSFX( itemsfx(pi->getId()) );
				pc_currchar->Equip(pi, 1);
			}
			else
			{
				if (drop[0] == -1)
				{
					pc->playSFX( itemsfx(pi->getId()) );
					pc_currchar->Equip(pi, 1);
				}
			}
		}

		if (!(pc->IsGM())) //Ripper..players cant equip items on other players or npc`s paperdolls.
		{
			if ((pck->getSerial32() != pc->getSerial32())/*&&(chars[s].npc!=k)*/) //-> really don't understand this! :|, xan
			{
				ps->sysmsg(TRANSLATE("You can't put items on other people!"));
				item_bounce6(ps,pi);
				return;
			}
		}

		NxwSocketWrapper sws;
		sws.fillOnline( pi );
		for( sws.rewind(); !sws.isEmpty(); sws++ )
			SendDeleteObjectPkt( sws.getSocket(), pi->getSerial32() );

		pi->layer=buffer[s][5];
		pi->setContSerial(LongFromCharPtr(buffer[s] +6));

		if (g_nShowLayers) InfoOut("Item equipped on layer %i.\n",pi->layer);

		wearIt(s,pi);

		NxwSocketWrapper sw;
		sw.fillOnline( pck, false );
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			NXWSOCKET j=sw.getSocket();
			if( j!=INVALID )
				wornitems(j, pck );
		}

		pc->playSFX( itemsfx(pi->getId()) );
		weights::NewCalc(pc);	// Ison 2-20-99
		statwindow(pc_currchar,pc_currchar);

//		if (pi->glow>0)
//		{
//			pc->removeHalo(pi); // if gm equips on differnt player it needs to be deleted out of the hashteble
//			pck->addHalo(pi);
//			pck->glowHalo(pi);
//		}

		if ( pck->Equip(pi, 1) == 2)	// bypass called
		{
			P_ITEM pack = pck->getBackpack();
			pc->playSFX( itemsfx(pi->getId()) );
			pi->layer= 0;
			pi->setContainer( pack );
			sendbpitem(s, pi);
			return;
		}

	}
}




