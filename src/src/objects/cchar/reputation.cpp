/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief cChar's reputation methods
*/

#include "enums.hpp"
#include "networking/cclient.hpp"
#include "objects/cchar.hpp"
#include "objects/cpc.hpp"
#include "objects/cnpc.hpp"

const bool cChar::isGrey() const
{
	if ( dynamic_cast<cNPC>(this) || isMurderer() || isCriminal() )
		return false;
	
	if ( (karma <= -10000) || isPermaGrey() )
		return true;
	
	return false;
}

/*!
\brief increase or decrease the fame of the char
\param value positive or negative value to add to fame
\note every increase of karma have an event and stuff related
*/
void cChar::modifyFame( int32_t value )
{
	if( getFame() > 10000 )
		setFame( 10000 );

	if ( ! value )
		return;
	
	int32_t	nFame	= value;
	int	nChange	= 0;
	int	nCurFame= fame;
	
	bool gained = true;

	//! \todo Need to check this!
	if( nCurFame > nFame ) // if player fame greater abort function
	{
		return;
	}

	if( nCurFame < nFame )
	{
		nChange=(nFame-nCurFame)/75;
		fame=(nCurFame+nChange);
		gained = true;
	}

	if( isDead() )
	{
		if(nCurFame<=0)
		{
			fame=0;
		}
		else
		{
			nChange=(nCurFame-0)/25;
			fame=(nCurFame-nChange);
		}
		gained = false; 
	}
	
	if ( ! nChange )
		return;
	
	if ( events[evtChrOnReputationChange] ) {
		cVariantVector params = cVariantVector(3);
		params[0] = getSerial(); params[1] = gained ? nChange : -nChange;
		params[3] = repFame;
		events[evtChrOnReputationChange]->setParams(params);
		events[evtChrOnReputationChange]->execute();
		if ( events[evtChrOnReputationChange]->isBypassed() )
			return;
	}
	
	pPC tpc = NULL; // This PC
	if ( ! (tpc = dynamic_cast<pPC>(this) ) || ! tpc->getClient() )
		return;
	
	if(nChange<=25)
	{
		if(gained)
			getClient()->sysmessage( "You have gained a little fame.");
		else
			getClient()->sysmessage( "You have lost a little fame.");
	} else if(nChange<=75)
	{
		if(gained)
			getClient()->sysmessage( "You have gained some fame.");
		else
			getClient()->sysmessage( "You have lost some fame.");
	} else if(nChange<=100)
	{
		if(gained)
			getClient()->sysmessage( "You have gained alot of fame.");
		else
			getClient()->sysmessage( "You have lost alot of fame.");
	} else if(nChange>100)
	{
		if(gained)
			getClient()->sysmessage( "You have gained a huge amount of fame.");
		else
			getClient()->sysmessage( "You have lost a huge amount of fame.");
	}
}

/*!
\brief increase or decrease the karma of the char
\param value positive or negative value to add to karma
\param pKilled ptr to killed char
\note every increase of karma has a related event
\note Sparhawk: karma increase now can also be applied to npc's
*/
void cChar::increaseKarma( int32_t value, pChar pKilled )
{
	int32_t nCurKarma = getKarma();

	if( nCurKarma > 10000 )
		setKarma( 10000 );
	else if( nCurKarma < -10000 )
		setKarma( -10000 );

	if ( ! value )
		return;
	
	int32_t nKarma	= value,
		nChange	= 0;

	bool	positiveKarmaEffect	= false;

	if	( nCurKarma < nKarma && nKarma > 0 )
	{
		nChange=((nKarma-nCurKarma)/75);
		SetKarma( GetKarma() + nChange );
		positiveKarmaEffect = true;
	}
	else if ( nCurKarma > nKarma )
	{
		if ( ! pKilled )
		{
			nChange=((nCurKarma-nKarma)/50);
			SetKarma( GetKarma() + nChange );
		}
		else if( pKilled->GetKarma()>0 )
		{
			nChange=((nCurKarma-nKarma)/50);
			SetKarma( GetKarma() + nChange );
		}
	}
	
	if ( ! nChange )
		return;

	if ( events[evtChrOnReputationChange] ) {
		cVariantVector params = cVariantVector(3);
		params[0] = getSerial(); params[1] = positiveKarmaEffect ? nChange : -nChange;
		params[3] = repKarma;
		events[evtChrOnReputationChange]->setParams(params);
		events[evtChrOnReputationChange]->execute();
		if ( events[evtChrOnReputationChange]->isBypassed() )
			return;
	}
	
	pPC tpc = NULL; // This PC
	pClient tcl = NULL; // This client
	if ( ! (tpc = dynamic_cast<pPC>(this) ) || ! (tcl = tpc->getClient()) )
		return;
	
	if(nChange<=25) {
		if(positiveKarmaEffect)
			tcl->sysmessage( "You have gained a little karma.");
		else
			tcl->sysmessage( "You have lost a little karma.");
	} else if(nChange<=75) {
		if(positiveKarmaEffect)
			tcl->sysmessage( "You have gained some karma.");
		else
			tcl->sysmessage( "You have lost some karma.");
	} else if(nChange<=100) {
		if(positiveKarmaEffect)
			tcl->sysmessage( "You have gained alot of karma.");
		else
			tcl->sysmessage( "You have lost alot of karma.");
	} else if(nChange>100) {
		if(positiveKarmaEffect)
			tcl->sysmessage( "You have gained a huge amount of karma.");
		else
			tcl->sysmessage( "You have lost a huge amount of karma.");
	}
}
