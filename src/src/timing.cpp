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

#include "common_libs.h"
#include "basics.h"
#include "sregions.h"
#include "sndpkg.h"
#include "calendar.h"
#include "magic.h"
#include "race.h"
#include "tmpeff.h"
#include "debug.h"
#include "house.h"
#include "jail.h"
#include "timers.h"
#include "boats.h"
#include "spawn.h"
#include "trade.h"
#include "inlines.h"

extern bool g_bMustExecAICode;

static int32_t linInterpolation (int32_t ix1, int32_t iy1, int32_t ix2, int32_t iy2, int32_t ix);

void checkFieldEffects( uint32_t currenttime, pChar pc, char timecheck )
{

	if ( ! pc ) return;

	if ( (timecheck && !(nextfieldeffecttime<=currenttime)) ) //changed by Luxor
		return;
#ifdef SPAR_NEW_WR_SYSTEM
	pItemVectorIt itemIt( pc->nearbyItems->begin() ), itemEnd( pc->nearbyItems->end() );

	for( ; itemIt != itemEnd; ++itemIt ) {

		pItem pi= (*itemIt);
#else
	NxwItemWrapper si;
	si.fillItemsNearXYZ( pc->getPosition(), 2, false );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi=si.getItem();
#endif
		if(pi ) {

			if ( pi->getPosition().x == pc->getPosition().x && pi->getPosition().y == pc->getPosition().y )

				//Luxor: added new field damage handling
				switch( pi->getId() )
				{
					case 0x3996:
					case 0x398C: //Fire Field
						if (!pc->resistsFire())
							tempfx::add(pc, pc, tempfx::FIELD_DAMAGE, int32_t(pi->morex/100.0), damFire, 0, 1);
						return;
					case 0x3915:
					case 0x3920: //Poison Field
						if ((pi->morex<997)) {
							tempfx::add(pc, pc, tempfx::FIELD_DAMAGE, 2, damPoison, 0, 2);
							pc->applyPoison(poisonWeak);
						} else {
							tempfx::add(pc, pc, tempfx::FIELD_DAMAGE, 3, damPoison, 0, 2); // gm mages can cast greater poison field, LB
							pc->applyPoison(poisonNormal);
						}
						return;
					case 0x3979:
					case 0x3967: //Para Field
						if (chance(50)) {
							tempfx::add(pc, pc, tempfx::SPELL_PARALYZE, 0, 0, 0, 3);
							pc->playSFX( 0x0204 );
						}
						return;
				}
		}
	}
}

void checktimers() // Check shutdown timers
{

	overflow = (lclock > getclock());
	if (endtime)
	{
		if ( endtime <= getclock() ) keeprun=false;
	}
	lclock = getclock();

}

