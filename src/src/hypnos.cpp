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

#ifdef USE_SIGNALS
#include "arch/signals.h"
#endif

bool g_bInMainCycle = false;
void LoadOverrides ();
extern "C" int g_nTraceMode;

RemoteAdmin TelnetInterface;	//!< remote administration

nMULFiles::fTiledataLand *tiledataLand = NULL;
nMULFiles::fTiledataStatic *tiledataStatic = NULL;

void StartClasses()
{
	//! \todo this should be completely removed!
	ConOut("Initializing classes...");

	// Classes nulled now, lets get them set up :)
	cwmWorldState=new CWorldMain;
	mapRegions=new cRegion;

	Network=new cNetwork;
	Spawns=new cSpawns;
	Areas=new cAreas;
	Restocks= new cRestockMng();

	ConOut(" [ OK ]\n");
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
\brief Loads the server settings and datafiles

This function loads various settings and datafiles calling the right functions
which then take the duty of load the XML files.
This function is currently called by the main() function only, but it can be
used to reload all the server's data.
*/
void loadServer()
{
	srand(getclock()); // initial randomization call
	
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
	
	ConOut("Loading mountable creature...");
	loadmounts();
	ConOut("[  Ok  ]\n");

	ConOut("Loading Creature info...");
	creatures.load();
	ConOut("[  Ok  ]\n");

	ConOut("Building pointer arrays...");
	BuildPointerArray();
	ConOut("[  Ok  ]\n");

	ConOut("Loading accounts...");
	Accounts->LoadAccounts();
	ConOut("[  Ok  ]\n");

	ConOut("Loading areas...");
	Areas->loadareas();
	ConOut("[DONE]\n");

	ConOut("Loading spawn regions...");
	//loadspawnregions();
	Spawns->loadFromScript();
	ConOut("[DONE]\n");

	ConOut("Loading regions...");
	loadregions();
	ConOut("[DONE]\n");

	data::init(); // Luxor

	ConOut("Loading Teleport...");
	read_in_teleport();
	ConOut("[DONE]\n");

	ConOut("Loading vital scripts... ");
	loadmetagm();
	ConOut("[DONE]\n");

	npcs::initNpcSpells();

	TelnetInterface.Init();	// initialise remote admin interface

	LoadOverrides();
	cScheduler::init();

	ConOut(" [DONE]\nLoading custom titles...");
	loadcustomtitle();
	ConOut(" [DONE]\n");

	ConOut("Initializing creatures... ");
	creatures.load();
	ConOut("[DONE]\n");

	ConOut("Initializing magic... ");
	//Magic->InitSpells();
	magic::loadSpellsFromScript();
	ConOut("[DONE]\n");

	ConOut("Initializing races... ");
	Race::parse();
	ConOut("[DONE]\n");

	ConOut("Loading IP blocking rules... ");
	Network->LoadHosts_deny();
	ConOut("[DONE]\n");

	Guilds->CheckConsistancy(); // LB

	StartClasses();
}

/*!
\brief facilitate console control. SysOp keys and localhost controls
*/
void checkkey ()
{
	char c;
	int i,j=0;

	// Flameeyes: borland c++ builder doesn't accept kbhit() in windows mode
	// Also to force only remote admin under unix, we can remove the kbhit() test

#if !defined __BORLANDC__ && !defined __unix__
	if ((kbhit())||(INKEY!='\0'))
#else
	if ( INKEY != '\0' )
#endif
	{
		if (INKEY!='\0') {
			c = toupper(INKEY);
			INKEY = '\0';
			secure = 0;
		}
	#if !defined __BORLANDC__ && !defined __unix__
		else {
			c = toupper(getch());
		}
	#endif

		if (c=='S')
		{
			if (secure)
			{
				InfoOut("Secure mode disabled. Press ? for a commands list.\n");
				secure=0;
				return;
			}
			else
			{
				InfoOut("Secure mode re-enabled.\n");
				secure=1;
				return;
			}
		} else {
			if (secure && c != '?')  //Allows help in secure mode.
			{
				InfoOut("Secure mode prevents keyboard commands! Press 'S' to disable.\n");
				return;
			}

			switch(c)
			{
			case '\x1B':
				keeprun=false;
				break;
			case 'Q':
			case 'q':
				InfoOut("Immediate Shutdown initialized!\n");
				keeprun=false;
				break;
			case 'T':
			case 't':
				endtime=getclock()+(SECS*60*2);
				endmessage(0);
				break;
			case '#':
				if ( !cwmWorldState->Saving() )
				{
					cwmWorldState->saveNewWorld();
				}
				break;
			case 'D':	// Disconnect account 0 (useful when client crashes)
			case 'd':
				{
					int found = 0;
					InfoOut( "Disconnecting account 0 players... ");
					for (i=0;i<now;i++)
						if (acctno[i]==0 && clientInfo[i]->ingame)
						{
							found++;
							Network->Disconnect(i);
						}
					if (found>0) ConOut( "[ OK ] (%d disconnected)\n", found);
					else ConOut( "[FAIL] (no account 0 players online)\n", found);
				}
				break;
			case 'H':
			case 'h':				// Enable/Disable heartbeat
				if (heartbeat)
					InfoOut("Hypnos: Heartbeat Disabled\n");
				else
					InfoOut("Hypnos: Heartbeat Enabled\n");
				heartbeat = !heartbeat;
				break;
			case 'W':
			case 'w':				// Display logged in chars
				ConOut("----------------------------------------------------------------\n");
				ConOut("Current Users in the World:\n");
				j = 0;  //Fix bug counting ppl online.
				for (i=0;i<now;i++)
				{
					pChar pc_i=cSerializable::findCharBySerial(currchar[i]);
					if(pc_i && clientInfo[i]->ingame) //Keeps NPC's from appearing on the list
					{
						ConOut("%i) %s [ %08x ]\n", j, pc_i->getCurrentName().c_str(), pc_i->getSerial());
						j++;
					}
				}
				ConOut("Total Users Online: %d\n", j);
				break;
			case 'A': //reload the accounts file
			case 'a':
				InfoOut("Reloading accounts file...");
				Accounts->LoadAccounts();
				ConOut("[DONE]\n");
				break;
			case 'x':
			case 'X':
				InfoOut("Reloading XSS scripts...\n");
				deleteNewScripts();
				newScriptsInit();
				ConOut("[DONE]\n");
				break;
			case 'r':
			case 'R':
				InfoOut("Hypnos: Total server reload!");
				//! \todo Need to freeze and unfreeze all the clients here for the resync
				loadServer();
				break;
			case '?':
				ConOut("Console commands:\n");
				ConOut("	<Esc> or Q: Shutdown the server.\n");
				ConOut("	T - System Message: The server is shutting down in 2 minutes.\n");
				ConOut("	# - Save world\n");
				ConOut("	D - Disconnect Account 0\n");
				ConOut("	W - Display logged in characters\n");
				ConOut("	A - Reload accounts file\n");
				ConOut("	X - Reload XSS scripts\n");
				ConOut("	S - Toggle Secure mode %s\n", secure ? "[enabled]" : "[disabled]" );
				ConOut("	B - Set breakpoint on first amx function [DEBUG MODE ONLY]\n");
				ConOut("	C - Set breakpoint on first legacy trigger [DEBUG MODE ONLY]\n\n");
				ConOut("	? - Commands list (this)\n");
				ConOut("End of commands list.\n");
				break;
			default:
				InfoOut("Key %c [%x] does not preform a function.\n",c,c);
				break;
			}
		}
	}
}

static inline void exitOnError(bool error)
{
	if ( ! error ) return;
	
	Network->SockClose();
	DeleteClasses();
	exit(-1);
}

int main(int argc, char *argv[])
{
	int i;
	unsigned long tempSecs;
	unsigned long tempMilli;
	unsigned long loopSecs;
	unsigned long loopMilli;
	unsigned long tempTime;
	unsigned int uiNextCheckConn=0;//CheckConnection rate - AntiChrist ( thnx to LB )
	unsigned long CheckClientIdle=0;
	int r;

	initclock() ;

	loadServer();

	serverstarttime=getclock();

#if 0
// Old load...
	loadserverdefaults();
	preloadSections("config/server.cfg");
	preloadSections("custom/server.cfg");

	initConsole();

	ConOut("Starting Hypnos...\n\n");

	//XAN : moved here 'cos nxw needs early initialization
	//(has vital data in server.cfg, needed for proper "bootstrap" :))
	loadserverscript();
#endif

#ifdef USE_SIGNALS
	//thx to punt and Plastique :]
	signal(SIGPIPE, SIG_IGN);
	initSignalHandlers();
#endif

	if (ServerScp::g_nDeamonMode!=0) {
		ConOut("Going into deamon mode... bye...\n");
		init_deamon();
	}

	ConOut("Applying interface settings... ");
	constart();
	ConOut("[ OK ]\n");

	openings = 0;

#if 0
// old things
	exitOnError(error);

	commitserverscript(); // second phase setup
		
	ConOut("\nLoading scripts with new method...\n");
	newScriptsInit();


	//Now lets load the custom scripts, if they have them defined...
	i=0;

	exitOnError(error); // errors here crashes further startup process. so stop insted of crash

	ConOut("\n");
	SetGlobalVars();
#endif
	

	keeprun=(Network->kr); //LB. for some technical reasons global varaibles CANT be changed in constructors in c++.
	error=Network->faul; // i hope i can find a cleaner solution for that, but this works !!!
	// has to here and not at the cal cause it would get overriten later

	serverstarttime=getclock();

	exitOnError(error);

	ConOut("\n");
	cwmWorldState->loadNewWorld();

	exitOnError(error); // LB prevents file corruption

	ConOut("Clearing all trades...");
	clearalltrades();
	ConOut(" [DONE]\n");

	FD_ZERO(&conn);
	starttime=getclock();
	endtime=0;
	lclock=0;

	ConOut("\n\n");

	clearscreen();

	ConOut("Hypnos %s [%s]\nProgrammed by: %s", strVersion, OS, strDevelopers);
	ConOut("\nBased on NoX-Wizard 20031228");
	ConOut("\nWeb-site : http://hypnos.berlios.de/\n");
	ConOut("\n");
	ConOut("Original copyright (C) 1997, 98 Marcus Rating (Cironian)\n\n");
	ConOut("This program is free software; you can redistribute it and/or modify\n");
	ConOut("it under the terms of the GNU General Public License as published by\n");
	ConOut("the Free Software Foundation; either version 2 of the License, or\n");
	ConOut("(at your option) any later version.\n\n");
	ConOut("Running on %s\n", getOSVersionString().c_str() );

	
	if (SrvParms->server_log)
		ServerLog.Write("-=Server Startup=-\n=======================================================================\n");

	serverstarttime=getclock(); // dont remove, its absolutly necassairy that its 3 times in the startup sequence for several timing reasons.

	exitOnError(error);

	ConOut("\nMap size : %dx%d", map_width, map_height);

	if ((map_width==768)&&(map_height==512))
		ConOut(" [standard Britannia/Sosaria map size]\n");
	else if ((map_width==288)&&(map_height==200))
		ConOut(" [standard Ilshenar map size]\n");
	else ConOut(" [custom map size]\n");

	if (ServerScp::g_nAutoDetectIP==1)  {
		ConOut("\nServer waiting connections on all interfaces at TCP port %i\n", g_nMainTCPPort);
	} else {
		ConOut("\nServer waiting connections at IP %s, TCP port %i\n", serv[0][1], g_nMainTCPPort);
	}

	// print allowed clients
	std::string t;
	std::vector<std::string>::const_iterator vis( clientsAllowed.begin() ), vis_end( clientsAllowed.end() );

	ConOut("\nAllowed clients : ");
	for ( ; vis != vis_end;  ++vis)
	{
		t = (*vis);  // a bit pervert to store c++ strings and operate with c strings, admitably

		if ( t == "SERVER_DEFAULT" )
		{
			ConOut("%s : %s\n", t.c_str(), strSupportedClient);
			break;
		}
		else if ( t == "ALL" )
		{
			ConOut("ALL\n");
			break;
		}

		ConOut("%s,", t.c_str());
	}
	ConOut("\n");
	
	pointers::init(); //Luxor

	InfoOut("Server started\n");

	Spawns->doSpawnAll();

	//OnStart
	AMXEXEC(AMXT_SPECIALS,0,0,AMX_AFTER);
	g_bInMainCycle = true;
	while (keeprun)
	{

		keeprun = (!pollCloseRequests());
		// Uncomment by Dupois July 18, 2000! see note above about InitKbThread()
		//g_MenuCollector.cleanup();
		checkkey();
		//OnLoop
		AMXEXEC(AMXT_SPECIALS,2,0,AMX_AFTER);

		switch(speed.nice)
		{
			case 0: break;	// very unnice - hog all cpu time
			case 1: if (now!=0) Sleep(10); else Sleep(100); break;
			case 2: Sleep(10); break;
			case 3: Sleep(40); break;// very nice
			case 4: if (now!=0) Sleep(10); else Sleep(400); break; // anti busy waiting
			case 5: if (now!=0) Sleep(40); else Sleep(500); break;

			default: Sleep(10); break;
		}


		if(loopTimeCount >= 1000)
		{
			loopTimeCount = 0;
			loopTime = 0;
		}
		loopTimeCount++;

		StartMilliTimer(loopSecs, loopMilli);
                //testAI();
		if(networkTimeCount >= 1000)
		{
			networkTimeCount = 0;
			networkTime = 0;
		}

		StartMilliTimer(tempSecs, tempMilli);

		if ( TIMEOUT( CheckClientIdle ) )
		{
			CheckClientIdle=((SrvParms->inactivitytimeout/2)*SECS)+getclock();

			for (r=0;r<now;r++)
			{
				pChar pc_r=cSerializable::findCharBySerial(currchar[r]);
				if(! pc_r )
					continue;
				if (!pc_r->IsGM()
					&& pc_r->clientidletime<getclock()
					&& clientInfo[r]->ingame
					)
				{
					ConOut("Player %s disconnected due to inactivity !\n", pc_r->getCurrentName().c_str());
					//sysmessage(r,"you have been idle for too long and have been disconnected!");
					nPackets::Sent::IdleWarning pk(0x7);
					client->sendPacket(&pk);
					Network->Disconnect(r);
				}

			}
		}
		if( TIMEOUT( uiNextCheckConn ) ) // Cut lag on CheckConn by not doing it EVERY loop.
		{
			Network->CheckConn();
			TelnetInterface.CheckConn();
			uiNextCheckConn = (unsigned int)( getclock() + 250 );
		}

		Network->CheckMessage();
		TelnetInterface.CheckInp();

		tempTime = CheckMilliTimer(tempSecs, tempMilli);

		networkTime += tempTime;
		networkTimeCount++;

		if(timerTimeCount >= 1000)
		{
			timerTimeCount = 0;
			timerTime = 0;
		}

		StartMilliTimer(tempSecs, tempMilli);

		checktimers();

		tempTime = CheckMilliTimer(tempSecs, tempMilli);
		timerTime += tempTime;
		timerTimeCount++;

		if(autoTimeCount >= 1000)
		{
			autoTimeCount = 0;
			autoTime = 0;
		}
		StartMilliTimer(tempSecs, tempMilli);

		checkauto();

		tempTime = CheckMilliTimer(tempSecs, tempMilli);
		autoTime += tempTime;
		autoTimeCount++;

		Network->ClearBuffers();
		tempTime = CheckMilliTimer(loopSecs, loopMilli);
		loopTime += tempTime;

	}

	cwmWorldState->saveNewWorld();

	sysbroadcast("The server is shutting down.");
	ConOut("Closing sockets...");

	Network->SockClose();

	ConOut(" Done.\n");
	ConOut("Saving server.cfg...\n");
	saveserverscript();
	ConOut("\n");
	ConOut("Deleting Classes...");
	DeleteClasses();
	data::shutdown(); // Luxor
	ConOut("[DONE]\n");

	if (NewErrorsLogged())
		ConOut("New ERRORS have been logged. Please send the logs/error*.log and logs/critical*.log files to the dev team !\n");
	if (NewWarningsLogged())
		ConOut("New WARNINGS have been logged. Probably scripting errors. See the logs/warnings*.log for details !\n");

	if (error) {
		ConOut("ERROR: Server terminated by error!\n");

		if (SrvParms->server_log)
			ServerLog.Write("Server Shutdown by Error!\n");

	} else {
		ConOut("Hypnos: Server shutdown complete!\n");
		if (SrvParms->server_log)
			ServerLog.Write("Server Shutdown!\n");

	}
	return 0;
}

void StoreItemRandomValue(pItem pi,int tmpreg)
{ // Function Created by Magius(CHE) for trade System

	int max=0,min=0;

	if ( ! pi ) return;

	if (pi->good<0) return;

	if (tmpreg<0)
	{
		pItem pio=pi->getOutMostCont();
		if (pio->isInWorld())
			tmpreg=calcRegionFromXY( pio->getPosition() );
		else
		{
			pChar pc=cSerializable::findCharBySerial(pio->getContSerial());
			if (!pc) return;
			tmpreg=calcRegionFromXY( pc->getPosition() );
		}
	}

	if (tmpreg<0 || tmpreg>255 || pi->good<0 || pi->good>255) return;

	min=region[tmpreg].goodrnd1[pi->good];
	max=region[tmpreg].goodrnd2[pi->good];

	if (max!=0 || min!=0)
		pi->rndvaluerate=(int) RandomNum(min,max);
}


void StartMilliTimer(unsigned long &Seconds, unsigned long &Milliseconds)
{
#if defined(__unix__)
	timeval t ;
	gettimeofday(&t,NULL) ;
	Seconds = t.tv_sec ;
	Milliseconds = t.tv_usec/1000 ;
#else

	timeb t;
	::ftime( &t );
	Seconds = t.time;
	Milliseconds = t.millitm;
#endif
}


unsigned long CheckMilliTimer(unsigned long &Seconds, unsigned long &Milliseconds)
{
	unsigned long newSec ;
	unsigned long newMill ;

	#ifdef __unix__
	timeval t ;
	gettimeofday(&t,NULL) ;
	newSec = t.tv_sec ;
	newMill = t.tv_usec/1000 ;

	#else
	struct timeb t;
	::ftime( &t );
	newSec = t.time ;
	newMill = t.millitm ;

	#endif
	return( 1000 * ( newSec - Seconds ) + ( newMill - Milliseconds ) );
}

/*!
\brief initialise globals variables
\todo Remove this!
*/
void SetGlobalVars()
{
	ConOut("Initializing global variables...");

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

	ConOut(" [ OK ]\n");
}
