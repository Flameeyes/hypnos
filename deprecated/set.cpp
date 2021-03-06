  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "common_libs.h"
#include "race.h"
#include "set.h"
#include "range.h"
#include "network.h"

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  Purpose : define standard container for a set of objects which responds to common
  queried sets, like items worn by a char or chars near a point. Access available
  both to AMX and native (C++) code. Last 16 sets are reserved to C++ code, so
  unclosed AMX sets will make nxw unable to create new AMX sets, but will keep C++
  code depending on sets working.

  Author : Xanathar.
  -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/


namespace amxSet {

AMX_WRAPPER_DB g_oSet;
static uint32_t currentIndex=INVALID;

uint32_t create( )
{
	++currentIndex;

	uint32_t iSet=currentIndex;
	g_oSet.insert( make_pair( iSet, new NxwWrapper() ) );
	return iSet;
}


void deleteSet( uint32_t iSet )
{
	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter!=g_oSet.end() )
		g_oSet.erase( iter );
}

bool end( uint32_t iSet )
{
	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter!=g_oSet.end() )
		if( iter->second!=NULL ) {
			return iter->second->isEmpty();
		}
	return true;
}

void rewind( uint32_t iSet )
{
	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter!=g_oSet.end() )
		if( iter->second!=NULL ) {
			iter->second->rewind();
		}
}

void next( uint32_t iSet )
{
	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter!=g_oSet.end() )
		if( iter->second!=NULL ) {
			(*iter->second)++;
		}
}


uint32_t get( SERIAL iSet)
{
	uint32_t ser = INVALID;
	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter!=g_oSet.end() )
		if( iter->second!=NULL ) {
			ser = iter->second->get();
			(*iter->second)++;
		}
	return ser;
}


void add( uint32_t iSet, SERIAL nVal )
{
	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter!=g_oSet.end() )
		if( iter->second!=NULL )
			iter->second->insert(nVal);
}

void copy( uint32_t iSet, const NxwWrapper& from )
{
	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter!=g_oSet.end() )
		if( iter->second!=NULL )
			iter->second->copyQ( from );
}

uint32_t size( uint32_t iSet)
{
	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter!=g_oSet.end() )
		if( iter->second!=NULL )
			return iter->second->size();
	return 0;
}


void addOwnedNpcs( uint32_t iSet, pChar pc, bool includeStabled, bool onlyFollowing )
{

	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter != g_oSet.end() )
	{
		NxwCharWrapper* sc=static_cast<NxwCharWrapper*>(iter->second);
		sc->fillOwnedNpcs( pc, includeStabled, onlyFollowing );
		sc->rewind();
	}
}

void addNpcsNearXY( uint32_t iSet, uint16_t x, uint16_t y, int distance )
{

	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter != g_oSet.end() )
	{
		NxwCharWrapper* sc=static_cast<NxwCharWrapper*>(iter->second);
		sc->fillNpcsNearXY( x, y, distance );
		sc->rewind();
	}
}

void addNpcsNearObj( uint32_t iSet, pChar pc, int nDistance )
{

	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter != g_oSet.end() )
	{
		NxwCharWrapper* sc=static_cast<NxwCharWrapper*>(iter->second);
		sc->fillNpcsNear( pc, nDistance );
		sc->rewind();
	}
}

void addNpcsNearObj( uint32_t iSet, pItem pi, int nDistance )
{

	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter != g_oSet.end() )
	{
		NxwCharWrapper* sc=static_cast<NxwCharWrapper*>(iter->second);
		sc->fillNpcsNear( pi, nDistance );
		sc->rewind();
	}
}

void addPartyFriend( uint32_t iSet, pChar pc, int distance, bool excludeThis )
{

	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter != g_oSet.end() )
	{
		NxwCharWrapper* sc=static_cast<NxwCharWrapper*>(iter->second);
		sc->fillPartyFriend( pc, distance, excludeThis );
		sc->rewind();
	}
}

void addItemsInContainer( uint32_t iSet, pItem pi, bool includeSubCont, bool includeOnlyFirstSubCont )
{

	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter != g_oSet.end() )
	{
		NxwItemWrapper* si=static_cast<NxwItemWrapper*>(iter->second);
		si->fillItemsInContainer( pi, includeSubCont, includeOnlyFirstSubCont );
		si->rewind();
	}
}

