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

const bool cChar::IsGrey() const
{
	if ( npc || IsMurderer() || IsCriminal() )
		return false;
	else
		if ( (karma <= -10000) || (nxwflags[0] & flagPermaGrey) || (nxwflags[0] & flagGrey) )
			return true;
		else
			return false;
}

/*!
\brief increase or decrease the fame of the char
\author Endymion
\since 0.82a
\param value positive or negative value to add to fame
\note every increase of karma have an event and stuff related
*/
void cChar::modifyFame( int32_t value )
{
	if( GetFame() > 10000 )
		SetFame( 10000 );

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
		tVariantVector params = tVariantVector(3);
		params[0] = getSerial(); params[1] = gained ? nChange : -nChange;
		params[3] = REPUTATION_FAME;
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
\author Endymion
\since 0.82a
\param value positive or negative value to add to karma
\param killed ptr to killed char
\note every increase of karma has a related event
\note Sparhawk: karma increase now can also be applied to npc's
*/
void cChar::IncreaseKarma( int32_t value, pChar pKilled )
{
	int32_t nCurKarma = GetKarma();

	if( nCurKarma > 10000 )
		SetKarma( 10000 );
	else if( nCurKarma < -10000 )
		SetKarma( -10000 );

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
		tVariantVector params = tVariantVector(3);
		params[0] = getSerial(); params[1] = positiveKarmaEffect ? nChange : -nChange;
		params[3] = REPUTATION_KARMA;
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
		if(positiveKarmaEffect)
			sysmsg( "You have gained a little karma.");
		else
			sysmsg( "You have lost a little karma.");
	} else if(nChange<=75)
	{
		if(positiveKarmaEffect)
			sysmsg( "You have gained some karma.");
		else
			sysmsg( "You have lost some karma.");
	} else if(nChange<=100)
	{
		if(positiveKarmaEffect)
			sysmsg( "You have gained alot of karma.");
		else
			sysmsg( "You have lost alot of karma.");
	} else if(nChange>100)
	{
		if(positiveKarmaEffect)
			sysmsg( "You have gained a huge amount of karma.");
		else
			sysmsg( "You have lost a huge amount of karma.");
	}
}
