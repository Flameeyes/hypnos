/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "nxwcommn.h"
#include "network.h"
#include "itemid.h"
#include "sndpkg.h"
#include "srvparms.h"
#include "debug.h"
#include "amx/amxcback.h"
#include "trigger.h"
#include "magic.h"
#include "house.h"
#include "npcai.h"
#include "layer.h"
#include "mount.h"
#include "nxw_utils.h"
#include "data.h"
#include "boats.h"
#include "books.h"
#include "set.h"
#include "dbl_single_click.h"
#include "titles.h"
#include "rcvpkg.h"
#include "map.h"
#include "items.h"
#include "chars.h"
#include "basics.h"
#include "inlines.h"
#include "skills.h"
#include "classes.h"
#include "range.h"
#include "scp_parser.h"
#include "nox-wizard.h"
#include "utils.h"
#include "fishing.h"

/*!
\brief apply wear out to item, delete if necessary
\author Ripper
\return bool
\param pi pointer to item to wear out
\note only used on Item_ToolWearOut(NXWSOCKET  s, P_ITEM pi)
\todo merge whit Item_ToolWearOut(NXWSOCKET  s, P_ITEM pi)
*/
/*static bool Item_ToolWearOut(P_ITEM pi)
{
	if(chance(5))
		pi->hp--; //Take off a hit point
	if(pi->hp<=0)
	{
		pi->deleteItem();
		return true;
	}
	else
		return false;
}*/

/*!
\brief apply wear out to item, delete if necessary
\author Ripper, rewritten by Luxor
\return bool
\param s socket of player who wear out the item
\param pi pointer to item to wear out
\todo become a method of cItem
*/
static bool Item_ToolWearOut(NXWSOCKET  s, P_ITEM pi)
{
	VALIDATEPIR(pi, false);
	if ( s < 0 || s >= now )
		return false;

	P_CHAR pc = pointers::findCharBySerial(currchar[s]);
	VALIDATEPCR(pc, false);
	if( chance(5) ) { // has item been destroyed ??
		pi->hp--;
		if ( pi->hp <= 0 ) {
			pc->sysmsg("Your %s has been destroyed", pi->getCurrentNameC());
			pi->Delete();
			return true;
		}
	}
	return false;
}



/*!
\brief Double clicks over PC/NPCs
\param ps clientof player who click
\param target pointer to the pc/npc clicked
*/
void dbl_click_character(NXWCLIENT ps, P_CHAR target)
{
	if(ps==NULL) return;
	P_CHAR pc=ps->currChar();
	VALIDATEPC(pc);
	VALIDATEPC(target);
	NXWSOCKET s = ps->toInt();

	if( pc->war && (pc->getSerial32()!=target->getSerial32()) )
		pc->attackStuff(target);

	int keyboard = buffer[s][1]&0x80;

	P_ITEM pack	= target->getBackpack();


	switch( target->getId() )
	{
		// Handle pack animals
		case	0x0123	:
		case	0x0124	:
			if ( target->npc )
			{

				if ( target->getOwnerSerial32() == pc->getSerial32() || pc->IsGMorCounselor() )
				{
					if (ISVALIDPI(pack)) {
						pc->showContainer(pack);
						SetTimerSec( &(pc->objectdelay), SrvParms->objectdelay );
					}
					else
						WarnOut("Pack animal %i has no backpack!\n",target->getSerial32());
				}
				else
				{
					if ( pc->checkSkill( SNOOPING, 0, 1000 ) )
					{
						if (ISVALIDPI(pack) )
						{
							pc->showContainer(pack);
							pc->sysmsg(TRANSLATE("You successfully snoop the pack animal.") );
							SetTimerSec( &(pc->objectdelay), SrvParms->objectdelay+SrvParms->snoopdelay );
						}
						else
							WarnOut("Pack animal %i has no backpack!\n",target->getSerial32());
					}
					else
					{
						pc->sysmsg( TRANSLATE("You failed to snoop the pack animal.") );
						pc->IncreaseKarma( ServerScp::g_nSnoopKarmaLoss  );
						pc->modifyFame( ServerScp::g_nSnoopFameLoss );
						pc->setCrimGrey(ServerScp::g_nSnoopWillCriminal);
						SetTimerSec( &(pc->objectdelay), SrvParms->objectdelay+SrvParms->snoopdelay );
					}
				}
			}
			return;
		//
		// Handle humanoids
		//
		case BODY_MALE		:
		case BODY_FEMALE	:
		case BODY_DEADMALE	:
		case BODY_DEADFEMALE	:
		case BODY_GMSTAFF	:
			if (target->npc && target->npcaitype==NPCAI_PLAYERVENDOR)//PlayerVendors
			{
				target->talk(s,TRANSLATE("Take a look at my goods."),0);
				if ( ISVALIDPI(pack))
					pc->showContainer(pack);
			}
			else if ( pc->getSerial32() == target->getSerial32() )
			{//dbl-click self
				if ( (!keyboard) && ( pc->unmountHorse() == 0 ) ) return; //on horse
				//if not on horse, treat ourselves as any other char
			}//self

			UI08 pdoll[66] = { 0x88, 0x00, 0x05, 0xA8, 0x90, 0x00, };

			LongToCharPtr(target->getSerial32(), pdoll +1);

			completetitle = complete_title(target);
			if ( strlen(completetitle) >= 60 )
				completetitle[60]=0;
			strcpy((char*)&pdoll[5], completetitle);
			Xsend(s, pdoll, 66);
//AoS/			Network->FlushBuffer(s);
			return;
		//
		// Handle others
		//
	}
	/*	// Handle mountable npcs
		case 0x00c8	:
		case 0x00e2	:
		case 0x00e4	:
		case 0x00cc	:
		case 0x00dc	:
		case 0x00d2	:
		case 0x00da	:
		case 0x00db	:
		case 0x007a	:	// LBR	unicorn
		case 0x00bb	:	// 	ridgeback
		case 0x0317	:	//	giant beetle
		case 0x0319	:	//	skeletal mount
		case 0x031a	:	//	swamp dragon
		case 0x031f	:	//	armor dragon*/
	std::map<SI32,SI32>::iterator iter = mountinfo.find(target->getId());
	if( iter!=mountinfo.end() ) {
		if ( target->npc )	// riding a morphed player char is not allowed
			{
				if (pc->distFrom(target)<2 || pc->IsGMorCounselor())
				{
					//cannot ride animals under polymorph effect
					if ( pc->polymorph) {
						pc->sysmsg( TRANSLATE("You cannot ride anything under polymorph effect."));
					}
					else
						if ( pc->dead) {
							pc->sysmsg(TRANSLATE("You are dead and cannot do that."));
						}
						else
							if (target->war) {
								pc->sysmsg(TRANSLATE("Your pet is in battle right now!"));
							}
							else

								pc->mounthorse( target);


				}
				else  {
					pc->sysmsg( TRANSLATE("You need to get closer."));
				}
			}
		return;
	}

	pc->sysmsg(TRANSLATE("You cannot open monsters paperdolls."));
}
