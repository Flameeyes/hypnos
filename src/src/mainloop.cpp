/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "clock.h"
#include "dirs.h"
#include "hypnos.h"
#include "logsystem.h"
#include "mainloop.h"
#include "archs/tinterface.h"
#include "networking/tkiller.h"
#include "networking/tuoreceiver.h"
#include "networking/tracreceiver.h"

#ifdef HAVE_SIGNAL_H
#include "archs/signal.h"
#endif

#ifdef HAVE_FSTREAM
	#include <fstream>
#elif defined HAVE_FSTREAM_H
	#include <fstream.h>
#endif

#ifdef HAVE_SSTREAM
	#include <sstream>
	using std::ostringstream;
#elif HAVE_SSTREAM_H
	#include <sstream.h>
#endif

tMainLoop *tMainLoop::instance = NULL;

/*!
\brief Constructor

This constructor takes care of loading the server data, and starting all the
threads.
This replace the old LoadServer() function.
*/
tMainLoop::tMainLoop()
{
	// Here we should have already an instanced interface...
	assert(tInterface::instance);

#ifdef __unix__
	// Put the pid into the right file
	ofstream pid(nDirs::getPidFilePath());
	if ( pid )
	{
		pid << getpid();
	} else {
		ostringstream sout;
		sout << "Error writing PID to " << nDirs::getPidFilePath() << ". Is the directory writable?";
		LogWarning(sout.str().c_str());
	}
#endif
	
	srand(getClockmSecs()); // initial randomization call
	nSettings::load();
	
	// Load MULs
	nMULFiles::setMULpath(nDirs::getMulsDir());
	tiledataStatic = new nMULFiles::fTiledataStatic();
	tiledataLand = new nMULFiles::fTiledataLand();
	multiIDX = new nMULFiles::fMultiIDX();
	multiMUL = new nMULFiles::fMulti(multiIDX);
	
	// Load datafiles
	nMOTD::loadMOTD();
	nLoginServer::loadLoginServer();
	nNewbies::loadStartLocations();
	nNewbies::loadNewbieItems();
	nSkills::loadSkills();
	nJails::loadJails();
	cWeapon::loadWeaponsInfo();
	cContainer::loadContainersData();
	
	tInterface::outPlain("Loading mountable creature...");
	loadmounts();
	tInterface::outPlain("[  Ok  ]\n");

	tInterface::outPlain("Loading Creature info...");
	creatures.load();
	tInterface::outPlain("[  Ok  ]\n");

	tInterface::outPlain("Building pointer arrays...");
	BuildPointerArray();
	tInterface::outPlain("[  Ok  ]\n");

	tInterface::outPlain("Loading accounts...");
	Accounts->LoadAccounts();
	tInterface::outPlain("[  Ok  ]\n");

	tInterface::outPlain("Loading areas...");
	Areas->loadareas();
	tInterface::outPlain("[DONE]\n");

	tInterface::outPlain("Loading spawn regions...");
	//loadspawnregions();
	Spawns->loadFromScript();
	tInterface::outPlain("[DONE]\n");

	tInterface::outPlain("Loading regions...");
	loadregions();
	tInterface::outPlain("[DONE]\n");

	data::init(); // Luxor

	tInterface::outPlain("Loading Teleport...");
	read_in_teleport();
	tInterface::outPlain("[DONE]\n");

	npcs::initNpcSpells();

	tInterface::outPlain("Loading scheduler thread...");
	cScheduler::init();
	tInterface::outPlain("[  Ok  ]\n");

	tInterface::outPlain("Loading custom titles...");
	loadcustomtitle();
	tInterface::outPlain("[  Ok  ]\n");

	tInterface::outPlain("Initializing creatures... ");
	creatures.load();
	tInterface::outPlain("[  Ok  ]\n");

	tInterface::outPlain("Initializing magic... ");
	//Magic->InitSpells();
	magic::loadSpellsFromScript();
	tInterface::outPlain("[  Ok  ]\n");

	tInterface::outPlain("Initializing races... ");
	Race::parse();
	tInterface::outPlain("[  Ok  ]\n");

	tInterface::outPlain("Loading IP blocking rules... ");
	Network->LoadHosts_deny();
	tInterface::outPlain("[  Ok  ]\n");

	lowlevelOutput(stdout, "Clearing all trades...");
	clearalltrades();
	lowlevelOutput(stdout, " [DONE]\n");
	
	Guilds->CheckConsistancy(); // LB

	//! \todo this should be completely removed!
	tInterface::outPlain("Initializing classes...");

	// Classes nulled now, lets get them set up :)
	cwmWorldState=new CWorldMain;
	mapRegions=new cRegion;

	Spawns=new cSpawns;
	Areas=new cAreas;
	Restocks= new cRestockMng();

	tInterface::outPlain(" [ OK ]\n");
	
	tInterface::outPlain("Starting network threads...");
	new tUOListener(nSettings::Server::getLocalHostname(), nSettings::Server::getLocalPort());
	new tRemoteAdmin(nSettings::Server::getLocalHostname(), nSettings::Server::getRACPort());
	new tKiller();
	
	tUOListener::instance->start();
	tRemoteAdmin::instance->start();
	tKiller::instance->start();
	tInterface::outPlain("[  Ok  ]\n");
	
#ifdef HAVE_SIGNAL_H
	// Only if we have signal.h try to star the signal thread
	// Anyway, don't publish this
	new tSigHandler();
#endif

	tInterface::instance->start();
	
	start();
}

