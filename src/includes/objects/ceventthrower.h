/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __CEVENTTHROWER_H__
#define __CEVENTTHROWER_H__

#include "common_libs.h"
#include "backend/scripting.h"

/*!
\class cEventThrower ceventthrower.h "objects/ceventthrower.h"
\brief Events-throwing object
\author Flameeyes (class skeleton and documentation)

This class is an abstraction of an object which throws events, like items,
chars, bodies, and so on. It provides the events vector and functions to
get/set triggers.

All the classes which inherits this, must implement the eventsNumber() function
with the number of events that class can throw, so the vector is dimensioned
at construction time.

\section whatis What are events?

Events are handles to scripted functions which are called when something occurs
to an event-thrower object.
For example there are several events for cBody, cChar and cItem classes.

Events are defined in a vector (cEventThrower::events) which contains pointers
to the handles to call a scripted function.

At the moment (2004-08-21) only one function can be attached to an event, but
this may change in the future.

\todo Allow to attach more than one function to an event
\see cScriptingEngine
*/
class cEventThrower {
protected:
	cEventThrower();
	cEventThrower(uint16_t n);
	FunctionVector events;	//!< Handlers to call for events
	virtual uint16_t eventsNumber() const = 0;
public:
	//!< Generic events for all event-throwing classes
	enum {
		evtOnStart,
		evtGenericMax
	};

	pFunctionHandle getEvent(uint16_t index);
	virtual ~cEventThrower() { }
};

#endif
