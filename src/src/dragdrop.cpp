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

static bool ItemDroppedOnPet(NXWCLIENT ps, PKGx08 *pp, P_ITEM pi)
{
	if (ps == NULL) return false;
	VALIDATEPIR(pi, false);
	P_CHAR pet = pointers::findCharBySerial(pp->Tserial);
	VALIDATEPCR(pet, false);
	NXWSOCKET  s = ps->toInt();
	P_CHAR pc = ps->currChar();
	VALIDATEPCR(pc, false);

	if((pet->hunger<6) && (pi->type==ITYPE_FOOD))//AntiChrist new hunger code for npcs
	{
		pc->playSFX( 0x3A+(rand()%3) );	//0x3A - 0x3C three different sounds

		if(pi->poisoned)
		{
			pet->applyPoison(PoisonType(pi->poisoned));
		}

		std::string itmname;
		if( pi->getCurrentName() == "#" )
		{
			char temp2[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
			pi->getName(temp2);
			itmname = temp2;
		}
		else itmname = pi->getCurrentName();

		pet->emotecolor = 0x0026;
		pet->emoteall(TRANSLATE("* You see %s eating %s *"), 1, pet->getCurrentNameC(), itmname.c_str() );
		pet->hunger++;
	} else
	{
		ps->sysmsg(TRANSLATE("It doesn't appear to want the item"));
		Sndbounce5(s);
		if (ps->isDragging())
		{
			ps->resetDragging();
			item_bounce5(s,pi);

		}
	}
	return true;
}

static bool ItemDroppedOnGuard(NXWCLIENT ps, PKGx08 *pp, P_ITEM pi)
{
	if (ps == NULL)
		return false;
	VALIDATEPIR(pi, false);
	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	NXWSOCKET  s=ps->toInt();
	P_CHAR pc = ps->currChar();
	VALIDATEPCR(pc,false);

	P_CHAR pc_t=pointers::findCharBySerial(pp->Tserial); //the guard
	VALIDATEPCR(pc_t,false);
	// Search for the key word "the head of"
	if( strstr( pi->getCurrentNameC(), "the head of" ) ) //!!! Wrong! it must check the ItemID, not the name :(
	{
		// This is a head of someone, see if the owner has a bounty on them
		P_CHAR own=pointers::findCharBySerial(pi->getOwnerSerial32());
		VALIDATEPCR(own,false);

		if( own->questBountyReward > 0 )
		{
			// Give the person the bounty assuming that they are not the
			// same person as the reward is for
			if( pc->getSerial32() != own->getSerial32() )
			{
				// give them the gold for bringing the villan to justice
				pc->addGold(own->questBountyReward);
				pc->playSFX( goldsfx( own->questBountyReward ) );

				// Now thank them for their hard work
				sprintf( temp, TRANSLATE("Excellent work! You have brought us the head of %s. Here is your reward of %d gold coins."),
					own->getCurrentNameC(), own->questBountyReward );
				pc_t->talk( s, temp, 0);

				// Delete the Bounty from the bulletin board
				BountyDelete(own );

				// xan : increment fame & karma :)
				pc->modifyFame( ServerScp::g_nBountyFameGain );
				pc->IncreaseKarma(ServerScp::g_nBountyKarmaGain);
			}
			else
				pc_t->talk( s, TRANSLATE("You can not claim that prize scoundrel. You are lucky I don't strike you down where you stand!"),0);

			// Delete the item
			pi->Delete();
		}
	}
	return true;
}

static bool ItemDroppedOnBeggar(NXWCLIENT ps, PKGx08 *pp, P_ITEM pi)
{
	if (ps == NULL)
		return false;

	VALIDATEPIR(pi, false);

	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var

	NXWSOCKET  s=ps->toInt();
	P_CHAR pc=ps->currChar();
	VALIDATEPCR(pc,false);

	P_CHAR pc_t=pointers::findCharBySerial(pp->Tserial); //beggar
	VALIDATEPCR(pc_t,false);

	if(pi->getId()!=ITEMID_GOLD)
	{
		sprintf(temp,TRANSLATE("Sorry %s i can only use gold"), pc->getCurrentNameC());
		pc_t->talk( s,temp,0);
		Sndbounce5(s);
		if (ps->isDragging())
		{
			ps->resetDragging();
			item_bounce5(s,pi);
			return true;
		}
	}
	else
	{
		sprintf(temp,TRANSLATE("Thank you %s for the %i gold!"), pc->getCurrentNameC(), pi->amount);
		pc_t->talk( s,temp,0);
		if(pi->amount<=100)
		{
			pc->IncreaseKarma(10);
			ps->sysmsg(TRANSLATE("You have gain a little karma!"));
		}
		else if(pi->amount>100)
		{
			pc->IncreaseKarma(50);
			ps->sysmsg(TRANSLATE("You have gain some karma!"));
		}
		pi->Delete();
		return true;
	}
	return true;
}

static bool ItemDroppedOnTrainer(NXWCLIENT ps, PKGx08 *pp, P_ITEM pi)
{
	if (ps == NULL) return false;
	VALIDATEPIR(pi, false);
	NXWSOCKET  s = ps->toInt();
	P_CHAR pc = ps->currChar();
	VALIDATEPCR(pc,false);
	P_CHAR pc_t = pointers::findCharBySerial(pp->Tserial);
	VALIDATEPCR(pc_t,false);

	if( pi->getId() == ITEMID_GOLD )
	{ // They gave the NPC gold
		UI08 sk=pc_t->trainingplayerin;
		pc_t->talk( s, TRANSLATE("I thank thee for thy payment. That should give thee a good start on thy way. Farewell!"),0);

		int sum = pc->getSkillSum();
		int delta = pc_t->getTeachingDelta(pc, sk, sum);

		if(pi->amount>delta) // Paid too much
		{
			pi->amount-=delta;

			Sndbounce5(s);
			if (ps->isDragging())
			{
				ps->resetDragging();
				item_bounce5(s,pi);
			}
		}
		else
		{
			if(pi->amount < delta)		// Gave less gold
				delta = pi->amount;		// so adjust skillgain
			pi->Delete();
		}
		pc->baseskill[sk]+=delta;
		Skills::updateSkillLevel(pc, sk);
		pc->updateSkill(sk);

		pc->trainer=-1;
		pc_t->trainingplayerin=0xFF;
		pc->playSFX( itemsfx(pi->getId()) );
	}
	else // Did not give gold
	{
		pc_t->talk( s, TRANSLATE("I am sorry, but I can only accept gold."),0);
		Sndbounce5(s);
		if (ps->isDragging())
		{
			ps->resetDragging();
			item_bounce5(s,pi);
		}
	}//if items[i]=gold
	return true;
}

static bool ItemDroppedOnSelf(NXWCLIENT ps, PKGx08 *pp, P_ITEM pi)
{
	if (ps == NULL) return false;
	VALIDATEPIR(pi, false);
//	NXWSOCKET  s=ps->toInt();
//	CHARACTER cc=ps->currCharIdx();
	P_CHAR pc = ps->currChar(); // MAKE_CHAR_REF(cc);
	VALIDATEPCR(pc, false);

	Location charpos = pc->getPosition();

	if (pi->getId() >= 0x4000 ) // crashfix , prevents putting multi-objects ni your backback
	{
		ps->sysmsg(TRANSLATE("Hey, putting houses in your pack crashes your back and client !"));
		pi->MoveTo( charpos );
		pi->Refresh();//AntiChrist
		return true;
	}

//	if (pi->glow>0) // glowing items
//	{
//		pc->addHalo(pi);
//		pc->glowHalo(pi);
//	}

	P_ITEM pack = pc->getBackpack();
	if (pack==NULL) // if player has no pack, put it at its feet
	{
		pi->MoveTo( charpos );
		pi->Refresh();
	}
	else
	{
		pack->AddItem(pi); // player has a pack, put it in there

		weights::NewCalc(pc);//AntiChrist bugfixes
		statwindow(pc,pc);
		pc->playSFX( itemsfx(pi->getId()) );
	}
	return true;
}

static bool ItemDroppedOnChar(NXWCLIENT ps, PKGx08 *pp, P_ITEM pi)
{
	if (ps == NULL) return true;
	VALIDATEPIR(pi, false);
	NXWSOCKET  s = ps->toInt();
//	CHARACTER cc=ps->currCharIdx();
	P_CHAR pTC = pointers::findCharBySerial(pp->Tserial);	// the targeted character
	VALIDATEPCR(pTC, false);
	P_CHAR pc_currchar = ps->currChar(); //MAKE_CHAR_REF(cc);
	VALIDATEPCR(pc_currchar, false);
	Location charpos = pc_currchar->getPosition();

	if (!pTC) return true;

	if (pc_currchar->getSerial32() != pTC->getSerial32() /*DEREF_P_CHAR(pTC)!=cc*/)
	{
		if (pTC->npc)
		{
			if(!pTC->HasHumanBody())
			{
				ItemDroppedOnPet( ps, pp, pi);
			}
			else	// Item dropped on a Human character
			{
				// Item dropped on a Guard (possible bounty quest)
				if( ( pTC->npc == 1 ) && ( pTC->npcaitype == NPCAI_TELEPORTGUARD ) )
				{
					ItemDroppedOnGuard( ps, pp, pi);
				}
				if ( pTC->npcaitype == NPCAI_BEGGAR )
				{
					ItemDroppedOnBeggar( ps, pp, pi);
				}

				//This crazy training stuff done by Anthracks (fred1117@tiac.net)
				if(pc_currchar->trainer != pTC->getSerial32())

				{
					pTC->talk(s, TRANSLATE("Thank thee kindly, but I have done nothing to warrant a gift."),0);
					Sndbounce5(s);
					if (ps->isDragging())
					{
						ps->resetDragging();
						item_bounce5(s,pi);
					}
					return true;
				}
				else // The player is training from this NPC
				{
					ItemDroppedOnTrainer( ps, pp, pi);
				}
			}//if human or not
		}
		else // dropped on another player
		{
			// By Polygon: Avoid starting the trade if GM drops item on logged on char (crash fix)
			if ((pc_currchar->IsGM()) && !pTC->IsOnline())
			{
				// Drop the item in the players pack instead
				// Get the pack
				P_ITEM pack = pTC->getBackpack();
				if (pack != NULL)	// Valid pack?
				{
					pack->AddItem(pi);	// Add it
					weights::NewCalc(pTC);
				}
				else	// No pack, give it back to the GM
				{
					pack = pc_currchar->getBackpack();
					if (pack != NULL)	// Valid pack?
					{
						pack->AddItem(pi);	// Add it
						weights::NewCalc(pc_currchar);
					}
					else	// Even GM has no pack?
					{
						// Drop it to it's feet
						pi->MoveTo( charpos );
						pi->Refresh();
					}
				}
			}
			else
			{
                                //<Luxor>: secure trade
                 P_ITEM tradeCont = tradestart(pc_currchar, pTC);
                 if (ISVALIDPI(tradeCont)) {
                    tradeCont->AddItem( pi, 30, 30 );
                 } else {
                    Sndbounce5(s);
                    if (ps->isDragging()) {
                 		ps->resetDragging();
                 		UpdateStatusWindow(s,pi);
                   	}
                 }
                 //</Luxor>
		        }
	        }
	}
	else // dumping stuff to his own backpack !
	{
		ItemDroppedOnSelf( ps, pp, pi);
	}
	return true;
}

void dump_item(NXWCLIENT ps, PKGx08 *pp) // Item is dropped on ground or a character
{
	if (ps == NULL) return;

	tile_st tile;
	NXWSOCKET  s=ps->toInt();

	P_CHAR pc=ps->currChar();
	VALIDATEPC(pc);

	P_ITEM pi=pointers::findItemBySerial(pp->Iserial);


	if (!ISVALIDPI(pi))
	{
		LogError("client sent bad itemserial %d",pp->Iserial);
		return;
	}

	if ( isCharSerial(pi->getContainer()->getSerial()) && pi->getContainer() != pc ) {
		P_CHAR pc_i = (pChar) pi->getContainer();
		if (ISVALIDPC(pc_i))
			pc_i->sysmsg("Warning, backpack disappearing bug located!");

		if (ps->isDragging()) {
                        ps->resetDragging();
                        UpdateStatusWindow(s,pi);
                }
		pi->setContainer( pi->getOldContainer() );
                pi->setPosition( pi->getOldPosition() );
                pi->layer = pi->oldlayer;
                pi->Refresh();
	}

	if (pi->magic == 2) { //Luxor -- not movable objects
		if (ps->isDragging()) {
                        ps->resetDragging();
                        UpdateStatusWindow(s,pi);
                }
		pi->setContainer( pi->getOldContainer() );
		pi->MoveTo( pi->getOldPosition() );
		pi->layer = pi->oldlayer;
		pi->Refresh();
		return;
	}




        if(pi!=NULL)
	{
		weights::NewCalc(pc);
		statwindow(pc,pc);
	}


	//Ripper...so order/chaos shields disappear when on ground.
	if( pi->getId()==0x1BC3 || pi->getId()==0x1BC4 )
	{
		pc->playSFX( 0x01FE);
		staticeffect(DEREF_P_CHAR(pc), 0x37, 0x2A, 0x09, 0x06);
		pi->Delete();
		return;
	}


	//test UOP blocking Tauriel 1-12-99
	if (!pi->isInWorld())
	{
		item_bounce6(ps,pi);
		return;
	}




	data::seekTile(pi->getId(), tile);
	if (!pc->IsGM() && ((pi->magic==2 || (tile.weight==255 && pi->magic!=1))&&!pc->canAllMove()) ||
		( (pi->magic==3 || pi->magic==4) && !(pi->getOwnerSerial32()==pc->getSerial32())))
	{
		item_bounce6(ps,pi);
		return;
	}

	if (buffer[s][5]!=(unsigned char)'\xFF')
	{


        if (pi->amxevents[EVENT_IDROPINLAND]!=NULL) {
	       	g_bByPass = false;
        	pi->amxevents[EVENT_IDROPINLAND]->Call( pi->getSerial32(), pc->getSerial32() );
			if (g_bByPass) {
				pi->Refresh();
				return;
			}
		}

		/*
		//<Luxor>
		g_bByPass = false;
		pi->runAmxEvent( EVENT_IDROPINLAND, pi->getSerial32(), pc->getSerial32() );
	        if (g_bByPass) {
			pi->Refresh();
			return;
		}
        //</Luxor>
		*/
		NxwSocketWrapper sw;
		sw.fillOnline( pi );
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			SendDeleteObjectPkt( sw.getSocket(), pi->getSerial32() );
		}

		pi->MoveTo(pp->TxLoc,pp->TyLoc,pp->TzLoc);
		pi->setContainer(0);

		P_ITEM p_boat = Boats->GetBoat(pi->getPosition());

		if(ISVALIDPI(p_boat))
		{
			pi->SetMultiSerial(p_boat->getSerial32());
		}


		pi->Refresh();
	}
	else
	{
		if ( !ItemDroppedOnChar(ps, pp, pi) ) {
			//<Luxor>: Line of sight check
			//This part avoids the circle of transparency walls bug

			//-----
			if ( !lineOfSight( pc->getPosition(), Loc( pp->TxLoc, pp->TyLoc, pp->TzLoc ) ) ) {
		                ps->sysmsg(TRANSLATE("You cannot place an item there!"));

        	        	Sndbounce5(s);
	                	if (ps->isDragging()) {
	                        	ps->resetDragging();
                        		UpdateStatusWindow(s,pi);
                		}
                		pi->setContainer( pi->getOldContainer() );
                		pi->setPosition( pi->getOldPosition() );
                		pi->layer = pi->oldlayer;
                		pi->Refresh();
                		return;
        		}
        		//</Luxor>

	        	//<Luxor> Items count check
	        	if (!pc->IsGM()) {
				NxwItemWrapper si;
				si.fillItemsAtXY( pp->TxLoc, pp->TyLoc );
				if (si.size() >= 2) { //Only 2 items permitted
					ps->sysmsg(TRANSLATE("There is not enough space there!"));
					Sndbounce5(s);
					if (ps->isDragging()) {
						ps->resetDragging();
						UpdateStatusWindow(s,pi);
					}
					if (ISVALIDPI(pc->getBackpack())) {
						pc->getBackpack()->AddItem(pi);
					} else {
						pi->setContainer( pi->getOldContainer() );
						pi->setPosition( pi->getOldPosition() );
					}
					pi->layer = pi->oldlayer;
					pi->Refresh();
					return;
				}
			}
        		//</Luxor>
		}

		weights::NewCalc(pc);  // Ison 2-20-99
		statwindow(pc,pc);
		pc->playSFX( itemsfx(pi->getId()) );

		//Boats !
		if (pc->getMultiSerial32() > 0) //How can they put an item in a multi if they aren't in one themselves Cut lag by not checking everytime something is put down
		{
			P_ITEM multi = pointers::findItemBySerial( pc->getMultiSerial32() );
			if (ISVALIDPI(multi))
			{
				multi=findmulti( pi->getPosition() );
				if (ISVALIDPI(multi))
					//setserial(DEREF_P_ITEM(pi),DEREF_P_ITEM(multi),7);
					pi->SetMultiSerial(multi->getSerial32());
			}
		}
		//End Boats
	}
}