/*!
\brief Running function for the thread

This function does all the dirt work of the thread, waiting till a shutdown
request is received, and then closes up the server.
*/
void *tMainLoop::run()
{
	uint32_t shutdowntime = 0;
	sCommand comm;
	do {
		comm = commands.pull();
		
		switch(comm.cmd)
		{
		case cmdSave:
			doSavegame();
			break;
		case cmdShutdown:
			{
				shutdowntime = comm.param.toUInt32()*MINS;
				ostringstream sout;
				sout << "Server shutting down in " << comm.param.toUInt32() << "minutes"
				doBroadcast(sout.str());
				break;
			}
		case cmdResync:
			doResync();
			break;
		case cmdBroadcast:
			doBroadcast(comm.param);
			break;
		}
	} while( comm.cmd != cmdShutdown );
	
	Wefts::OSSleep(shutdowntime);
	
	sysbroadcast("The server is shutting down.");
	
	tInterface::outPlain("Closing listening sockets...");
	
	tUOListener::instance->closeServer();
	tUOListener::instance->join();
	delete tUOListener::instance;
	
	tRemoteAdmin::instance->closeServer();
	tRemoteAdmin::instance->join();
	delete tRemoteAdmin::instance;
	
	tInterface::outPlain("[  Ok  ]\n");
	
	doSavegame(false);

	tInterface::outPlain("Saving server.cfg...\n");
	saveserverscript();
	tInterface::outPlain("[  Ok  ]\n");
	
	tInterface::outPlain("Deleting Classes...");
	//! \todo this should be completely removed!
	delete cwmWorldState;
	delete mapRegions;

	delete Spawns;
	delete Areas;

	delete Restocks;
	
	data::shutdown(); // Luxor
	tInterface::outPlain("[  Ok  ]\n");
	
	tInterface::outPlain("Killing scheduler thread...");
	cScheduler::close();
	tInterface::outPlain("[  Ok  ]\n");
	
	tInterface::outPlain("Killing killer thread...");
	tKiller::instance->stop();
	tKiller::instance->join();
	delete tKiller::instance;
	tInterface::outPlain("[  Ok  ]\n");
	
	if (cLogFile::newErrorsLogged())
		outError("New errors have been logged. See the server log for details.\n");
	
	if (cLogFile::newWarningsLogged())
		outWarning("New warnings have been logged. See the server log for details.\n");
	
	tInterface::instance->stop();
	tInterface::instance->join();
	
	delete tInterface::instance;
	
	stop();
}
