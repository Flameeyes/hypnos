/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __MAINLOOP_H__
#define __MAINLOOP_H__

#include "common_libs.h"

#include <wefts_thread.h>

class tInterface;

/*!
\brief Main Loop Thread

This thread is the main loop for the server. It handles the savegames, the
load/unload of the stuff, and the shutdown.

All the other threads requests services to this thread, using the private FIFO
for commands (actually using the functions which interacts with the FIFO.

Commands can have an optional parameter, which is passed to the actual thread
function.
*/
class tMainLoop : public Wefts::Thread
{
private:
	//! Commands sendable to the thread
	enum Command {
		cmdSave,	//!< Request for a savegame
		cmdResync,	//!< Request for a resync
		cmdShutdown,	//!< Request for a shutdown of the server
				//!< (param is the time before shutdown)
		cmdBroadcast	//!< Requested a broadcast to all clients
				//!< (param is the message to broadcast)
	};
	
	//! Parameterized commands sendable to the thread
	struct sCommand {
		Command cmd;	//!< Command to execute
		cVariant param;	//!< Optional parameter
		
		sCommand(Command aCmd, cVariant aParam)
			: cmd(aCmd), param(aParam)
		{ }
	};
	
	Wefts::Queue<sCommand> commands;
	
	void doSavegame();
	void doResync();
	void doBroadcast(std::string bcast);
public:
	tMainLoop(tInterface *output);
	
	void *run();
	
	void requestSave();
	void requestResync();
	void requestShutdown(uint16_t min);
	void broadcast(std::string bcast);
	
	static tMainLoop *instance;
};

/*!
\brief Already shuttign down exception

This exception is thrown when requesting a command to the main loop after a
shutdown request. All the commands after shutdown request are ignored, so be
sure when you request shutdown.

A savegame is done after the clients are disconnected, so there's no need
to request savegames after a shutdown request.
*/
class eShuttingDown : public eException
{
public:
	eShuttingDown();
};

#endif