void addItemWeared( uint32_t iSet, pChar pc, bool includeLikeHair, bool includeProtectedLayer, bool excludeIllegalLayer )
{

	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter != g_oSet.end() )
	{
		NxwItemWrapper* si=static_cast<NxwItemWrapper*>(iter->second);
		si->fillItemWeared( pc, includeLikeHair, includeProtectedLayer, excludeIllegalLayer );
		si->rewind();
	}
}

void addItemsAtXY( uint32_t iSet, uint16_t x, uint16_t y, uint32_t type )
{

	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter != g_oSet.end() )
	{
		NxwItemWrapper* si=static_cast<NxwItemWrapper*>(iter->second);
		si->fillItemsAtXY( x, y, type );
		si->rewind();
	}
}

void addItemsNearXY( uint32_t iSet, uint16_t x, uint16_t y, int distance, bool excludeNotMovable )
{

	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter != g_oSet.end() )
	{
		NxwItemWrapper* si=static_cast<NxwItemWrapper*>(iter->second);
		si->fillItemsNearXYZ( x, y, distance, excludeNotMovable );
		si->rewind();
	}
}

inline void NxwSocketWrapper2NxwCharWrapper( NxwSocketWrapper& sw, NxwCharWrapper* sc )
{
	for( sw.rewind(); !sw.isEmpty(); sw++ ) {
		NXWCLIENT ps = sw.getClient();
		if( ps != 0 ) {
			pChar pc=ps->currChar();
			if(pc)
				sc->insert( pc->getSerial() );
		}
	}
}

void addAllOnlinePlayers( uint32_t iSet )
{
	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter != g_oSet.end() )
	{
		NxwSocketWrapper sw;
		sw.fillOnline();

		NxwSocketWrapper2NxwCharWrapper( sw, static_cast<NxwCharWrapper*>(iter->second) );
		iter->second->rewind();
	}
}

void addOnlinePlayersNearChar( uint32_t iSet, pChar pc, bool excludeThis, int distance )
{

	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter != g_oSet.end() )
	{
		NxwSocketWrapper sw;
		sw.fillOnline( pc, excludeThis, distance );

		NxwSocketWrapper2NxwCharWrapper( sw, static_cast<NxwCharWrapper*>(iter->second) );
		iter->second->rewind();
	}
}

void addOnlinePlayersNearItem( uint32_t iSet, pItem pi, int distance )
{

	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter != g_oSet.end() )
	{
		NxwSocketWrapper sw;
		sw.fillOnline( pi, distance );

		NxwSocketWrapper2NxwCharWrapper( sw, static_cast<NxwCharWrapper*>(iter->second) );
		iter->second->rewind();
	}
}

void addOnlinePlayersNearXY( uint32_t iSet, uint16_t x, uint16_t y, int distance )
{
 
	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter != g_oSet.end() )
	{
		NxwSocketWrapper sw;
		Location loc; loc.x=x; loc.y=y;
		sw.fillOnline( loc, distance );

		NxwSocketWrapper2NxwCharWrapper( sw, static_cast<NxwCharWrapper*>(iter->second) );
		iter->second->rewind();
	}
}

void addGuildMembers( uint32_t iSet, SERIAL guild )
{
 
	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter != g_oSet.end() )
	{
		NxwCharWrapper* sc = static_cast<NxwCharWrapper*>(iter->second);
		sc->fillGuildMembers( guild );
		sc->rewind();
	}
}

void addGuildRecruits( uint32_t iSet, SERIAL guild )
{
 
	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter != g_oSet.end() )
	{
		NxwCharWrapper* sc = static_cast<NxwCharWrapper*>(iter->second);
		sc->fillGuildRecruits( guild );
		sc->rewind();
	}
}

void addGuilds( uint32_t iSet, SERIAL guild )
{
 
	AMX_WRAPPER_DB::iterator iter( g_oSet.find( iSet ) );
	if( iter != g_oSet.end() )
	{
		NxwItemWrapper* si = static_cast<NxwItemWrapper*>( iter->second );
		si->fillGuilds( guild );
		si->rewind();
	}
}


} //namespace


/*!
\brief Constructor
*/
NxwWrapper::NxwWrapper() { 
	this->rewind();
};

/*!
\brief Destructor
*/
NxwWrapper::~NxwWrapper() {
};


/*!
\brief Check if empty
\author Endymion
\return true if empty
*/
bool NxwWrapper::isEmpty()
{
	return ( ( this->vect.size()==0 ) || ( current==vect.end() ) );	
};

