  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "nxwcommn.h"
#include "object.h"
#include "archive.h"
#include "chars.h"
#include "items.h"
#include "inlines.h"
#include "utils.h"
#include "scripts.h"

cScpIterator* cObject::getScriptIterator( std::string section, std::string& sectionId )
{
	cScpIterator*	iter	= 0;

	if ( section == "LOOTLIST" )
		iter = Scripts::Npc->getNewIterator( "SECTION %s %s", section.c_str(), sectionId.c_str() );
	else if ( section == "NPCLIST" )
		iter = Scripts::Npc->getNewIterator( "SECTION %s %s", section.c_str(), sectionId.c_str() );
	else if	( section == "RANDOMCOLOR" )
		iter = Scripts::Colors->getNewIterator( "SECTION %s %s", section.c_str(), sectionId.c_str() );
	else if	( section == "RANDOMNAME" )
		iter = Scripts::Npc->getNewIterator( "SECTION %s %s", section.c_str(), sectionId.c_str() );
	else if ( section == "UNDEADLIST" )
		iter = Scripts::Necro->getNewIterator( "SECTION %s %s", section.c_str(), sectionId.c_str() );
	return iter;
}

std::string cObject::getRandomScriptValue( std::string section, std::string& sectionId )
{
	std::string 	script1;
	int 		i	= 0,
			j	= 0;
	std::string	value;

	cScpIterator*	iter	= getScriptIterator( section, sectionId );
	if (iter == 0)
	{
		WarnOut("SECTION %s %s not found\n", section.c_str(), sectionId.c_str() );
	}
	else
	{
		int loopexit=0;
		do
		{
			script1 = iter->getEntry()->getFullLine();
			if ( script1[0]!='}' && script1[0]!='{' )
			{
				++i;
			}
		}
		while ( script1[0] !='}' && ++loopexit < MAXLOOPS );

		safedelete(iter);

		if(i>0)
		{
			i=rand()%i;
			iter = getScriptIterator( section, sectionId );
			if (iter == 0)
			{
				WarnOut("SECTION %s %s not found\n", section.c_str(), sectionId.c_str() );
			}
			else
			{
				loopexit=0;
				do
				{
					script1 = iter->getEntry()->getFullLine();
					if ( script1[0]!='}' && script1[0]!='{' )
					{
						if(j==i)
						{
							value = script1;
							// Wintermute stop going through the whole loop
							break;
						}
						++j;
					}
				}
				while ( script1[0]!='}' && ++loopexit < MAXLOOPS );
				safedelete(iter);
			}
		}
	}
	return value;
}

cObject::cObject()
{
	setCurrentName("#");
	setSecondaryName("#");
	serial.serial32 = INVALID;
	multi_serial.serial32 = INVALID;
	OwnerSerial.serial32 = INVALID;
	old_position = Loc(0,0,0);
	position = Loc(0,0,0);
	ScriptID = 0;

	color = color_old = 0;
	id = id_old = 0;

	//amxEvents = 0;
	tempfx = NULL;
	disabledmsg=NULL;
	disabled=0;
}

cObject::~cObject()
{
	/*if( amxEvents )
	{
		//AmxEventMap::iterator it( amxEvents->begin() ), end( amxEvents->end() );
		//for( ; it!=end; it++ )
		//{
		//	delete it->second;
		//}
		safedelete( amxEvents );
	}*/
	if ( tempfx ) {
		tempfx->clear();
		safedelete( tempfx );
	}
	if( disabledmsg!=NULL )
		safedelete( disabledmsg );
}


/*!
\brief Set the serial of the object
\author Anthalir
\since 0.82a
\param newserial the new serial (unsigned int)
*/
void cObject::setSerial32(SI32 newserial)
{
	serial.serial32= newserial;
	if( newserial!=INVALID )
		objects.insertObject( this );
}

void cObject::setOwnerSerial32(SI32 ownser, bool force)
{

  //Endymion
  //i think need to have only in cObejct the setOwnerOnly and have SetOwnerSerial in cItem and cChar

	if( !force )
		if ( ownser == getOwnerSerial32() ) return;

	if ( getOwnerSerial32() != INVALID ) // if it was set, remove the old one
		if ( isCharSerial( getSerial32() ) )
			pointers::delFromOwnerMap( (P_CHAR)( this ) );
		else
			pointers::delFromOwnerMap( (P_ITEM)( this ) );


	setOwnerSerial32Only(ownser);

	if ( getOwnerSerial32() == INVALID ) {
		if( isCharSerial( getSerial32() ) )
			((P_CHAR)(this))->tamed = false;
		return;
	}

	if ( isCharSerial( getSerial32() ) ) {
		if ( getOwnerSerial32() != getSerial32() )
			((P_CHAR)(this))->tamed = true;
		else
			((P_CHAR)(this))->tamed = false;
	}

	if ( isCharSerial( getSerial32() ) ) // if there is an owner, add it
		pointers::addToOwnerMap( (P_CHAR)( this ) );
	else
		pointers::addToOwnerMap( (P_ITEM)( this ) );
	//End Endymion..
}

