/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file 
\brief Declaration of class cEventThrower
*/

#ifndef __CEVENTTHROWER_H__
#define __CEVENTTHROWER_H__

#include "common_libs.h"
#include "backend/scripting.h"

/*!
\class cEventThrower ceventthrower.h "objects/ceventthrower.h"
\brief Events-throwing object

This class is an abstraction of an object which throws events, like items,
chars, bodies, and so on.
This class provides the events array and functions to get/set triggers.

All the classes which inherits this, must implement the eventsNumber() function
with the number of events that class can throw, so the vector is dimensioned
at construction time.
*/
class cEventThrower {
protected:
	cEventThrower();
	FunctionVector events;	//!< Handlers to call for events
	virtual uint16_t eventsNumber() const = 0;
public:
	pFunctionHandler getEvent(uint16_t index);
	virtual ~cEventThrower() { }
};

#endif