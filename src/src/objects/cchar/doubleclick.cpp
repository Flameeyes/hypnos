/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

/*!
\file
\brief Character click & doubleclick (mount/unmount and paperdolls)
*/

#include "ai.h"
#include "objects/cobject.h"
#include "magic.h"
#include "npcs.h"
#include "msgboard.h"
#include "target.h"
#include "constants.h"
#include "menu.h"
#include "logsystem.h"
#include "basics.h"
#include "settings.h"

/*!
\brief single clicks over PC/NPCs
\param client client of player who clicks "this"
*/
void cChar::singleClick( pClient client )
{
        pPC clickedBy = client->currChar();
	if ( ! clickedBy )
	{
		LogMessage("cChar::singleClick couldn't find char serial: %d\n", serial);
		return;
	}
	
	if ( events[evtChrOnClick] ) {
		tVariantVector params = tVariantVector(2);
		params[0] = getSerial(); params[1] = clickedBy->getSerial();
		events[evtChrOnClick]->setParams(params);
		events[evtChrOnClick]->execute();
		if ( events[evtChrOnClick]->isBypassed() )
			return;
	}
	
	if ( nSettings::Server::shouldShowPCNames() || npc || clickedBy == this )
		showLongName( clickedBy, false );
}


/*!
\brief Double clicks over PC/NPCs
\param client client of player who dclicks "this"
\param keyboard ??? highest bit of first serial byte from packet
*/
void cChar::doubleClick(pClient client)
{
	if (client==NULL) return;
	pChar clicker = client->currChar();
	if ( ! clicker ) return;

	if( clicker->war && (clicker->getSerial()!=getSerial()) )
		clicker->attackStuff(this);

	pItem pack = getBackpack();

	switch( getId() )
	{
		// Handle pack animals
		case	0x0123	:
		case	0x0124	:
			if ( npc )
			{
				if ( getOwner() == clicker || clicker->IsGMorCounselor() )
				{
					if (pack) {
						clicker->showContainer(pack);
						SetTimerSec( &(clicker->objectdelay), SrvParms->objectdelay );
					}
					else
						WarnOut("Pack animal %i has no backpack!\n",getSerial());
				}
				else
				{
					if ( clicker->checkSkill( skSnooping, 0, 1000 ) )
					{
						if (pack )
						{
							clicker->showContainer(pack);
							clicker->sysmsg("You successfully snoop the pack animal.");
							SetTimerSec( &(clicker->objectdelay), SrvParms->objectdelay+SrvParms->snoopdelay );
						}
						else
							WarnOut("Pack animal %i has no backpack!\n",getSerial());
					}
					else
					{
						clicker->sysmsg("You failed to snoop the pack animal.");
						clicker->IncreaseKarma( - nSettings::Skills::getSnoopKarmaLoss() );
						clicker->modifyFame( - nSettings::Skills::getSnoopFameLoss() );
						//!\todo should investigate
						clicker->setCrimGrey(ServerScp::g_nSnoopWillCriminal);
						SetTimerSec( &(clicker->objectdelay), SrvParms->objectdelay+SrvParms->snoopdelay );
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
			if (npc && npcaitype==NPCAI_PLAYERVENDOR)//PlayerVendors
			{
				talk(client,"Take a look at my goods.",0);
				if ( pack)
                                        clicker->showContainer(pack);
			}
			else if ( clicker == this )
				if ( unmountHorse() ) return; //on horse
				//if not on horse, treat ourselves as any other char


                        //! \todo when paperdoll send packet done, rewrite this

			uint8_t pdoll[66] = { 0x88, 0x00, 0x05, 0xA8, 0x90, 0x00, };

			LongToCharPtr(getSerial(), pdoll +1);

			strncpy((char*)&pdoll[5], getCompleteTitle().c_str(), 60);
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
	std::map<int32_t,int32_t>::iterator iter = mountinfo.find(getId());
	if( iter!=mountinfo.end() ) {
		if ( npc )	// riding a morphed player char is not allowed
			{
				if (clicker->distFrom(this)<2 || clicker->IsGMorCounselor())
				{
					//cannot ride animals under polymorph effect
					if ( clicker->polymorph) {
						pc->sysmsg("You cannot ride anything under polymorph effect.");
					}
					else
						if ( clicker->dead) {
							pc->sysmsg("You are dead and cannot do that.");
						}
						else
							if (war) {
								clicker->sysmsg("Your pet is in battle right now!");
							}
							else
								clicker->mounthorse( this);
				}
				else  {
					clicker->sysmsg("You need to get closer.");
				}
			}
		return;
	}

	clicker->sysmsg("You cannot open monsters paperdolls.");
}