/*!
\brief return one coord of the object position
\author Anthalir
\since 0.82a
\param what what to return ?
\return signed int
*/
SI32 cObject::getPosition( Coord what ) const
{
	switch( what )
	{
		case cX:
			return position.x;
		case cY:
			return position.y;
		case cZ:
			return position.z;
		case cDispZ:
			return position.dispz;
	}
}

/*!
\brief Set the position of the object
\author Anthalir
\since 0.82a
\param where Location structure representing the new position
*/
void cObject::setPosition(Location where)
{
        old_position = position; // Luxor
	position = where;
}

/*!
\brief Set one coord of the object position
\author Anthalir
\since 0.82a
\param what what to set ?
	\li "x" = set the x position
	\li "y" = set the y position
	\li "z" = set the z position
	\li "dz"= set the dispz position (used in cChar)
\param value the value to set
\todo change from string to a simpler enum
*/
void cObject::setPosition( const char *what, SI32 value)
{
        old_position = position; // Luxor
	switch( what )
	{
		case cX:
			position.x = value;
			return;
		case cY:
			position.y = value;
			return;
		case cZ:
			position.z = value;
			return;
		case cDispZ:
			position.dispz = value;
			return;
	}
}

SI32 cObject::getOldPosition( const char *what) const
{
	switch( what )
	{
		case cX:
			return old_position.x;
		case cY:
			return old_position.y;
		case cZ:
			return old_position.z;
		case cDispZ:
			return old_position.dispz;
	}
}

void cObject::setOldPosition( const char *what, SI32 value)
{
	switch( what )
	{
		case cX:
			old_position.x = value;
			return;
		case cY:
			old_position.y = value;
			return;
		case cZ:
			old_position.z = value;
			return;
		case cDispZ:
			old_position.dispz = value;
			return;
	}
}

/*!
\brief Set the current name of object
\author Anthalir, rewritten by Luxor
\since 0.82a
\param format formatted C string
*/
void cObject::setCurrentName( char *format, ... )
{
	char tmp[150];
	va_list vargs;
	va_start(vargs, format);
	vsnprintf(tmp, sizeof(tmp)-1, format, vargs);
	va_end(vargs);
	current_name=string( tmp );
}

/*!
\brief Set the secondary name of the object
\author Anthalir, rewritten by Luxor
\since 0.82a
\param format C char pointer - see printf
\warning This function must only be used by items because it use same var than real name
*/
void cObject::setSecondaryName(const char *format, ...)
{
	char tmp[150];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(tmp, sizeof(tmp)-1, format, vargs);
        va_end(vargs);

        tmp[sizeof(tmp)-1] = '\0';
	secondary_name = string(tmp);
}

/*!
\brief Tell if the AmxEvent id is valid
\author Luxor
\since 0.82
*//*
LOGICAL cObject::isValidAmxEvent( UI32 eventId )
{
	if ( eventId < 0 )
		return false;

	if ( isCharSerial(getSerial32()) && eventId >= ALLCHAREVENTS )
		return false;

	if ( isItemSerial(getSerial32()) && eventId >= ALLITEMEVENTS )
		return false;

	return true;
}

AmxEvent* cObject::getAmxEvent( UI32 eventId )
{
	if( isValidAmxEvent( eventId ) )
	{
		if( amxEvents != NULL )
		{
			AmxEventMap::iterator it = amxEvents->find( eventId );
			if( it != amxEvents->end() )
				return it->second;
		}
	}
	return NULL;
}

AmxEvent* cObject::setAmxEvent( UI32 eventId, char *amxFunction, LOGICAL dynamicEvent )
{
	AmxEvent* event = NULL;

	if( isValidAmxEvent( eventId ) )
	{
		delAmxEvent( eventId );

		if( amxEvents == NULL )
			amxEvents = new AmxEventMap;

		event = newAmxEvent( amxFunction, dynamicEvent );
		amxEvents->insert( make_pair( eventId, event ) );
	}

	return event;
}

cell cObject::runAmxEvent( UI32 eventID, SI32 param1, SI32 param2, SI32 param3, SI32 param4 )
{
	AmxEvent* event = getAmxEvent( eventID );

	g_bByPass = false;

	if( event != NULL )
		return event->Call( param1, param2, param3, param4 );

	return INVALID;
}

void cObject::delAmxEvent( UI32 eventId )
{
	if( isValidAmxEvent( eventId ) )
	{
		if( amxEvents )
		{
			AmxEventMap::iterator it = amxEvents->find( eventId );
			if( it != amxEvents->end() )
			{
				// we do not delete the actual AmxEvent instance as it's part of a vital hash queue in amxcback.cpp
				amxEvents->erase( it );
			}
			if( amxEvents->empty() )
				safedelete( amxEvents );
		}
	}
}
*/