/*!
\brief Move to next
\author Endymion
\return the object itself (standard)
*/
NxwWrapper& NxwWrapper::operator++(int)
{
	next();
	return (*this);
}

/*!
\brief Clear all list
\author Endymion
*/
void NxwWrapper::clear()
{
	vect.clear();
	this->rewind();
}

/*!
\brief Get size of list
\author Endymion
\return size
*/
uint32_t NxwWrapper::size()
{
	return vect.size();
}

/*!
\brief Move to next value
\author Endymion
*/
void NxwWrapper::next()
{
	current++;
}

/*!
\brief Reset internal counter
\author Endymion
*/
void NxwWrapper::rewind()
{
	current=vect.begin();
}

/*!
\brief Get the current value and after move to next
\author Endymion
\return the current value
*/
uint32_t NxwWrapper::get()
{
	if( isEmpty() )
		return INVALID;
	else {
		return (*current);
	}

};

/*!
\brief Return a copy of the queue
\author Sparhawk
\param from the origin of the queue
*/
void NxwWrapper::copyQ( const NxwWrapper& from )
{
	vect = from.vect;
}

/*!
\brief Insert a new value
\author Endymion
\param s the value
*/
void NxwWrapper::insert( uint32_t s )
{
	uint32_vector::const_iterator i = std::find( vect.begin(), vect.end(), s);
	if( i==vect.end() ) //unique
		vect.push_back( s );
};


/*!
\brief Constructor
*/
NxwSerialWrapper::NxwSerialWrapper() { };

/*!
\brief Destructor
*/
NxwSerialWrapper::~NxwSerialWrapper() { };


/*!
\brief Get the current serial
\author Endymion
\return the current serial
*/
uint32_t NxwSerialWrapper::getSerial()
{
	return get();
};

/*!
\brief Insert a new serial
\author Endymion
\param s the serial
*/
void NxwSerialWrapper::insertSerial( uint32_t s )
{
	if( s!=INVALID )
		insert( s );
};

/*!
\brief Insert a new serial
\author Endymion
\param obj the object
*/
void NxwSerialWrapper::insertSerial( cObject* obj )
{
	if( (obj!=NULL) )
		this->insertSerial( obj->getSerial() );
};


/*!
\brief Fills with serial in a container
\author Endymion
\param serial the serial
\param bIncludeSubContained if true recurse subcontainers
\param bIncludeOnlyFirstSubcont if true only recurse first sub container
*/
void NxwSerialWrapper::fillSerialInContainer( uint32_t serial, bool bIncludeSubContained, bool bIncludeOnlyFirstSubcont )
{
	
	std::map< uint32_t , vector<pItem> >::iterator cont( pointers::pContMap.find( serial ) );
	if( cont != pointers::pContMap.end() && !cont->second.empty() )
	{
		std::vector<pItem>::iterator iter( cont->second.begin() ), end( cont->second.end() );
		pItem pi;
		for( ; iter != end; ++iter )
		{

			pi=(*iter);

			insertSerial(pi);
			if (pi->type==ITYPE_CONTAINER)		// a subcontainer ?
			{
				if (bIncludeSubContained ) {
					fillSerialInContainer( pi, bIncludeOnlyFirstSubcont ? false : bIncludeSubContained);
				} //Endymion bugfix, becuase not added to set before :[
			}
		}
	}
}

/*!
\brief Fills with serial in a container
\author Endymion
\param obj the object
\param bIncludeSubContained if true recurse subcontainers
\param bIncludeOnlyFirstSubcont if true only recurse first sub container
*/
void NxwSerialWrapper::fillSerialInContainer( cObject* obj, bool bIncludeSubContained, bool bIncludeOnlyFirstSubcont )
{
	if( obj != 0 )
		fillSerialInContainer( obj->getSerial(), bIncludeSubContained, bIncludeOnlyFirstSubcont );
}


/*!
\brief Constructor
*/
NxwCharWrapper::NxwCharWrapper() { };

NxwCharWrapper::NxwCharWrapper( const NxwCharWrapper& that )
{
	copyQ( that );
}

/*!
\brief Destructor
*/
NxwCharWrapper::~NxwCharWrapper() { };

/*!
\brief Get the current char and after move to next
\author Endymion
\return ptr to the current char
*/
pChar NxwCharWrapper::getChar()
{
	return cSerializable::findCharBySerial( getSerial() );
};

