/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"

cRegion::cRegion()
{
	for( int x=0; x<REGION_X_CELLS; x++ )
		for ( int y=0; y<REGION_Y_CELLS; y++ ) {
			this->regions[x][y].charsInRegions.clear();
			this->regions[x][y].itemsInRegions.clear();
		}

}

cRegion::~cRegion()
{


}

void cRegion::add( pChar pc )
{

	if(!pc )
		return;
	if(!isValidCoord(pc->getPosition()) )
		//move to safe location? boh
		return;

	this->removeNow( pc );
	this->addNow( pc );

}

void cRegion::add( pItem pi )
{

	if ( ! pi ) return;
	if( !pi || !pi->isInWorld() )
		return;
	if(!isValidCoord(pi->getPosition())) {
		//move to safe location? boh
		return;
	}

	this->removeNow( pi );
	this->addNow( pi );
}

void cRegion::remove( pChar pc )
{

	if ( ! pc ) return;

	if(!isValidCoord(pc->getPosition()) )
		return;

	this->removeNow( pc );

}

void cRegion::remove( pItem pi )
{

	if ( ! pi ) return;

	if(!isValidCoord(pi->getPosition().x) )
		return;

	this->removeNow( pi );
}

void cRegion::removeNow( pChar pc )
{

	WHERE_IS_NOW_MAP::iterator iternow( this->char_where_is_now.find( pc->getSerial() ) );
	if( iternow!=this->char_where_is_now.end() ) {
		RegCoordPoint* p = &iternow->second;
	
		uint32_set* iter = &this->regions[p->a][p->b].charsInRegions;
		uint32_set::iterator i( iter->find(pc->getSerial()) );
		if( i!=iter->end() )
			iter->erase( i );
	}
}

void cRegion::removeNow( pItem pi )
{

	WHERE_IS_NOW_MAP::iterator iternow( this->item_where_is_now.find( pi->getSerial() ) );
	if( iternow!=this->item_where_is_now.end() ) {
		RegCoordPoint* p = &iternow->second;
	
		uint32_set* iter = &this->regions[p->a][p->b].itemsInRegions;
		uint32_set::iterator i( iter->find(pi->getSerial()) );
		if( i!=iter->end() )
			iter->erase( i );
	}
}

void cRegion::addNow( pChar pc )
{
	RegCoordPoint p( pc->getPosition() ); 
	this->char_where_is_now.insert( make_pair( pc->getSerial(), p ) );
	this->regions[p.a][p.b].charsInRegions.insert( pc->getSerial() );
}

void cRegion::addNow( pItem pi )
{
	RegCoordPoint p( pi->getPosition() ); 
	this->item_where_is_now.insert( make_pair( pi->getSerial(), p ) );
	this->regions[p.a][p.b].itemsInRegions.insert( pi->getSerial() );
}

RegCoordPoint::RegCoordPoint( sLocation location )
{
	 this->a=location.x/REGION_GRIDSIZE;
	 this->b=location.y/REGION_COLSIZE;
}


