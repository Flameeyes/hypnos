/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "objects/ceventthrower.h"
#include "logsystem.h"

/*!
\brief Default constructor
\param n Return of eventsNumber() function (dimension of events array)

This functions also initialize the events handle array to the given value.
*/
cEventThrower::cEventThrower(uint16_t n) :
	events(n)
{
}

/*!
\brief Gets the handler for the given event
\param index Event index, must be in the range of events of the derived class
\return The handler of the given event, or NULL if not present or index out of bound
*/
pFunctionHandle cEventThrower::getEvent(uint16_t index)
{
	if ( index >= eventsNumber() )
	{
		LogWarning("Event index %u out of bound (max: %u)", index, eventsNumber());
		return NULL;
	}
	
	return events[index];
}