/*!
\brief Insert a new char
\author Endymion
\param pc the char
*/
void NxwCharWrapper::insertChar( pChar pc )
{
	if ( ! pc ) return;
	insertSerial( pc->getSerial() );
};

/*!
\brief Fills a set with a list of npcs owned by a char
\author Endymion
\param pc the char
\param bIncludeStabled if true stabled pets should be included
\param bOnlyFollowing if true only following pets should be included
\warning this function ADD new char to current list
*/
void NxwCharWrapper::fillOwnedNpcs( pChar pc, bool bIncludeStabled, bool bOnlyFollowing )
{

	if ( ! pc ) return;

	std::map< uint32_t, std::vector< pChar > >::iterator vect( pointers::pOwnCharMap.find( pc->getSerial() ) );
	if( ( vect!=pointers::pOwnCharMap.end() ) && !vect->second.empty() ) {

		std::vector< pChar >::iterator iter( vect->second.begin() ), end( vect->second.end() );
		pChar poOwnedChr;
		for( ; iter != end; ++iter ) {
			poOwnedChr = (*iter);

			if( poOwnedChr )
			{
				if ((poOwnedChr->ftargserial==pc->getSerial()) ||
					( !bOnlyFollowing && bIncludeStabled && ( poOwnedChr->isStabled() ) ) ) {
					insertSerial(poOwnedChr->getSerial());
				}
			}
		}
	} 
}

/*!
\brief Fills with a list of chars at given location
\author Luxor
\param location the location
*/
void NxwCharWrapper::fillCharsAtXY( uint16_t x, uint16_t y, bool bExcludeOfflinePlayers, bool bOnlyPlayer )
{

	if( mapRegions->isValidCoord( x, y ) )
	{
		uint16_t 	nowx = x / REGION_GRIDSIZE,
			nowy = y / REGION_COLSIZE;

		if( !mapRegions->regions[nowx][nowy].charsInRegions.empty() )
		{
			uint32_set::iterator	it( mapRegions->regions[nowx][nowy].charsInRegions.begin() ),
						end( mapRegions->regions[nowx][nowy].charsInRegions.end() );
			for( ; it != end; ++it ) {
				pChar pc = cSerializable::findCharBySerial( *it );
				if ( pc == 0 )
					continue;
				if ( pc->getPosition().x != x || pc->getPosition().y != y )
					continue;
				if ( bExcludeOfflinePlayers && !pc->npc && !pc->IsOnline() )
					continue;
				if ( bOnlyPlayer && pc->npc )
					continue;
				if ( pc->mounted )
					continue;
				insertSerial( pc->getSerial() );
			}
		}
	}
}

/*!
\brief Fills with a list of chars at given location
\author Luxor
\param location the location
*/
void NxwCharWrapper::fillCharsAtXY( Location location, bool bExcludeOfflinePlayers, bool bOnlyPlayer )
{
	fillCharsAtXY( location.x, location.y, bExcludeOfflinePlayers, bOnlyPlayer );
}




/*!
\brief Fills a set with a list of char near x, y
\author Endymion
\param x the x location
\param y the y location
\param nDistance the distance requested
\param bExcludeOfflinePlayers if true exclude offline players from search
\warning this function ADD new char to current list
*/
void NxwCharWrapper::fillCharsNearXYZ ( uint16_t x, uint16_t y, int nDistance, bool bExcludeOfflinePlayers, bool bOnlyPlayer )
{
	Location pos = Location( x, y, 0 );
	if( mapRegions->isValidCoord( x, y ) )
		for( int32_t ix=x-REGION_GRIDSIZE; ix<=x+REGION_GRIDSIZE; ix+=REGION_GRIDSIZE ) {
			if( ix>=0 ) {
				for( int32_t iy=y-REGION_COLSIZE; iy<=y+REGION_COLSIZE; iy+=REGION_COLSIZE ) {
					if( iy>=0 && mapRegions->isValidCoord( x, y ) ) {
						uint16_t nowx = ix/REGION_GRIDSIZE, nowy= iy/REGION_COLSIZE;

						if( mapRegions->regions[nowx][nowy].charsInRegions.empty() )
							continue;

						uint32_set::iterator	iter( mapRegions->regions[nowx][nowy].charsInRegions.begin() ),
									end( mapRegions->regions[nowx][nowy].charsInRegions.end() );
						for( ; iter != end; ++iter ) {
							pChar pc=cSerializable::findCharBySerial( *iter );
							if( pc == 0 )
								continue;
							if( pc->isStabled() || pc->mounted )
								continue;
							uint32_t uiDist = uint32_t( dist( pos, pc->getPosition(), false ) );
							if ( uiDist > nDistance )
								continue;
							if ( bOnlyPlayer && pc->npc )
								continue;
							if ( bExcludeOfflinePlayers && !pc->npc && !pc->IsOnline() )
								continue;
							insertSerial( pc->getSerial() );
						}
					}
				}
			}
		}


}


