/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file timing.cpp
\brief functions that handle the timer controlled stuff
\author Duke
\note not necessarily ALL those functions
*/
#include "common_libs.hpp"
#include "sregions.hpp"
#include "sndpkg.hpp"
#include "magic.hpp"
#include "tmpeff.hpp"
#include "timers.hpp"
#include "spawn.hpp"
#include "trade.hpp"
#include "inlines.hpp"
#include "networking/cclient.hpp"
#include "objects/cchar.hpp"
#include "objects/citem/chouse.hpp"

extern bool g_bMustExecAICode;

static int32_t linInterpolation (int32_t ix1, int32_t iy1, int32_t ix2, int32_t iy2, int32_t ix);

void checkFieldEffects( uint32_t currenttime, pChar pc, char timecheck )
{

	if ( ! pc ) return;

	if ( (timecheck && !(nextfieldeffecttime<=currenttime)) ) //changed by Luxor
		return;
	
	for( pc->nearbyItems->begin(); itemIt != pc->nearbyItems->end(); ++itemIt ) {

		pItem pi= (*itemIt);
	
		NxwItemWrapper si;
		si.fillItemsNearXYZ( pc->getPosition(), 2, false );
		for( si.rewind(); !si.isEmpty(); si++ )
		{
			pItem pi=si.getItem();
			if ( !pi || pi->getPosition().x != pc->getPosition().x || pi->getPosition().y != pc->getPosition().y )
				continue;
			
			//Luxor: added new field damage handling
			switch( pi->getId() )
			{
				case 0x3996:
				case 0x398C: //Fire Field
					if (!pc->resistsFire())
						tempfx::add(pc, pc, tempfx::tmpfxFieldDamage, int32_t(pi->morex/100.0), damFire, 0, 1);
					return;
				case 0x3915:
				case 0x3920: //Poison Field
					if ((pi->morex<997)) {
						tempfx::add(pc, pc, tempfx::tmpfxFieldDamage, 2, damPoison, 0, 2);
						pc->applyPoison(poisonWeak);
					} else {
						tempfx::add(pc, pc, tempfx::tmpfxFieldDamage, 3, damPoison, 0, 2); // gm mages can cast greater poison field, LB
						pc->applyPoison(poisonNormal);
					}
					return;
				case 0x3979:
				case 0x3967: //Para Field
					if (chance(50)) {
						tempfx::add(pc, pc, tempfx::tmpfxSpellParalyze, 0, 0, 0, 3);
						pc->playSFX( 0x0204 );
					}
					return;
			}
		}
	
	}
}

void checktimers() // Check shutdown timers
{

	overflow = (lclock > getClockmSecs());
	if (endtime)
	{
		if ( endtime <= getClockmSecs() ) keeprun=false;
	}
	lclock = getClockmSecs();

}

