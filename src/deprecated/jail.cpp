/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "extras/jail.h"
#include "basics.h"
#include "inlines.h"

PRISONCELLVECTOR prison::cells;
JAILEDVECTOR prison::jailed;


/*!
\brief Constructor of cJailed
\author Endymion
*/
cJailed::cJailed()
{
	sec = 0;
	timer = 0;
	why = "";
	cell=INVALID;
};

/*!
\brief Destructor of cJailed
\author Endymion
*/
cJailed::~cJailed() { };

/*!
\brief Constructor of cPrisonCell
\author Endymion
*/
cPrisonCell::cPrisonCell() : pos(0, 0, 0, 0)
{
	serial=INVALID;
	free=true;
};

/*!
\brief Destructor of cJailed
\author Endymion
*/
cPrisonCell::~cPrisonCell() { };

namespace prison {
/*!
\brief archive a jail worldfile
\author Sparhawk
*/
void archive()
{
	std::string saveFileName( SrvParms->savePath + SrvParms->jailWorldfile + SrvParms->worldfileExtension );
	std::string timeNow( getNoXDate() );
	for( int i = timeNow.length() - 1; i >= 0; --i )
		switch( timeNow[i] )
		{
			case '/' :
			case ' ' :
			case ':' :
				timeNow[i]= '-';
		}
	std::string archiveFileName( SrvParms->archivePath + SrvParms->jailWorldfile + timeNow + SrvParms->worldfileExtension );


	if( rename( saveFileName.c_str(), archiveFileName.c_str() ) != 0 )
	{
		LogWarning("Could not rename/move file '%s' to '%s'\n", saveFileName.c_str(), archiveFileName.c_str() );
	}
	else
	{
		InfoOut("Renamed/moved file '%s' to '%s'\n", saveFileName.c_str(), archiveFileName.c_str() );
	}
}

}

void prison::safeoldsave()
{
	std::string oldFileName( SrvParms->savePath + SrvParms->jailWorldfile + SrvParms->worldfileExtension );
	std::string newFileName( SrvParms->savePath + SrvParms->jailWorldfile + SrvParms->worldfileExtension + "$" );
	remove( newFileName.c_str() );
	rename( oldFileName.c_str(), newFileName.c_str() );
}

/*!
\brief Auto check and free of jailed people
\author Endymion
*/
void prison::checkForFree()
{
	JAILEDVECTOR::iterator next;
	for( JAILEDVECTOR::iterator j = prison::jailed.begin(); j!=prison::jailed.end(); j=next )
	{
		next=j; next++;
		if( TIMEOUT( (*j).timer ) ) {
			pChar pc = cSerializable::findCharBySerial( (*j).serial );
			if( pc ) {
				prison::release( NULL, pc );
			}
			else { //invalid char in jail.. so remove it
				WarnOut( "invalid char serial [ %i ] found in jail, removing..", (*j).serial );
				prison::freePrisonCell( (*j).cell );
				prison::jailed.erase( j );
			}
			return;
		}

	}

}

/*!
\brief Release a jailed player
\author Endymion
\param releaser the releaser
\param pc the player jailed
*/
void prison::release( pChar releaser, pChar pc )
{
	if ( ! pc ) return;
	JAILEDVECTOR::iterator j = prison::jailed.begin();
	while(  j!=prison::jailed.end() && (*j).serial!=pc->getSerial() )	j++;

	pPC tmp;
	pClient pc_client, releaser_client;
	pc_client = (tmp = dynamic_cast<pPC>(pc))? tmp->getClient() : NULL;
	releaser_client = (tmp = dynamic_cast<pPC>(releaser))? tmp->getClient() : NULL;

	if(j==prison::jailed.end()) {
		if( releaser_client )
			releaser_client->sysmessage( "The player isn't jailed" );
		return;
	}

	pc->MoveTo(  j->oldpos );

	prison::freePrisonCell( j->cell );
	prison::jailed.erase( j );

	pc->jailed=false;
	pc->teleport();

	if( ! releaser ) {
		ServerLog.Write("%s is auto-released from jail \n", pc->getCurrentName().c_str());
		pc_client->sysmessage("Your jail time is over!");
	}
	else {
		releaser_client->sysmessage("%s is now free", pc->getCurrentName().c_str());
		pc_client->sysmessage( "%s have released you", releaser->getCurrentName().c_str() );
	}
}

/*!
\brief Free a cell
\author Endymion
\param cell the cell serial
\note only use internal.. use release for release a player
*/
void prison::freePrisonCell( uint32_t cell )
{
	PRISONCELLVECTOR::iterator iter= prison::cells.begin();
	while( iter!=prison::cells.end() && (*iter).serial!=cell ) iter++;
	if( iter==prison::cells.end() ) {
		WarnOut( "Free a cell not found ( serial %i )", cell );
		return;
	}

	(*iter).free=true;


}


void prison::addCell( uint32_t serial, uint32_t x, uint32_t y, uint32_t z )
{
	for( PRISONCELLVECTOR::iterator j = prison::cells.begin(); j!=prison::cells.end(); j++ )
		if( (*j).serial == serial )
			return;

	cPrisonCell c;

	c.serial=serial;
	c.pos.x=x;
	c.pos.y=y;
	c.pos.z=z;
	c.pos.dispz=z;

	prison::cells.push_back( c );



}