/*!
\brief Fills a set with a list of char near location
\author Endymion
\param location the location
\param nDistance the distance requested
\param bExcludeOfflinePlayers if true exclude offline players from search
\warning this function ADD new char to current list
*/
void NxwCharWrapper::fillCharsNearXYZ ( Location location, int nDistance, bool bExcludeOfflinePlayers , bool bOnlyPlayer )
{
	fillCharsNearXYZ( location.x, location.y, nDistance, bExcludeOfflinePlayers );
}


/*!
\brief Fills a set with a list of npcs near location
\author Endymion
\param x the x location
\param y the y location
\param nDistance the distance requested
\warning this function ADD new char to current list
*/
void NxwCharWrapper::fillNpcsNearXY( uint16_t x, uint16_t y, int nDistance )
{
	if( mapRegions->isValidCoord( x, y ) )
		for( int32_t ix=x-REGION_GRIDSIZE; ix<=x+REGION_GRIDSIZE; ix+=REGION_GRIDSIZE ) {
			if( ix>=0 ) {
				for( int32_t iy=y-REGION_COLSIZE; iy<=y+REGION_COLSIZE; iy+=REGION_COLSIZE ) {
					if( iy>=0 && mapRegions->isValidCoord( x, y ) ) {
						uint16_t	nowx = ix/REGION_GRIDSIZE,
							nowy= iy/REGION_COLSIZE;

						if( mapRegions->regions[nowx][nowy].charsInRegions.empty() )
							continue;

						uint32_set::iterator iter( mapRegions->regions[nowx][nowy].charsInRegions.begin() ),
									end( mapRegions->regions[nowx][nowy].charsInRegions.end() );
						for( ; iter != end; ++iter ) {
							pChar pc=cSerializable::findCharBySerial( *iter );
							if( pc == 0 )
								continue;
							if( !pc->npc )
								continue;
							if(  !pc->isStabled() && !pc->mounted ) {
								int iDist=(int)dist(Location(x,y,0),pc->getPosition(), false);
								if (iDist <= nDistance)
									this->insertSerial(pc->getSerial());
							}
						}
					}
				}
			}
		}
}

/*!
\brief Fills a set with a list of npcs near char
\author Endymion
\param pc the char
\param nDistance the distance requested
\warning this function ADD new char to current list
*/
void NxwCharWrapper::fillNpcsNear( pChar pc, int nDistance )
{
	if ( ! pc ) return;
	fillNpcsNearXY(pc->getPosition().x, pc->getPosition().y, nDistance );
}

/*!
\brief Fills a set with a list of npcs near item
\author Endymion
\param pi the item
\param nDistance the distance requested
\warning this function ADD new char to current list
\note location is true, of the in world container
*/
void NxwCharWrapper::fillNpcsNear( pItem pi, int nDistance )
{
	if ( ! pi ) return;

	pItem out=pi->getOutMostCont();
	if( out->isInWorld() ) {
		fillNpcsNearXY( out->getPosition().x, out->getPosition().y, nDistance );
	}
	else {
		fillNpcsNear( cSerializable::findCharBySerial( out->getContSerial() ), nDistance );
	}

}


/*!
\brief Fills a set with a list of char in same party of given char
\author Endymion
\param pc the player
\param nDistance the distance requested
\param bExcludeThis if true exclude this
\param nDistance maximum distance from the player
\warning this function ADD new char to current list
\note offline player are not added
*/
void NxwCharWrapper::fillPartyFriend( pChar pc, uint32_t nDistance, bool bExcludeThis )
{
	if ( ! pc ) return;
	if( pc->party != INVALID )
	{
		NxwCharWrapper sc;
		sc.fillCharsNearXYZ( pc->getPosition(), nDistance, true, true );
		pChar pj;
		for( sc.rewind(); !sc.isEmpty(); sc++ ) {
			pj=sc.getChar();
			if( pj )
				if( pc->party==pj->party ) {
					if( pc->distFrom( pj ) <= nDistance ) {
						if( !bExcludeThis || ( pc->getSerial()!=pj->getSerial32() ) )
							this->insert( pj->getSerial() );
				}
			}
		}
	}
}


