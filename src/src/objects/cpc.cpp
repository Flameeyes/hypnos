/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cPC class
*/

#include "objects/cpc.h"

void cPC::heartbeat()
{
	if ( dead )
		return;
	if ( Accounts->GetInWorld( account ) == getSerial32() && logout > 0 && ( logout <= (SI32)uiCurrentTime  ) )
	{
		Accounts->SetOffline( account);
		logout = INVALID;
		teleport( TELEFLAG_NONE );
		return;
	}
	if( !IsOnline() )
		return;

	if ( amxevents[EVENT_CHR_ONHEARTBEAT] )
	{
		g_bByPass = false;
		amxevents[EVENT_CHR_ONHEARTBEAT]->Call( getSerial32(), uiCurrentTime );
		if( g_bByPass == true ) return;
		if( dead )	// Killed as result of script action
			return;
	}
	/*
	g_bByPass = false;
	runAmxEvent( EVENT_CHR_ONHEARTBEAT, getSerial32(), uiCurrentTime );
	if( g_bByPass == true )
		return;
	*/
	if( dead )	// Killed as result of script action
		return;
	generic_heartbeat();
	if( dead )	// Killed as result of generic heartbeat action
		return;

	if ( SrvParms->hunger_system && TIMEOUT( hungertime ) && SrvParms->hungerrate > 1 )
	{
		if ( !IsGMorCounselor() && hunger )
		{
			--hunger;

			switch( hunger )
			{
				case 6:
				case 5: sysmsg( TRANSLATE("You are still stuffed from your last meal") );
					break;
				case 4: sysmsg( TRANSLATE("You are not very hungry but could eat more") );
					break;
				case 3: sysmsg( TRANSLATE("You are feeling fairly hungry") );
					break;
				case 2: sysmsg( TRANSLATE("You are extremely hungry") );
					break;
				case 1: sysmsg( TRANSLATE("You are very weak from starvation") );
					break;
				case 0:	sysmsg( TRANSLATE("You must eat very soon or you will die!") );
					break;
			}
			hungertime = uiCurrentTime+(SrvParms->hungerrate*MY_CLOCKS_PER_SEC); // Bookmark
		}
	}
	if ( SrvParms->hunger_system && TIMEOUT( hungerdamagetimer ) && SrvParms->hungerdamage > 0 ) // Damage them if they are very hungry
	{
		hungerdamagetimer=uiCurrentTime+(SrvParms->hungerdamagerate*MY_CLOCKS_PER_SEC); /** set new hungertime **/
		if (hp > 0 && hunger<2 && !IsCounselor() && !dead)
		{
			sysmsg( TRANSLATE("You are starving !") );
			hp -= SrvParms->hungerdamage;
			updateStats(0);
			if(hp<=0)
			{
				Kill();
				sysmsg(TRANSLATE("You have died of starvation"));
			}
		}
	}
	if( dead )	// Starved to death
		return;

	checkFieldEffects( uiCurrentTime, this, 1 );
	if( dead )
		return;

	checkPoisoning();
	if( dead )	// Poison took it's toll
		return;

	int timer;

	NXWCLIENT ps = getClient();
	NXWSOCKET socket = getSocket();

	if     ( swingtargserial == INVALID )
		doCombat();
	else //if( TIMEOUT( timeout ) )
		combatHit( pointers::findCharBySerial( swingtargserial ) );


	if ( !TIMEOUT( smoketimer ) )
	{
		if ( TIMEOUT( smokedisplaytimer ) )
		{
			smokedisplaytimer = uiCurrentTime + 5 * MY_CLOCKS_PER_SEC;
			staticeffect( DEREF_P_CHAR( this ), 0x37, 0x35, 0, 30 );
			playSFX( 0x002B );
			switch( RandomNum( 0, 6 ) )
			{
			 case 0:	emote(socket,TRANSLATE("*Drags in deep*") ,1);		break;
			 case 1:	emote(socket,TRANSLATE("*Coughs*"),1);				break;
			 case 2:	emote(socket,TRANSLATE("*Retches*"),1);				break;
			 case 3:	emote(socket,TRANSLATE("*Hacking cough*"),1);		break;
			 case 4:	emote(socket,TRANSLATE("*Sighs in contentment*"),1 );break;
			 case 5:	emote(socket,TRANSLATE("*Puff puff*") ,1);			break;
			 case 6:	emote(socket,TRANSLATE("Wheeeee!!! Smoking!"),1);	break;
			 default:	break;
			}
		}
	}

	if ( clientInfo[socket]->lsd )
		do_lsd(); //LB's LSD potion-stuff

	if ( TIMEOUT( mutetime ) && squelched == 2 )
	{
		squelched = 0;
		mutetime  = 0;
		sysmsg( TRANSLATE("You are no longer squelched!") );
	}

/*	if ( IsCriminal() && ( crimflag <= uiCurrentTime  ) )
	{
		sysmsg( TRANSLATE("You are no longer a criminal.") );
		crimflag = 0;
		SetInnocent();
	}*/ //Luxor: now criminal flag is handled by CRIMINAL tempfx

	if ( TIMEOUT( murderrate ) )
	{
		if ( kills > 0)
			--kills;
		if ( kills == repsys.maxkills && repsys.maxkills > 0 )
		{
			sysmsg( TRANSLATE( "You are no longer a murderer." ) );
			SetInnocent();
		}
		murderrate = ( repsys.murderdecay * MY_CLOCKS_PER_SEC ) + uiCurrentTime;
	}

	updateFlag();

	if ( flags & flagIsCasting )
	{
		if ( TIMEOUT( spelltime ) )//Spell is complete target it.
		{
			if ( magic::spellRequiresTarget( spell ) ) {
				P_TARGET targ = clientInfo[socket]->newTarget( new cTarget() );
				targ->code_callback = target_castSpell;
				targ->buffer[0]=spell;
				targ->send( ps );
				ps->sysmsg( TRANSLATE("Select your target") );
			}
			else
			{
		    		TargetLocation TL( this );
				magic::castSpell( spell, TL, this );
			}
			flags &= ~flagIsCasting;
			spelltime = 0;
		}
		else if ( TIMEOUT( nextact ) ) //redo the spell action
		{ //<Luxor>
			nextact = uiCurrentTime + UI32(MY_CLOCKS_PER_SEC*1.5);
			if ( isMounting() )
				playAction( 0x1b );
			else
				playAction( spellaction );
		//</Luxor>
		}
	}

	if( SrvParms->bg_sounds >= 1 )
	{
		if( SrvParms->bg_sounds > 10 )
			SrvParms->bg_sounds = 10;
		timer = SrvParms->bg_sounds * 100;
		if ( !timer ) ++timer;
		if( rand() % timer == timer / 2 )
			bgsound( DEREF_P_CHAR(this) );
	}

	if( TIMEOUT( spiritspeaktimer ) && spiritspeaktimer > 0 )
		spiritspeaktimer = 0;


	if( onhorse )
	{
		P_ITEM pHorse = GetItemOnLayer(LAYER_MOUNT);
		if(!pHorse)
			onhorse = false;	// turn it off, we aren't on one because there's no item!
		else
			if( pHorse->canDecay() && pHorse->getDecayTime() != 0 && TIMEOUT( pHorse->getDecayTime() ) )
			{
				onhorse = false;
				pHorse->Delete();
			}
	}

	if( GetFame() < 0 )
		SetFame(0);
}

/*!
\brief PC-related implementation of the cChar::updateFlag() method
\todo Check the comments inside it
*/
bool cPC::updateFlag()
{
	bool flagHasChanged = false;
	if ( kills >= (unsigned) repsys.maxkills )
	{
		if( !IsMurderer() )
			flagHasChanged = true;
		//
		//! \todo - TODO check out logic of next 2 statements (Sparhawk)
		//
		SetMurderer();
		murderrate = (repsys.murderdecay*MY_CLOCKS_PER_SEC)+uiCurrentTime;
	}
	/*else

		if ( crimflag == 0 )
		{
			if( !IsInnocent() )
				flagHasChanged = true;
			SetInnocent();
		}
		else
		{
			if( !IsCriminal() )
				flagHasChanged = true;
			SetCriminal();
		}
		*/
	return flagHasChanged;
}

/*!
\brief Send the update skill packet to the client
\param skill Skill to update
\author Flameeyes
*/
void cChar::updateSkill(UI16 skill)
{
	if ( ! client )
		return;

	cPacketSendUpdateSkill pk(this, skill);

	client->sendPackage(&pk);
}