void checkauto() // Check automatic/timer controlled stuff (Like fighting and regeneration)
{
//	static uint32_t checkspawnregions=0;
       	static uint32_t checktempfx=0;
	static uint32_t checknpcs=0;
	static uint32_t checktamednpcs=0;
	static uint32_t checknpcfollow=0;
	static uint32_t checkitemstime=0;
	static uint32_t lighttime=0;
	static uint32_t housedecaytimer=0;

	bool lightChanged = false;

	//
	// Accounts
	//
	if (SrvParms->auto_a_reload > 0 && TIMEOUT( Accounts->lasttimecheck + (SrvParms->auto_a_reload*60*SECS) ) )
		Accounts->CheckAccountFile();
	//
	// Weather (change is handled by crontab)
	//
	// Calendar
	//
	if ( TIMEOUT( uotickcount ) )
	{
		if (Calendar::advanceMinute())
			day++;
		uotickcount=getClockmSecs()+secondsperuominute*SECS;
		if (Calendar::g_nMinute%8==0)
			moon1=(moon1+1)%8;
		if (Calendar::g_nMinute%3==0)
			moon2=(moon2+1)%8;
	}
	//
	// Light
	//
	if( TIMEOUT( lighttime ) )
	{
		int32_t lightLevel = worldcurlevel;

		int32_t timenow = (Calendar::g_nHour * 60) + Calendar::g_nMinute;
		int32_t dawntime = (Calendar::g_nCurDawnHour * 60) + Calendar::g_nCurDawnMin;
		int32_t sunsettime = (Calendar::g_nCurSunsetHour * 60) + Calendar::g_nCurSunsetMin;
		int32_t nighttime = qmin((sunsettime+120), (1439));
		int32_t morntime = qmax((dawntime-120), (0));
		int32_t const middaytime = 750;
//		int32_t const midnighttime = 0; // unused variable
		int32_t dawnlight = (((worlddarklevel - worldbrightlevel))/3) + worldbrightlevel;
		//
		// default lights at dawn and sunset
		//
		if ( timenow == dawntime || timenow==sunsettime )
			lightLevel = dawnlight;
		//
		// highest light at midday
		//
		else if( timenow == middaytime )
			lightLevel = qmax(worldbrightlevel-1, 0);
		//
		// darkest light during night
		//
		else if( timenow >= nighttime )
			lightLevel = worlddarklevel;
		//
		else if( timenow <= morntime )
			lightLevel = worlddarklevel;
		//
		// fading light slight before dawn
		//
		else if( timenow > morntime && timenow < dawntime )
			lightLevel = linInterpolation(morntime, worlddarklevel, dawntime, dawnlight, timenow);
		//
		// fading light slight from dawn to midday
		else if( timenow > dawntime &&  timenow < middaytime )
			lightLevel = linInterpolation(dawntime, dawnlight, middaytime, worldbrightlevel, timenow);
		//
		// fading light slight from midday to sunset
		//
		else if( timenow > middaytime && timenow < sunsettime )
			lightLevel = linInterpolation(middaytime, worldbrightlevel, sunsettime, dawnlight, timenow);
		//
		// fading light slight from sunset to night
		//
		else if( timenow > sunsettime && timenow < nighttime )
			lightLevel = linInterpolation(sunsettime, dawnlight, nighttime, worlddarklevel, timenow);

		if (wtype)
			lightLevel += 2;
		if (moon1+moon2<4)
			++lightLevel;
		if (moon1+moon2<10)
			++lightLevel;

		if (lightLevel != worldcurlevel)
		{
			worldcurlevel = lightLevel;
			lightChanged  = true;
		}
		lighttime=getClockmSecs()+secondsperuominute*5*SECS;
	}

	//
	//	Housedecay and stabling
	//
	if ( TIMEOUT( housedecaytimer ) )
	{
		// check_houses
		if( SrvParms->housedecay_secs != UINVALID )
			cHouse::checkDecay();
		housedecaytimer = getClockmSecs()+HOURS; // check only each hour
	}
	//
	// Spawns
	//
	if( TIMEOUT( Spawns->check ) )
	{
		Spawns->doSpawn();
	}

	//
	// Shoprestock
	//
	Restocks->doRestock();

	//
	// Prison release
	//
	prison::checkForFree();

	//
	// Temporary effects
	//
        if( TIMEOUT( checktempfx ) )
		tempfx::checktempeffects();

	//
	// Characters & items
	//
	NxwSocketWrapper sw;
	sw.fillOnline();

	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient ps = sw.getClient();
		if( ps == NULL )
			continue;

		pChar pc=ps->currChar();
		if( !pc )
			continue;

		if( lightChanged )
			dolight(ps->toInt(),worldcurlevel);

		pc->heartbeat();

		if( TIMEOUT( checknpcs ) || TIMEOUT( checktamednpcs ) || TIMEOUT( checknpcfollow ) )
		{
			CharList *pCV = pointers::getNearbyChars( pc, VISRANGE, pointers::NPC );
			CharList it( pCV->begin() ), end( pCV->end() );
			pChar pNpc = 0;
			while( it != end )
			{
				pNpc = (*it);
				if( pNpc->lastNpcCheck != getClockmSecs() &&
				    (TIMEOUT( checknpcs ) ||
				    (TIMEOUT( checktamednpcs ) && pNpc->tamed) ||
				    (TIMEOUT( checknpcfollow ) && pNpc->npcWander == cNPC::WANDER_FOLLOW ) ) )
				{
					pNpc->heartbeat();
					pNpc->lastNpcCheck = getClockmSecs();
				}
				++it;
			}
		}

		if( TIMEOUT( checkitemstime ) )
		{
			NxwItemWrapper si;
			si.fillItemsNearXYZ( pc->getPosition(), 2*VISRANGE, false );
			for( si.rewind(); !si.isEmpty(); si++ )
			{
				pItem pi=si.getItem();

				if( !pi )
					continue;

				pi->doDecay();

				switch( pi->type )
				{
					case  51	:
					case  52	:
						//if( TIMEOUT( pi->gatetime ) )
							//for (int k=0;k<2;++k)	Sparhawk what's this???? Let's comment it out for now
							//	pi->deleteItem(); // bugfix for items disappearing
							//pi->deleteItem();
						break;
					case  61    :
					case  62	:
					case  63	:
					case  64	:
					case  65	:
					case  69	:
					case 125	:
						break; //SPAWNERS may not decay!!! --> Sparhawk then don't use the decay tag in the script
					case  88	:
						if( pi->morey >= 0 && pi->morey < 25 )
							if (pc->distFrom(pi)<=pi->morey)
								if( (uint32_t)RandomNum(1,100) <= pi->morez )
									pi->playSFX(ps, pi->morex);
						break;
					case 117	:	// Boats
						if( pi->type2 == 1 || pi->type2 == 2 )
							if( TIMEOUT( pi->gatetime ) )
							{
								if (pi->type2==1)
									Boats->Move(ps,pi->dir,pi);
								else
								{
									int dir=pi->dir+4;
									dir%=8;
									Boats->Move(ps,dir,pi);
								}
								pi->gatetime=(uint32_t)(getClockmSecs() + (double)(SrvParms->boatspeed*SECS));
							}
						break;
				}
			}
		}
	}//for i<now


	if( TIMEOUT( checkitemstime ) )
		checkitemstime = (uint32_t)((double) getClockmSecs()+(speed.itemtime*SECS));
	if( TIMEOUT( checknpcs ) )
		checknpcs = (uint32_t)((double) getClockmSecs()+(speed.npctime*SECS));
	if( TIMEOUT( checktamednpcs ) )
		checktamednpcs=(uint32_t)((double) getClockmSecs()+(speed.tamednpctime*SECS));
	if( TIMEOUT( checknpcfollow ) )
		checknpcfollow=(uint32_t)((double) getClockmSecs()+(speed.npcfollowtime*SECS));
	//
	// Finish
	//
	if ( TIMEOUT( nextfieldeffecttime ) )
		nextfieldeffecttime = (uint32_t)((double) getClockmSecs() + (0.5*SECS));
	if ( TIMEOUT( nextdecaytime ) )
		nextdecaytime = getClockmSecs() + (15*SECS);
        if( TIMEOUT( checktempfx ) )
		checktempfx = (uint32_t)((double) getClockmSecs()+(0.5*SECS));
}

static int32_t linInterpolation (int32_t ix1, int32_t iy1, int32_t ix2, int32_t iy2, int32_t ix)
{
	float X2 = ix2 - ix1;
	float Y2 = iy2 - iy1;
	float  X =  ix - ix1;
	float  Y = (X/X2)*Y2)+iy1;

	return static_cast<int32_t>(Y);
}