/*!
\brief Fills a set with a list of member in given guild
\author Endymion
\param guild the guild
\warning this function ADD new char to current list
*/
void NxwCharWrapper::fillGuildMembers( uint32_t guild )
{
	pGuild pGuild = Guildz.getGuild( guild );
	if( pGuild != 0 )
	{
		std::map< uint32_t, pGuildMember >::iterator iter( pGuild->members.begin() ), end( pGuild->members.end() );
		for( ; iter!=end; iter++ ) {
			insertSerial( iter->first );
		}
	}
}

/*!
\brief Fills a set with a list of recruit in given guild
\author Endymion
\param guild the guild
\warning this function ADD new char to current list
*/
void NxwCharWrapper::fillGuildRecruits( uint32_t guild )
{
	pGuild pGuild = Guildz.getGuild( guild );
	if( pGuild != NULL )
	{
		std::map< uint32_t, pGuildRecruit >::iterator iter( pGuild->recruits.begin() ), end( pGuild->recruits.end() );
		for( ; iter!=end; iter++ ) {
			insertSerial( iter->first );
		}
	}
}

/*!
\brief Constructor
*/
NxwItemWrapper::NxwItemWrapper() { };

NxwItemWrapper::NxwItemWrapper( const NxwItemWrapper& that )
{
	copyQ( that );
}

/*!
\brief Destructor
*/
NxwItemWrapper::~NxwItemWrapper() { };

/*!
\brief Get the current item
\author Endymion
\return the current item
*/
pItem NxwItemWrapper::getItem()
{
	return cSerializable::findItemBySerial( getSerial() );
};

/*!
\brief Insert a new item
\author Endymion
\param pi the item
*/
void NxwItemWrapper::insertItem( pItem pi )
{
	if ( ! pi ) return;
	insertSerial( pi->getSerial() );
};

/*!
\brief Fills with item in a container
\author Endymion
\param pi the container
\param bIncludeSubContained if true recurse subcontainers
\param bIncludeOnlyFirstSubcont if true only recurse first sub container
*/
void NxwItemWrapper::fillItemsInContainer( pItem pi, bool bIncludeSubContained, bool bIncludeOnlyFirstSubcont )
{
	if ( ! pi ) return;
	fillSerialInContainer( pi, bIncludeSubContained, bIncludeOnlyFirstSubcont );
}


/*!
\brief Fills with a list of item at given location
\author Endymion
\param x the x location
\param y the y location
\param type if not INVALID only add item with this type
\param id if not INVALID only add item with this id
\warning this function ADD new char to current list
*/
void NxwItemWrapper::fillItemsAtXY( uint16_t x, uint16_t y, int32_t type, int32_t id )
{
	
	if(!mapRegions->isValidCoord( x, y ))
		return;

	uint16_t nowx = x / REGION_GRIDSIZE, nowy = y / REGION_COLSIZE;

	if( mapRegions->regions[nowx][nowy].itemsInRegions.empty() )
		return;

	uint32_set::iterator	iter( mapRegions->regions[nowx][nowy].itemsInRegions.begin() ),
				end( mapRegions->regions[nowx][nowy].itemsInRegions.end() );\

	for( ; iter != end; ++iter ) {
		// <Luxor bug fix>
		pItem pi=cSerializable::findItemBySerial( *iter );
		if ( pi == 0 )
			continue;
		if ( !pi->isInWorld() )
			continue;
		if ( pi->getPosition().x != x || pi->getPosition().y != y )
			continue;
		// </Luxor>
		if ( type == INVALID || pi->type==(uint32_t)type )
			if ( id == INVALID || pi->getId() == id )
				insertItem(pi);
	}
}

/*!
\brief Fills with a list of item at given location
\author Endymion
\param location the location
\param type if not INVALID only add item with this type
\param id if not INVALID only add item with this id
\warning this function ADD new char to current list
*/
void NxwItemWrapper::fillItemsAtXY( Location location, int32_t type, int32_t id )
{
	fillItemsAtXY( location.x, location.y, type, id ); 
}

