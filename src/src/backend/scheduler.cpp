/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

// Request limits
#define __STDC_LIMIT_MACROS

#include "logsystem.h"
#include "inlines.h"
#include "backend/scheduler.h"
#include "archs/tinterface.h"

#include <mxml.h>

cScheduler *cScheduler::scheduler = NULL;

void cScheduler::init()
{
	scheduler = new cScheduler();
	scheduler->start(false, true);
}

void cScheduler::close()
{
	scheduler->stop();
	safedelete(scheduler);
}

void cScheduler::restart()
{
	scheduler->stop();
	delete scheduler;
	scheduler = new cScheduler();
	scheduler->start(false, true);
}

/*!
\brief Constructor for new scheduled events
\param funcName name of the scripts' function to call
\param nInterval Interval to set for the events scheduling
\param parent Parent scheduler, the one to add the evnet to
\note This function deletes the newly-created instance if called with wrong
	parameters.
\note This function also set the minimum interval time for the scheduler to
	sleep.
*/
cScheduler::sEvent::sEvent(std::string funcName, uint32_t nInterval, cScheduler *parent)
{
	if ( funcName.length() == 0 || ! nInterval )
	{
		LogWarning("Trying to schedule a null event or an event with a null interval");
		delete this;
		return;
	}
	
	callback = cScriptingEngine::getEngine()->getFunction(funcName);
	if ( ! callback )
	{
		LogWarning("Unable to add function %s as a scheduled event", funcName.c_str());
		delete this;
		return;
	}
	
	interval = nInterval;
	lastrun = 0;
	
	parent->events.push_back(this);
	
	if ( parent->minInterval > interval )
		parent->minInterval = interval;
}

/*!
\brief Scheduler constructor
\\param in Input stream to read the XML datafile from

This functions loads the XML datafile from the given stream and creates all
the instances for the scheduled events to run.

\todo We open here the file, but we don't know exactly which is the program and
	the execution directory, so we should fix this adding a wrapper to the
	directories.
*/
cScheduler::cScheduler()
{
	outPlain("Loading scheduler...\t\t");
	
	std::ifstream xmlfile("config/scheduler.xml");
	if ( ! xmlfile )
	{
		outPlain("[ Failed ]\n");
		LogCritical("Unable to open schedule.xml file.");
		return;
	}
	
	minInterval = UINT32_MAX;
	try {
		MXML::Document doc(xmlfile);
		
		MXML::Node *n = doc.main()->child();
		do {
			if ( n->name() != "schedule" )
			{
				LogWarning("Unknown node %s in schedules.xml, ignoring", n->name().c_str() );
				continue;
			}
			
			try {
				new sEvent( n->data(), cVariant( n->getAttribute("interval") ).toUInt32(), this );
			} catch ( MXML::NotFoundError e ) {
				LogWarning("Incomplete node in schedules.xml, ignoring");
				continue;
			}
		} while((n = n->next()));
		
		outPlain("[   OK   ]\n");
	} catch ( MXML::MalformedError e) {
		outPlain("[ Failed ]\n");
		LogCritical("schedules.xml file not well formed.");
	}
}

cScheduler::~cScheduler()
{
	outPlain("Closing scheduler...");
	for(EventsList::iterator it = events.begin(); it != events.end(); it++)
		delete (*it);
	outPlain("\t\t[   OK   ]");
}

/*!
\brief Thread's run method

This metod is called when the thread is started, and it's actually the core of
the scheduler itself. Here the events are 
*/
void *cScheduler::run()
{
	while ( true )
	{
		Wefts::OSSleep(minInterval, 0);
		for(EventsList::iterator it = events.begin(); it != events.end(); it++)
			if ( getClockmSecs() > ( (*it)->lastrun + (*it)->interval ) )
				(*it)->callback->execute();
	}
}
