  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "common_libs.h"
#include "network.h"
#include "books.h"
#include "spawn.h"
#include "set.h"
#include "archive.h"
#include "sndpkg.h"
#include "inlines.h"

OBJECT_MAP cAllObjects::all;
uint32_t cAllObjects::current_item_serial=0x40000000;
uint32_t cAllObjects::current_char_serial=1; // ndEndy not 0 because target return 0 if invalid target, so OSI think 0 like INVALID


cAllObjects objects; //All objects

cAllObjects::cAllObjects()
{

}

cAllObjects::~cAllObjects()
{
}

void cAllObjects::clear()
{
	OBJECT_MAP::iterator iter( all.begin() );
	for( ; iter!=all.end(); iter++ ) {
		if( iter->second!=NULL )
			safedelete(iter->second);
	}
}

uint32_t cAllObjects::getNextCharSerial()
{
	return ++this->current_char_serial;
}

uint32_t cAllObjects::getNextItemSerial()
{
	return ++this->current_item_serial;
}

void cAllObjects::updateCharSerial( uint32_t ser )
{
	if( ser > this->current_char_serial )
		this->current_char_serial=ser;
}

void cAllObjects::updateItemSerial( uint32_t ser )
{
	if( ser > this->current_item_serial )
		this->current_item_serial=ser;
}

cAllObjectsIter::cAllObjectsIter(  )
{
}

cAllObjectsIter::~cAllObjectsIter()
{
}


void cAllObjectsIter::rewind()
{
	this->curr=objects.all.begin();
	this->next=this->curr;
	if ( this->next != objects.all.end() )
		this->next++;
}

bool cAllObjectsIter::IsEmpty()
{
	return ( curr==objects.all.end() );
}

pObject cAllObjectsIter::getObject()
{
	return curr->second;
}

uint32_t cAllObjectsIter::getSerial()
{
	return curr->first;
}


cAllObjectsIter& cAllObjectsIter::operator++(int)
{
	this->curr=this->next;
	if ( this->next != objects.all.end() )
		this->next++;
	return (*this);
}