/*!
\brief Fills with a list of item near given location
\author Endymion
\param x the x location
\param y the y location
\param nDistance only add item in distance range
\param bExcludeNotMovableItems if true exluce not movable items
\warning this function ADD new char to current list
*/
void NxwItemWrapper::fillItemsNearXYZ ( uint16_t x, uint16_t y, int nDistance, bool bExcludeNotMovableItems )
{
	if( mapRegions->isValidCoord( x, y ) )
		for( int32_t ix=x-REGION_GRIDSIZE; ix<=x+REGION_GRIDSIZE; ix+=REGION_GRIDSIZE ) {
			if( ix>=0 ) {
				for( int32_t iy=y-REGION_COLSIZE; iy<=y+REGION_COLSIZE; iy+=REGION_COLSIZE ) {
					if( iy>=0 && mapRegions->isValidCoord( x, y ) ) {
						uint16_t	nowx = ix/REGION_GRIDSIZE,
							nowy= iy/REGION_COLSIZE;

						if( mapRegions->regions[nowx][nowy].itemsInRegions.empty() )
							continue;

						uint32_set::iterator	iter( mapRegions->regions[nowx][nowy].itemsInRegions.begin() ),
									end( mapRegions->regions[nowx][nowy].itemsInRegions.end() );
						for( ; iter != end; ++iter ) {
							pItem pi=cSerializable::findItemBySerial( *iter );
							if( pi != 0 )
								if( pi->isInWorld() ) {
									int iDist=(int)dist(Location(x,y,0), pi->getPosition(), false);
									if (iDist <= nDistance) {
										if ((!bExcludeNotMovableItems) || (pi->magic != 2 && pi->magic != 3))
										{
											insertItem(pi);
										} //if Distance
									}
								}
						}
					}
				}
			}
		}
}

/*!
\brief Fills with a list of item near given location
\author Endymion
\param location the location
\param nDistance only add item in distance range
\param bExcludeNotMovableItems if true exluce not movable items
\warning this function ADD new char to current list
*/
void NxwItemWrapper::fillItemsNearXYZ ( Location location, int nDistance, bool bExcludeNotMovableItems )
{
	fillItemsNearXYZ( location.x, location.y, nDistance, bExcludeNotMovableItems );
}

/*!
\brief Fills a set with a list of item worn by given char
\author Endymion
\param pc the char
\param bExcludeIllegalLayer if true layer like backpack, trade are excluded
\param bIncludeLikeHair if true add also hair, beard ecc
\param bIncludeProtectedLayer if true add also protected layer
\warning this function ADD new char to current list
*/
void NxwItemWrapper::fillItemWeared( pChar pc, bool bIncludeLikeHair, bool bIncludeProtectedLayer, bool bExcludeIllegalLayer )
{
	
	if ( ! pc ) return;

	std::map< uint32_t , vector<pItem> >::iterator cont( pointers::pContMap.find( pc->getSerial() ) );
	if( cont==pointers::pContMap.end() || cont->second.empty() )
		return;

	std::vector<pItem>::iterator	iter( cont->second.begin() ),
					end( cont->second.end() );
	for( ; iter != end; ++iter )
	{
	
		pItem pi_j=(*iter);
		if(! pi_j )			// just to be sure ;-)
			continue;
		
		switch( pi_j->layer ) {
			case LAYER_BACKPACK:
			case LAYER_MOUNT:
			case LAYER_TRADE_RESTOCK:
			case LAYER_TRADE_NORESTOCK:
			case LAYER_TRADE_BOUGHT:
			case LAYER_BANKBOX:
				if( bExcludeIllegalLayer )
					continue;
			case LAYER_BEARD:
			case LAYER_HAIR:
				if( !bIncludeLikeHair )
					continue;
		}

		//race protected
		if( Race::isRaceSystemActive() )
			if (!bIncludeProtectedLayer && ( Race::isProtectedLayer( (uint32_t) pc->race, pi_j->layer ) ) ) 
				continue;

		this->insertSerial( pi_j->getSerial() );
	}

}

