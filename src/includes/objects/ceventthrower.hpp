/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __CEVENTTHROWER_H__
#define __CEVENTTHROWER_H__

#include "common_libs.hpp"
#include "backend/scripting.hpp"

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

\section howto How to throw an event?

You can throw an event (so call the registered function if present), both if
you are inside the class of the object of which you want to throw the event,
and if you are outside it.

In the first case you should check if there's a registered founction for the
event you want to throw and then execute it (after set the right parameters
to the function you are going to call):

\code	if ( events[evtChrOnWounded] ) {
		cVariantVector params = cVariantVector(3);
		params[0] = getSerial(); params[1] = amount;
		params[2] = serial_att;
		events[evtChrOnWounded]->setParams(params);
		cVariant ret = events[evtChrOnWounded]->execute();
		if ( events[evtChrOnWounded]->isBypassed() )
			return;
		amount = ret.toSInt();
	} \endcode

The parameters are passed to the called function using a cVariantVector (a 
vector of cVariant objects which can be nearly everything), which you should
create passing the number of parameters the function called has and then
filling it with the parameters.

The function also return a cVariant value which can be used for change the
behaviour of the thrower function. Another thing which can change the behaviour
of the caller is the \b bypass: when a script function bypass the call, the
caller can check the bypass state invoking
cScriptingEngine::cFunctionHandle::isBypassed() function, and then act as
requested (usually ending the function by return, or skipping the default code
behaviour).

In the second case (from outside the thrower class), you need to call the
getEvent() function to get the handle to the function to call:

\code	pFunctionHandle evt = src->getEvent(cChar::evtChrOnCastSpell);
	if ( evt )
	{
		cVariantVector params = cVariantVector(4);
		params[0] = src->getSerial(); params[1] = spellnumber;
		params[2] = src->spelltype; params[3] = INVALID;
		evt->setParams(params);
		evt->execute();
		if ( evt->isBypassed() )
			return;
	} \endcode

From the above example you should change also the \b src pointer with the one
of the instance you are throwing events from.

\note In the above case you should also use the scope specifier for the event 
index constant.

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