/*!
\author Luxor
\brief Adds a temp effect to the object
*/
bool cObject::addTempfx( cObject& src, SI32 num, SI32 more1, SI32 more2, SI32 more3, SI32 dur, SI32 amxcback )
{
	if ( num < 0 || num >= tempfx::MAX_TEMPFX_INDEX )
		return false;

	//
	//	Repeatable check
	//
	if ( !tempfx::isDestRepeatable(num) && getTempfx( num ) )
			return false;

	if ( !tempfx::isSrcRepeatable(num) && src.getTempfx( num ) )
			return false;

	//
	//	Create the tempfx
	//
	tempfx::cTempfx tmpeff( src.getSerial32(), getSerial32(), num, dur, more1, more2, more3, amxcback );

	if ( !tmpeff.isValid() )
		return false;

        //
        //	Put the object in the global check vector if necessary
        //
        tempfx::addTempfxCheck( getSerial32() );

	//
	//	Start the tempfx
	//
	tmpeff.start();

	//
	//	Put it in the class vector
	//
	if ( tempfx == NULL )
		tempfx = new TempfxVector;

	tempfx->push_front( tmpeff );

	return true;
}

/*!
\author Luxor
\brief Deletes every tempfx of the specified number
*/
void cObject::delTempfx( SI32 num, LOGICAL executeExpireCode, SERIAL funcidx )
{
	if ( num < 0 || num >= tempfx::MAX_TEMPFX_INDEX )
		return;

	if ( !hasTempfx() )
		return;

	TempfxVector::iterator it( tempfx->begin() );
	for ( ; it != tempfx->end();  ) {
		if( ( it->getNum() != num ) || ( it->getAmxCallback() != funcidx ) ) {
			it++;
			continue;
		}

		if ( executeExpireCode )
			it->executeExpireCode();

		it = tempfx->erase( it );
	}

	if ( tempfx->empty() )
		safedelete( tempfx );
}

/*!
\author Luxor
\brief Activates the temp effects
*/
void cObject::tempfxOn()
{
	if ( !hasTempfx() )
		return;

	TempfxVector::iterator it( tempfx->begin() );
        for ( ; it != tempfx->end(); it++ ) {
                (*it).activate();
	}
}

/*!
\brief Deactivates the temp effects
\author Luxor
*/
void cObject::tempfxOff()
{
	if ( !hasTempfx() )
		return;

	TempfxVector::iterator it( tempfx->begin() );
        for ( ; it != tempfx->end(); it++ ) {
                (*it).deactivate();
        }
}

/*!
\author Luxor
*/
void cObject::checkTempfx()
{
	if ( !hasTempfx() )
		return;

	TempfxVector::iterator it( tempfx->begin() );
	for ( ; it != tempfx->end(); ) {
		SI08 result = it->checkForExpire();
		if ( result == 1 ) { // Tempfx has been executed
			it = tempfx->erase( it );
			continue;
		}
		else if ( result == INVALID ) // Tempfx has deleted the object!! Avoid Crashing!
			return;
		++it;
	}
}

/*!
\author Luxor
\brief Tells if the object has tempfx in queue
*/
LOGICAL cObject::hasTempfx()
{
	if ( tempfx == NULL )
		return false;

	if ( tempfx->empty() ) {
		safedelete( tempfx );
		return false;
	}

	return true;
}
/*!
\author Sparhawk
\brief Tells if the object has tempfx in queue
*/
void cObject::Delete()
{
}

/*!
\brief Get the tempfx from given num and funcidx
\author Luxor
*/
tempfx::cTempfx* cObject::getTempfx( SI32 num, SERIAL funcidx )
{
	if ( num < 0 || num >= tempfx::MAX_TEMPFX_INDEX )
		return NULL;

	if ( !hasTempfx() )
		return NULL;

	TempfxVector::iterator it( tempfx->begin() );
	for( ; it != tempfx->end(); it++ ) {
		if( ( it->getNum() == num ) && ( it->getAmxCallback() == funcidx ) )
			return &(*it);
	}

	return NULL;
}
