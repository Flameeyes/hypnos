  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "area.h"

cAreas*	Areas=NULL;

cAreas::cAreas() 
{
	currarea = UINVALID32;
}

cAreas::~cAreas() 
{
}

uint32_t cAreas::insert( Area& newarea, uint32_t index )
{
	AREA_DB::iterator iter( this->allareas.find( index ) );
	if( iter==this->allareas.end() && index != UINVALID32 ) {
		if( currarea<=index )
			currarea=index;
		allareas[index]=newarea;
		return index;
	}
	else {
		currarea++;
		allareas[currarea]=newarea;
		return currarea;
	}
}

void cAreas::loadareas()
{
}