void checkauto() // Check automatic/timer controlled stuff (Like fighting and regeneration)
{
//	static TIMERVAL checkspawnregions=0;
       	static TIMERVAL checktempfx=0;
	static TIMERVAL checknpcs=0;
	static TIMERVAL checktamednpcs=0;
	static TIMERVAL checknpcfollow=0;
	static TIMERVAL checkitemstime=0;
	static TIMERVAL lighttime=0;
	static TIMERVAL housedecaytimer=0;

	bool lightChanged = false;

	//
	// Accounts
	//
	if (SrvParms->auto_a_reload > 0 && TIMEOUT( Accounts->lasttimecheck + (SrvParms->auto_a_reload*60*MY_CLOCKS_PER_SEC) ) )
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
		uotickcount=getclock()+secondsperuominute*MY_CLOCKS_PER_SEC;
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
		lighttime=getclock()+secondsperuominute*5*MY_CLOCKS_PER_SEC;
	}

	//
	//	Housedecay and stabling
	//
	if ( TIMEOUT( housedecaytimer ) )
	{
		//////////////////////
		///// check_houses
		/////////////////////
		if( SrvParms->housedecay_secs != UINVALID )
			check_house_decay();
		housedecaytimer = getclock()+MY_CLOCKS_PER_SEC*60*60; // check only each hour
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
#ifdef SPAR_C_LOCATION_MAP
			CharList *pCV = pointers::getNearbyChars( pc, VISRANGE, pointers::NPC );
			CharList it( pCV->begin() ), end( pCV->end() );
			pChar pNpc = 0;
			while( it != end )
			{
				pNpc = (*it);
				if( pNpc->lastNpcCheck != getclock() &&
				    (TIMEOUT( checknpcs ) ||
				    (TIMEOUT( checktamednpcs ) && pNpc->tamed) ||
				    (TIMEOUT( checknpcfollow ) && pNpc->npcWander == WANDER_FOLLOW ) ) )
				{
					pNpc->heartbeat();
					pNpc->lastNpcCheck = getclock();
				}
				++it;
			}
#else
			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( pc->getPosition(), VISRANGE, true, false );
			for( sc.rewind(); !sc.isEmpty(); sc++ )
			{
				pChar npc=sc.getChar();

				if(! npc || !npc->npc )
					continue;

				if( npc->lastNpcCheck != getclock() &&
				    (TIMEOUT( checknpcs ) ||
				    (TIMEOUT( checktamednpcs ) && npc->tamed) ||
				    (TIMEOUT( checknpcfollow ) && npc->npcWander == WANDER_FOLLOW ) ) )
				{
					npc->heartbeat();
					npc->lastNpcCheck = getclock();
				}
			}
#endif
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
									soundeffect4(ps->toInt(), pi, pi->morex);
						break;
					case 117	:	// Boats
						if( pi->type2 == 1 || pi->type2 == 2 )
							if( TIMEOUT( pi->gatetime ) )
							{
								if (pi->type2==1)
									Boats->Move(ps->toInt(),pi->dir,pi);
								else
								{
									int dir=pi->dir+4;
									dir%=8;
									Boats->Move(ps->toInt(),dir,pi);
								}
								pi->gatetime=(TIMERVAL)(getclock() + (double)(SrvParms->boatspeed*MY_CLOCKS_PER_SEC));
							}
						break;
				}
			}
		}
	}//for i<now


	if( TIMEOUT( checkitemstime ) )
		checkitemstime = (TIMERVAL)((double) getclock()+(speed.itemtime*MY_CLOCKS_PER_SEC));
	if( TIMEOUT( checknpcs ) )
		checknpcs = (TIMERVAL)((double) getclock()+(speed.npctime*MY_CLOCKS_PER_SEC));
	if( TIMEOUT( checktamednpcs ) )
		checktamednpcs=(TIMERVAL)((double) getclock()+(speed.tamednpctime*MY_CLOCKS_PER_SEC));
	if( TIMEOUT( checknpcfollow ) )
		checknpcfollow=(TIMERVAL)((double) getclock()+(speed.npcfollowtime*MY_CLOCKS_PER_SEC));
	//
	// Finish
	//
	if ( TIMEOUT( nextfieldeffecttime ) )
		nextfieldeffecttime = (TIMERVAL)((double) getclock() + (0.5*MY_CLOCKS_PER_SEC));
	if ( TIMEOUT( nextdecaytime ) )
		nextdecaytime = getclock() + (15*MY_CLOCKS_PER_SEC);
        if( TIMEOUT( checktempfx ) )
		checktempfx = (TIMERVAL)((double) getclock()+(0.5*MY_CLOCKS_PER_SEC));
}

static int32_t linInterpolation (int32_t ix1, int32_t iy1, int32_t ix2, int32_t iy2, int32_t ix)
{
	#define NSIN(X) ((static_cast<float>(1.0+sin((2.0*X-1.0)*PI)))/2.0f)
	#define NLIN(X) (X)

	float x1 = static_cast<R32>(ix1);
	float x2 = static_cast<R32>(ix2);//reinterpret_cast
	float y1 = static_cast<R32>(iy1);
	float y2 = static_cast<R32>(iy2);
	float  x = static_cast<R32>(ix);
	float X2 = x2 - x1;
	float Y2 = y2 - y1;
	float  X =  x - x1;
	float  Y = (NLIN((X/X2))*Y2)+y1;

	int32_t y = static_cast<int32_t>(Y);

	return y;
}