/*!
\brief Fills a set with a list of all guilds or guild political
\author Endymion
\param guild INVALID for all or specific for use guild options
\param options 
\warning this function ADD new char to current list
*/
void NxwItemWrapper::fillGuilds( uint32_t guild )
{
	if( guild == INVALID ) { //all guilds
		std::map< uint32_t, pGuild >::iterator iter( Guildz.guilds.begin() ), end( Guildz.guilds.end() );
		for( ; iter!=end; ++iter ) {
			insertSerial( iter->first );
		}
	}/*
	else {

		pGuild pGuild = Guildz.getGuild( guild );
		if( pGuild != 0 )
			if( options == GUILD_WAR ) {
				std::vector<uint32_t>::iterator iter( pGuild->war.begin() ), end( pGuild->war.end() );
				for( ; iter!=end; ++iter ) {
					insertSerial( *iter );
				}
			}
			else if( options == GUILD_ALLIED ) {
				std::vector<uint32_t>::iterator iter( pGuild->allied.begin() ), end( pGuild->allied.end() );
				for( ; iter!=end; ++iter ) {
					insertSerial( *iter );
				}
			}
	}*/

}



/*!
\brief Constructor
*/
NxwSocketWrapper::NxwSocketWrapper() { };

/*!
\brief Destructor
*/
NxwSocketWrapper::~NxwSocketWrapper() { };

/*!
\brief Get the current socket
\author Endymion
\return the current socket
*/
uint32_t NxwSocketWrapper::getSocket()
{
	return get();
}

/*!
\brief Get the current client
\author Endymion
\return the current client
*/
NXWCLIENT NxwSocketWrapper::getClient()
{
	if( isEmpty() )
		return NULL;
	else {
		return getClientFromSocket( get() );
	}
}

/*!
\brief Insert a new socket
\author Endymion
\param s the socket
*/
void NxwSocketWrapper::insertSocket( NXWSOCKET s )
{
	if( s!=INVALID )
		insert( s );
}

/*!
\brief Insert a new client
\author Endymion
\param ps the client
*/
void NxwSocketWrapper::insertClient( NXWCLIENT ps )
{
	if( ps != 0 )
		insertSocket( ps->toInt() );
}


/*!
\brief Fills with a list of socket
\author Endymion, rewritten by Luxor
\param onlyNearThis if true only socket near given char are added
\param bExcludeThis if true given char is not added to list
\param nDistance maximum distance from the character
\warning this function ADD new char to current list
\note Akron - Changed nDistance to uint32_t and doxygen documented it
*/
void NxwSocketWrapper::fillOnline( pChar onlyNearThis, bool bExcludeThis, uint32_t nDistance )
{
	pChar	pc;
	bool	validOnlyNearThis = onlyNearThis;

	for (int32_t i = 0; i < now; ++i )
	{
		pc = cSerializable::findCharBySerial( currchar[i] );
		//
		//	Sparhawk pc checking unnecessary here, is done by findCharBySerial
		//
		if ( pc != 0 )
			if ( !validOnlyNearThis )
				insertSocket(i);
			else
				if ( onlyNearThis->distFrom(pc) <= nDistance )
					if( !(bExcludeThis && ( pc->getSerial()==onlyNearThis->getSerial32() ) ) )
						insertSocket(i);
	}
}

/*!
\brief Fills with a list of socket
\author Endymion, rewritten by Luxor
\param location only socket near given location are added
\param nDistance the distance requested
\warning this function ADD new char to current list
*/
void NxwSocketWrapper::fillOnline( Location location, int nDistance )
{
	pChar pc;
	for (int32_t i = 0; i < now; ++i )
	{
		pc = cSerializable::findCharBySerial( currchar[i] );
		//
		//	Sparhawk pc checking unnecessary here, is done by findCharBySerial
		//
		if ( pc != 0 )
			if (dist(location, pc->getPosition()) <= nDistance)
				insertSocket(i);
	}
}

/*!
\brief Fills with a list of socket
\author Endymion
\param onlyNearThis only socket near given item are added
\param nDistance the distance requested
\warning this function ADD new char to current list
*/
void NxwSocketWrapper::fillOnline( pItem onlyNearThis, int nDistance )
{
	if ( ! onlyNearThis ) return;

	pItem out=onlyNearThis->getOutMostCont();
	if( out->isInWorld() ) {
		fillOnline(onlyNearThis->getPosition(), nDistance );
	}
	else {
		pChar own=cSerializable::findCharBySerial( out->getContSerial() );
		if( own )
			fillOnline( own->getPosition(), nDistance );
		else
			fillOnline( );
	}

}

/*!
\brief Fills with a list of socket
\author Endymion, rewritten by Luxor
\warning this function ADD new char to current list
*/
void NxwSocketWrapper::fillOnline(  )
{

	for (int32_t i = 0; i < now; ++i ) {
		if (currchar[i] != INVALID ) 
			insertSocket(i);
	}

}
