/*!
\mainpage Hypnos UO Server Emulator

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

\see \ref license for more information about the software license
\see \ref authors for information about the authors
\see http://noxwizard.sf.net/ for information about the NoX-Wizard Project

\section Manifesto

I'd like to write a manifesto stating my intentions about Hypnos' project.
Hypnos will never be the fastest UO emulator (such an emulator would have to
be written entirely in C++) nor the most scriptable (which may be Luxor's MyUO,
which supports packet forging and database access via script) but it has a
clear purpose in my opinions.

Hypnos must be simple. I want to create an emulator combining Sphere's
simplicity with NoX-Wizard's 'wannabe' power.

ID-defined objects (legacy from UOX/Wolfpack), numerical ID assigned to script
definitions must disappear. ".go" command and regions managed as in Sphere, and
so on. Unicorn (obviously reconfigured for Hypnos on release 1.0) is already
similar to Axis, and will be even more so when EVERY object will be
categorized, and for this I'll need much 'manpower'.

In addiction a default behaviour should be defined, suited for quick needs so,
like in Sphere, you won't go crazy trying to set up a minimal shard.

I'm displeased about the fact that removing non-id-scripted objects, tools like
Xuri's Worldbuilder won't work anymore, but we can live without it.

Another important feature will be independence from scripting language. I want
to use scripts written in different languages just changing a shared library
(.so in linux, .dll in windows) and configuration settings.

What I want won't be easily realized, and for most of the hard work I have to
thank Chrono and Ghisha, valid collegues which I hope to work with for a long
time, all of this wouldn't had be possible without them.

Also thanks to Judas for translating this text from italian to english :)

*/

#include "common_libs.h"
#include "sndpkg.h"
#include "sregions.h"
#include "version.h"
#include "calendar.h"
#include "magic.h"
#include "party.h"
#include "npcai.h"
#include "tmpeff.h"
#include "menu.h"
#include "telport.h"
#include "mount.h"
#include "ai.h"
#include "pointer.h"
#include "data.h"
#include "boats.h"
#include "area.h"
#include "spawn.h"
#include "trade.h"
#include "newbies.h"
#include "timing.h"
#include "inlines.h"
#include "map.h"
#include "hypnos.h"
#include "logsystem.h"
#include "backend/scheduler.h"
#include "extras/jails.h"
#include "extras/motd.h"
#include "extras/loginserver.h"
#include "objects/citem.h"
#include "objects/cchar.h"
#include "networking/remadmin.h"
#include "networking/network.h"
#include "skills/skills.h"

RemoteAdmin TelnetInterface;	//!< remote administration

nMULFiles::fTiledataLand *tiledataLand = NULL;
nMULFiles::fTiledataStatic *tiledataStatic = NULL;

void StartClasses()
{
	//! \todo this should be completely removed!
	outPlain("Initializing classes...");

	// Classes nulled now, lets get them set up :)
	cwmWorldState=new CWorldMain;
	mapRegions=new cRegion;

	Spawns=new cSpawns;
	Areas=new cAreas;
	Restocks= new cRestockMng();

	outPlain(" [ OK ]\n");
}

void DeleteClasses()
{
	//! \todo this should be completely removed!
	delete cwmWorldState;
	delete mapRegions;

	delete Network;
	delete Spawns;
	delete Areas;

	delete Restocks;
}

/*!
\brief initialise globals variables
\todo Remove this!
*/
void SetGlobalVars()
{
	outPlain("Initializing global variables...");

	w_anim[0]=0; w_anim[1]=0; w_anim[2]=0;

	save_counter=0;

	nettimeout.tv_sec=0;
	nettimeout.tv_usec=0;
	keeprun=true;
	error=0;
	now=0;
	secure=1;
	donpcupdate=0;
	wtype=0;

	outPlain(" [ OK ]\n");
}

/*!
\brief Loads the server settings and datafiles

This function loads various settings and datafiles calling the right functions
which then take the duty of load the XML files.
This function is currently called by the main() function only, but it can be
used to reload all the server's data.
*/
void loadServer()
{
#if 0
// Old load...
	loadserverdefaults();
	preloadSections("config/server.cfg");
	preloadSections("custom/server.cfg");

	//XAN : moved here 'cos nxw needs early initialization
	//(has vital data in server.cfg, needed for proper "bootstrap" :))
	loadserverscript();
#endif
	// old things
	commitserverscript(); // second phase setup
	SetGlobalVars();
	
	srand(getClockmSecs()); // initial randomization call
	
	// Load MULs
	nMULFiles::setMULpath("./muls"); //!\todo need to fix this
	tiledataStatic = new nMULFiles::fTiledataStatic();
	tiledataLand = new nMULFiles::fTiledataLand();
	
	// Load datafiles
	nMOTD::loadMOTD();
	nLoginServer::loadLoginServer();
	nNewbies::loadStartLocations();
	nNewbies::loadNewbieItems();
	nSkills::loadSkills();
	nJails::loadJails();
	cWeapon::loadWeaponsInfo();
	cContainer::loadContainersData();
	
	outPlain("Loading mountable creature...");
	loadmounts();
	outPlain("[  Ok  ]\n");

	outPlain("Loading Creature info...");
	creatures.load();
	outPlain("[  Ok  ]\n");

	outPlain("Building pointer arrays...");
	BuildPointerArray();
	outPlain("[  Ok  ]\n");

	outPlain("Loading accounts...");
	Accounts->LoadAccounts();
	outPlain("[  Ok  ]\n");

	outPlain("Loading areas...");
	Areas->loadareas();
	outPlain("[DONE]\n");

	outPlain("Loading spawn regions...");
	//loadspawnregions();
	Spawns->loadFromScript();
	outPlain("[DONE]\n");

	outPlain("Loading regions...");
	loadregions();
	outPlain("[DONE]\n");

	data::init(); // Luxor

	outPlain("Loading Teleport...");
	read_in_teleport();
	outPlain("[DONE]\n");

	npcs::initNpcSpells();

	TelnetInterface.Init();	// initialise remote admin interface

	cScheduler::init();

	outPlain(" [DONE]\nLoading custom titles...");
	loadcustomtitle();
	outPlain(" [DONE]\n");

	outPlain("Initializing creatures... ");
	creatures.load();
	outPlain("[DONE]\n");

	outPlain("Initializing magic... ");
	//Magic->InitSpells();
	magic::loadSpellsFromScript();
	outPlain("[DONE]\n");

	outPlain("Initializing races... ");
	Race::parse();
	outPlain("[DONE]\n");

	outPlain("Loading IP blocking rules... ");
	Network->LoadHosts_deny();
	outPlain("[DONE]\n");

	Guilds->CheckConsistancy(); // LB

	StartClasses();
	
	new tListening();
	tListening::instance->start();
}

/*!
\brief Shutdown the server

This function takes care of save server's volatile data into files and shut
it down, destroying the classes and cleaning up memory.
*/
void shutdownServer()
{
	cwmWorldState->saveNewWorld();

	sysbroadcast("The server is shutting down.");
	
	outPlain("Closing sockets...");
	tListening::instance->closeServer();
	tListening::instance->join();
	delete tListening::instance;
	outPlain(" Done.\n");
	
	outPlain("Saving server.cfg...\n");
	saveserverscript();
	outPlain("\n");
	outPlain("Deleting Classes...");
	DeleteClasses();
	data::shutdown(); // Luxor
	outPlain("[DONE]\n");
}
