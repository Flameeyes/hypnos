/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
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
#include "globals.h"
#include "basics.h"
#include "cmds.h"
#include "settings.h"

/*!
\brief single clicks over PC/NPCs
\param client client of player who clicks "this"
*/

void cChar::singleClick( pClient client )
{
        pPC clickedBy = client->currChar();
	if ( clickedBy )
	{
                if ( amxevents[EVENT_CHR_ONCLICK] != NULL )
	        {
		        g_bByPass = false;
		        amxevents[EVENT_CHR_ONCLICK]->Call( getSerial(), clickedBy->getSerial32() );
		        if ( g_bByPass==true )
			return;
	        }
                if ( nSettings::Server::shouldShowPCNames() || npc || getSerial() == clickedBy->getSerial32()) showLongName( clickedBy, false );
	}
	else
	{
		LogMessage("<%d> cChar::singleClick couldn't find char serial: %d\n", __LINE__, serial);
	}

}


/*!
\brief Double clicks over PC/NPCs
\param client client of player who dclicks "this"
\param keyboard ??? highest bit of first serial byte from packet
*/
void cChar::doubleClick(pClient client, int keyboard)
{
	if (client==NULL) return;
	pChar clicker = client->currChar();
	if ( ! clicker ) return;

	if( clicker->war && (clicker->getSerial()!=getSerial32()) )
		clicker->attackStuff(this);

	pItem pack = getBackpack();

	switch( getId() )
	{
		// Handle pack animals
		case	0x0123	:
		case	0x0124	:
			if ( npc )
			{
				if ( getOwnerSerial32() == clicker->getSerial() || clicker->IsGMorCounselor() )
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
					if ( clicker->checkSkill( SNOOPING, 0, 1000 ) )
					{
						if (pack )
						{
							clicker->showContainer(pack);
							clicker->sysmsg(TRANSLATE("You successfully snoop the pack animal.") );
							SetTimerSec( &(clicker->objectdelay), SrvParms->objectdelay+SrvParms->snoopdelay );
						}
						else
							WarnOut("Pack animal %i has no backpack!\n",getSerial());
					}
					else
					{
						clicker->sysmsg( TRANSLATE("You failed to snoop the pack animal.") );
						clicker->IncreaseKarma( - nSettings::Skills::getSnoopKarmaLoss() );
						clicker->modifyFame( - nSettings::Skills::getSnoopFameLoss() );
						//!\TODO should investigate
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
				talk(client,TRANSLATE("Take a look at my goods."),0);
				if ( pack)
                                        clicker->showContainer(pack);
			}
			else if ( clicker->getSerial() == getSerial32() )
			{//dbl-click self
				if ( (!keyboard) && ( clicker->unmountHorse() == 0 ) ) return; //on horse
				//if not on horse, treat ourselves as any other char
			}//self


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
						pc->sysmsg( TRANSLATE("You cannot ride anything under polymorph effect."));
					}
					else
						if ( clicker->dead) {
							pc->sysmsg(TRANSLATE("You are dead and cannot do that."));
						}
						else
							if (war) {
								clicker->sysmsg(TRANSLATE("Your pet is in battle right now!"));
							}
							else
								clicker->mounthorse( this);


				}
				else  {
					clicker->sysmsg( TRANSLATE("You need to get closer."));
				}
			}
		return;
	}

	clicker->sysmsg(TRANSLATE("You cannot open monsters paperdolls."));
}

