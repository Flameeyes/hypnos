/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "common_libs.h"
#include <wefts_thread.h>

/*!
\brief Scheduler
\author Flameeyes

This Schedule interface is derived from the NoX-Wizard's crontab system by
Xanathar.
The new interface with XML files follows only partially the original crontab
interface. All the events are now scripts' callbacks, and the resolution is
directly in seconds instead of the classical crontab-like interface.
The XML file which defines the events is schedules.xml. See for this the XML
datafile related documentation

\note The scheduler is run as a new thread, so we demand to the scripting
	engine the thread-safety of the scripts.
\note To reload the scheduler we should kill it and recreate it, reloading
	the XML file.
\note The schedule isn't really realtime, but it's triggers once in a time
	which is the minimum interval in the events list.
*/
class cScheduler : public Wefts::Thread
{
protected:
	//! Scheduled event
	class cEvent {
	public:
		pFunctionHandle callback;
			//!< Script function to call to run the schedule
		uint32_t interval;
			//!< Number of seconds to wait before the schedule
			//!< is run
		uint32_t lastrun;
			//!< Time at the call of the last run of the schedule
		
		sEvent(std::string funcName, uint32_t interval);
	};
	
	typedef std::list<sEvent*> EventsList;
				//!< List of scheduled events
	
	EventsList events;	//!< Scheduled events
	
	uint32_t minInterval;	//!< Minimum interval for the scheduled events
	
	static cScheduler *scheduler;
public:
	cScheduler(std::istream &in);
	void *run();
	
	static void init();
	static void close();
	static void restart();
};

#endif