/*!
\mainpage Hypnos documentation

<h1>Hypnos UO Server Emulator</h1>
<i>Work based on NoX-Wizard project.</i>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

<b>In addition to that license, if you are running this program or modified
versions of it on a public system you HAVE TO make the complete source of
the version used by you available or provide people with a location to
download it.</b>

You can find info about the authors in the AUTHORS file.

NoX-Wizard project can be found at <a href="http://noxwizard.sf.net/">
NoX-Wizard site</a>

<hr>

<h2>Manifesto</h2>

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
time, all of this woldn't had be possible without them.

Also thanks to Judas for translating this text from italian to english :)

*/

#include "common_libs.h"
#include "basics.h"
#include "sndpkg.h"
#include "sregions.h"
#include "remadmin.h"
#include "backend/scheduler.h"
#include "version.h"
#include "calendar.h"
#include "magic.h"
#include "races/race.h"
#include "party.h"
#include "npcai.h"
#include "network.h"
#include "tmpeff.h"
#include "menu.h"
#include "telport.h"
#include "mount.h"
#include "extras/jail.h"
#include "ai.h"
#include "worldmain.h"
#include "pointer.h"
#include "data.h"
#include "boats.h"
#include "area.h"
#include "spawn.h"
#include "trade.h"
#include "newbie.h"
#include "timing.h"
#include "inlines.h"
#include "basics.h"
#include "skills/skills.h"
#include "map.h"
#include "hypnos.h"
#include "logsystem.h"
#include "objects/citem.h"
#include "objects/cchar.h"


extern void initSignalHandlers();

extern void checkGarbageCollect(); //!< Remove items which were in deleted containers

bool g_bInMainCycle = false;
void LoadOverrides ();
extern "C" int g_nTraceMode;

RemoteAdmin TelnetInterface;	//!< remote administration

/*!
\todo clean and broke up
*/
static void item_char_test()
{
	LogMessage("Starting item consistancy check");
	ConOut("Starting item consistancy check... \n");


	cAllObjectsIter objs;
	for( objs.rewind(); !objs.IsEmpty(); objs++ )
	{
		uint32_t ser=objs.getSerial();

		if( isItemSerial( ser ) ) {

			pItem pi=(pItem)(objs.getObject() );

			if (pi->getSerial()==INVALID) {
				WarnOut("item %s [serial: %i] has invalid serial!",pi->getCurrentName().c_str(),pi->getSerial());
				LogWarning("ALERT ! item %s [serial: %i] has invalid serial!",pi->getCurrentName().c_str(),pi->getSerial());
			}

			// item is contained in himself
			if (pi == pi->getContainer())
			{
				WarnOut("item %s [serial: %i] has dangerous container value, autocorrecting",pi->getCurrentName().c_str(),pi->getSerial());
				LogWarning("ALERT ! item %s [serial: %i] has dangerous container value, autocorrecting",pi->getCurrentName().c_str(),pi->getSerial());
				pi->setContainer(0);
			}

			// item is owned by himself
			if (pi->getSerial() == pi->getOwnerSerial32())
			{
				WarnOut("item %s [serial: %i] has dangerous owner value",pi->getCurrentName().c_str(),pi->getSerial());
				LogWarning("ALERT ! item %s [serial: %i] has dangerous owner value",pi->getCurrentName().c_str(),pi->getSerial());
				pi->setOwnerSerial32(INVALID);
			}

		}
		else {
			pChar p_pet = (pChar)(objs.getObject());

			if (p_pet->isStabled())
			{
				pChar stablemaster=cSerializable::findCharBySerial(p_pet->getStablemaster());
				if (! stablemaster )
				{
					p_pet->unStable();
#ifdef SPAR_C_LOCATION_MAP
					pointers::addToLocationMap( p_pet );
#else
					mapRegions->add(p_pet);
#endif
					p_pet->timeused_last=getclock();
					p_pet->time_unused=0;
					LogMessage("Stabled animal got freed because stablemaster died");
					InfoOut("stabled animal got freed because stablemaster died\n");
				}
			 }
		}
	}
	ConOut("[DONE]\n");
}

extern "C" void breakOnFirstFuncz();

/*!
\brief facilitate console control. SysOp keys and localhost controls
\author ?
*/
void checkkey ()
{
	char c;
	int i,j=0;

	// Akron: borland c++ builder doesn't accept kbhit() in windows mode
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
			case 'B':
			case 'b':
				if (ServerScp::g_nLoadDebugger==0) {
					InfoOut("You must enable debugger starting Hypnos with the -debug option.\n");
					break;
				}
				breakOnFirstFuncz();
				InfoOut("Debug break scheduled for next amx call\n");
				break;
			case 'C':
			case 'c':
				if (ServerScp::g_nLoadDebugger==0) {
					InfoOut("You must enable debugger starting Hypnos with the -debug option.\n");
					break;
				}
				g_bStepInTriggers = true;
				InfoOut("Debug break scheduled for next trigger call\n");
				break;
			case 'N':
			case 'n':
				if (ServerScp::g_nLoadDebugger==0) {
					InfoOut("You must enable debugger starting Hypnos with the -debug option.\n");
					break;
				}
				g_nTraceMode=1-g_nTraceMode;
				InfoOut("Native tracing %s\n", (g_nTraceMode!=0) ? "enabled" : "disabled");
				break;
			case 'Q':
			case 'q':
				InfoOut("Immediate Shutdown initialized!\n");
				keeprun=false;
				break;
			case 'T':
			case 't':
				endtime=getclock()+(MY_CLOCKS_PER_SEC*60*2);
				endmessage(0);
				break;
			case '#':
				if ( !cwmWorldState->Saving() )
				{
					cwmWorldState->saveNewWorld();
				}
				break;
			case 'L':
			case 'l':
				if (g_nShowLayers)
				{
					InfoOut("Layer display disabled.\n");
					g_nShowLayers=false;
				}
				else
				{
					InfoOut("Layer display enabled.\n");
					g_nShowLayers=true;
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
				InfoOut("Command is disabled\n");
				break;
/*				ConOut("Hypnos: Reloading server.cfg, spawn.scp, and regions.scp...\n");
				loadspawnregions();
				loadregions();
				ConOut("Loading vital scripts... ");
				loadmetagm();
				loadmenuprivs();
				ConOut("[DONE]\n");
				loadserverscript();
				ConOut("Hypnos: Reloading IP Blocking rules...");
				Network->LoadHosts_deny();
				ConOut("[DONE]\n");
				break;
*/			case '?':
				ConOut("Console commands:\n");
				ConOut("	<Esc> or Q: Shutdown the server.\n");
				ConOut("	T - System Message: The server is shutting down in 2 minutes.\n");
				ConOut("	# - Save world\n");
				ConOut("	L - Toggle layer display");
				if (g_nShowLayers)
					ConOut(" [currently enabled]\n");

				else
					ConOut(" [currently disabled]\n");
				ConOut("	D - Disconnect Account 0\n");
				ConOut("	W - Display logged in characters\n");
				ConOut("	A - Reload accounts file\n");
				ConOut("	X - Reload XSS scripts\n");
				ConOut("	S - Toggle Secure mode ");
				if (secure)
					ConOut("[enabled]\n");
				else
					ConOut("[disabled]\n");
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

int main(int argc, char *argv[])
{
#define CIAO_IF_ERROR if (error==1) { Network->SockClose();  \
	DeleteClasses(); exit(INVALID); }

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

	if ((argc>1)&&(strstr(argv[1], "-debug")))	// activate debugger if requested
		ServerScp::g_nLoadDebugger = 1;

	serverstarttime=getclock();

	loadserverdefaults();
	preloadSections("config/server.cfg");
	preloadSections("custom/server.cfg");

	initConsole();

	ConOut("Starting Hypnos...\n\n");

	//XAN : moved here 'cos nxw needs early initialization
	//(has vital data in server.cfg, needed for proper "bootstrap" :))

	loadserverscript();

#ifdef __unix__
	//thx to punt and Plastique :]
	signal(SIGPIPE, SIG_IGN);
#endif

	if (ServerScp::g_nDeamonMode!=0) {
		ConOut("Going into deamon mode... bye...\n");
		init_deamon();
	}
	if (ServerScp::g_nDeamonMode!=0)	// if in daemon mode => disable debugger
		ServerScp::g_nLoadDebugger = 0;

	ConOut("Applying interface settings... ");
	constart();
	ConOut("[ OK ]\n");

	openings = 0;

	StartClasses();

	CIAO_IF_ERROR;

	commitserverscript(); // second phase setup
		
	ConOut("\nLoading scripts with new method...\n");
	newScriptsInit();


	//Now lets load the custom scripts, if they have them defined...
	i=0;

	CIAO_IF_ERROR; // errors here crashes further startup process. so stop insted of crash

	ConOut("\n");
	SetGlobalVars();

	nSkills::loadSkills();

	ConOut("Loading Weapon type...");
	loadweaponsinfo();
	ConOut("[  Ok  ]\n");

	ConOut("Loading mountable creature...");
	loadmounts();
	ConOut("[  Ok  ]\n");

	ConOut("Loading Creature info...");
	creatures.load();
	ConOut("[  Ok  ]\n");

	ConOut("Loading container...");
	loadcontainers();
	ConOut("[  Ok  ]\n");

	ConOut("Building pointer arrays...");
	BuildPointerArray();
	ConOut("[  Ok  ]\n");

	ConOut("Loading accounts...");
	Accounts->LoadAccounts();
	ConOut("[  Ok  ]\n");

	keeprun=(Network->kr); //LB. for some technical reasons global varaibles CANT be changed in constructors in c++.
	error=Network->faul; // i hope i can find a cleaner solution for that, but this works !!!
	// has to here and not at the cal cause it would get overriten later

	CIAO_IF_ERROR;

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

	cMsgBoard::MsgBoardMaintenance();

	CIAO_IF_ERROR;

	ConOut("\n");

	data::init(); // Luxor

	if (!keeprun) error = 1;
	CIAO_IF_ERROR

	ConOut("Loading Teleport...");
	read_in_teleport();
	ConOut("[DONE]\n");

	ConOut("Initializing random number seed... [ OK ]\n");

	srand(getclock()); // initial randomization call
	ConOut("Loading vital scripts... ");
	loadmetagm();
	ConOut("[DONE]\n");
//	ConOut("Loading Hypnos extensions...\n");

	npcs::initNpcSpells();

	TelnetInterface.Init();	// initialise remote admin interface

	initAmxEvents();
	LoadOverrides ();
	cScheduler::init();
	Calendar::loadCalendarScp();

	Translation::init_translation(); //belli marco

	serverstarttime=getclock();

	CIAO_IF_ERROR;

	ConOut("\n");
	cwmWorldState->loadNewWorld();

	CIAO_IF_ERROR; // LB prevents file corruption

	ConOut("Clearing all trades...");
	clearalltrades();
	ConOut(" [DONE]\n");

	//ndEndy.. Very laggy stuff, not very usefull, modify house db and boat db insted of search avery time
	//
	//Boats --Check the multi status of every item character at start up to get them set!
	//ConOut("Initializing multis...");
	//InitMultis();
	//ConOut(" [DONE]\n");
	//ConOut(" [FAIL]  <-!!! DISABLED FOR THIS DEBUG VERSION\n");
	//End Boats --^

	gcollect();

//	ConOut("Initializing glowing-items...");
//	start_glow();
//	ConOut(" [DONE]\n"); // Magius(CHE) (1)
	FD_ZERO(&conn);
	starttime=getclock();
	endtime=0;
	lclock=0;
	ConOut("Initializing Que System...");
	initque();

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
	item_char_test(); //LB
	Guilds->CheckConsistancy(); // LB


	ConOut("\n\n");

	clearscreen();

	ConOut("%s %s %s.%s [%s] compiled by %s\nProgrammed by: %s", PRODUCT, VER, VERNUMB, HOTFIX, OS, NAME, PROGRAMMERS);
	ConOut("\nBased on NoX-Wizard 20031228");
	ConOut("\nWeb-site : \n");
	ConOut("\n");
	ConOut("Copyright (C) 1997, 98 Marcus Rating (Cironian)\n\n");
	ConOut("This program is free software; you can redistribute it and/or modify\n");
	ConOut("it under the terms of the GNU General Public License as published by\n");
	ConOut("the Free Software Foundation; either version 2 of the License, or\n");
	ConOut("(at your option) any later version.\n\n");
	ConOut("Running on %s\n", getOSVersionString());

	
	if (SrvParms->server_log)
		ServerLog.Write("-=Server Startup=-\n=======================================================================\n");

	serverstarttime=getclock(); // dont remove, its absolutly necassairy that its 3 times in the startup sequence for several timing reasons.

	CIAO_IF_ERROR;

	ConOut("\nMap size : %dx%d", map_width, map_height);

	if ((map_width==768)&&(map_height==512)) ConOut(" [standard Britannia/Sosaria map size]\n");
	else if ((map_width==288)&&(map_height==200)) ConOut(" [standard Ilshenar map size]\n");
	else ConOut(" [custom map size]\n");

	if (ServerScp::g_nAutoDetectIP==1)  {
		ConOut("\nServer waiting connections on all interfaces at TCP port %i\n", g_nMainTCPPort);
	} else {
		ConOut("\nServer waiting connections at IP %s, TCP port %i\n", serv[0][1], g_nMainTCPPort);
	}
	//	ConOut("\nServer waiting connections on all interfaces at TCP port %s\n", serv[0][2]);

	// print allowed clients
	std::string t;
	std::vector<std::string>::const_iterator vis( clientsAllowed.begin() ), vis_end( clientsAllowed.end() );

	ConOut("\nAllowed clients : ");
	for ( ; vis != vis_end;  ++vis)
	{
	t = (*vis);  // a bit pervert to store c++ strings and operate with c strings, admitably

	  if ( t == "SERVER_DEFAULT" )
	  {
		  ConOut("%s : %s\n", t.c_str(), SUPPORTED_CLIENT);
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

	initSignalHandlers();


	if (ServerScp::g_nLoadDebugger) {
		SDbgOut("                         -=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
		SDbgOut("                         ||  RUNNING IN DEBUG MODE  ||\n");
		SDbgOut("                         -=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
	}

	pointers::init(); //Luxor


	checkGarbageCollect();

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
			CheckClientIdle=((SrvParms->inactivitytimeout/2)*MY_CLOCKS_PER_SEC)+getclock();

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
					char msg[3];
					msg[0]=0x53;
					msg[1]=0x07;
					Xsend(r, msg, 2);
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

	gcollect();		// cleanup before saving, especially items of deleted chars (Duke, 10.1.2001)

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



/*!
\brief Initialize  jails
\author Sabrewulf
\todo Rename it or better remove it
*/
void initque()
{
//Sabrewulf: scriptable jails locations
//jails are the locations 1-10

	int jailx, jaily, jailz;

	for (int loc = 1; loc <= 10; loc ++)
	{
		jailx = jaily = jailz =INVALID;

		location2xyz(loc, jailx, jaily, jailz);
		if (jailx < 0 || jaily < 0 || jailz < 0) //if location is not found
			prison::standardJailxyz (loc, jailx, jaily, jailz); //uses the standard jails

		prison::addCell( loc, jailx, jaily, jailz );
	}

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



void enlist(int s, int listnum) // listnum is stored in items morex
{
	pChar pc = cSerializable::findCharBySerial(currchar[s]);
	if ( ! pc ) return;

	int x,j;
//	char sect[512];
    cScpIterator* iter = NULL;
    char script1[1024];

	//sprintf(sect, "SECTION ITEMLIST %i", listnum);

	iter = Scripts::Items->getNewIterator("SECTION ITEMLIST %i", listnum);
    if (iter==NULL) return;

	int loopexit=0;
	do
	{
		strcpy(script1, iter->getEntry()->getFullLine().c_str());
		if ((script1[0]!='}')&&(script1[0]!='{'))
		{
			x=str2num(script1);
			pItem pj=item::CreateFromScript( x, pc->getBackpack() );
			if ( ! pj ) return;
			pj->Refresh();
		}
	}
	while((strcmp(script1,"}")) && (++loopexit < MAXLOOPS) );
    safedelete(iter);
}

/*!
\brief initialise globals variables
\todo Remove this!
*/
void SetGlobalVars()
{
	ConOut("Initializing global variables...");

	w_anim[0]=0; w_anim[1]=0; w_anim[2]=0;

	for (i=0; i>ALLSKILLS; i++) { strcpy(title[i].other, "old titles.scp error"); }

	save_counter=0;

	nettimeout.tv_sec=0;
	nettimeout.tv_usec=0;
	keeprun=true;
	error=0;
	now=0;
	secure=1;
	donpcupdate=0;
	wtype=0;

	g_nShowLayers=false;
	ConOut(" [ OK ]\n");
}

/*!
\brief initialise multis
\author unknown, modified by Luxor
\note Really slow because it uses the cAllObjectsIter, must change this soon.
*/
void InitMultis()
{
//	unsigned int i ; // unused variable

	cAllObjectsIter objs;
//	pChar pc; // unused variable
	for( objs.rewind(); !objs.IsEmpty(); objs++ )
	{
		if ( !isCharSerial(objs.getSerial()) ) continue;
		pChar pc_i = (pChar)(objs.getObject());
		if(!pc_i)
			continue;

		pItem multi=findmulti( pc_i->getPosition() );
		if( multi )
		{
			if (multi->type==117)
				pc_i->setMultiSerial(multi->getSerial());
			else
				pc_i->setMultiSerial32Only(INVALID);
		}
	}

	pItem pi;
	for( objs.rewind(); !objs.IsEmpty(); objs++ )
	{
	/*for (i=0;i<itemcount;i++)
	{*/
		if ( isCharSerial(objs.getSerial()) ) continue;
		pi=(pItem)(objs.getObject());
		if(!pi)
			continue;

		//Endymion modified from !pi->isInWorld() to pi->isInWorld()
		if (pi->isInWorld() && (pi->getSerial()!=INVALID))
		{
			pItem multi=findmulti( pi->getPosition() );
			if ( multi )
				if (multi->getSerial()!=pi->getSerial())
					pi->SetMultiSerial(multi->getSerial());
				else
					pi->setMultiSerial32Only(INVALID);
		}
	}
}

void StartClasses()
{
	ConOut("Initializing classes...");


	// Classes nulled now, lets get them set up :)
	cwmWorldState=new CWorldMain;
	mapRegions=new cRegion;
	Boats=new cBoat;
	Guilds=new cGuilds;

	Network=new cNetwork;
	//Respawn=new cRespawn;
	Spawns=new cSpawns;
	Areas=new cAreas;
	Restocks= new cRestockMng();

	ConOut(" [ OK ]\n");
}

void DeleteClasses()
{
	delete cwmWorldState;
	delete mapRegions;
	delete Boats;
	delete Guilds;

	delete Network;
	//delete Respawn;
	delete Spawns;
	delete Areas;

	delete Restocks;
	//objects.clear();
}
////////////////////////////
// garbage collection
////////////////////////////
static bool s_bGarbageCollect = false;			//! garbage must be checked ?

//! request garbage collecting
void gcollect()
{
	s_bGarbageCollect = true;
}

/*!
\brief remove all the item without any parents (not in any valid container)
*/
void checkGarbageCollect () // Remove items which were in deleted containers
{
	int removed, rtotal=0, corrected=0;
	int loopexit=0;
	bool first=true;

	if (!s_bGarbageCollect) return;
	s_bGarbageCollect = false;

	LogMessage("Performing Garbage Collection...");

	cAllObjectsIter objs;

	do
	{

		removed=0;

		for( objs.rewind(); !objs.IsEmpty(); objs++ )
		{

			if( isCharSerial( objs.getSerial() ) )
			{
				if( first ) {
					pChar pc=(pChar)(objs.getObject());
					if( pc->getOwnerSerial32()!=INVALID ) {
						pChar own=cSerializable::findCharBySerial( pc->getOwnerSerial32() );
						if(!own) {
							pc->setOwnerSerial32( INVALID );
							++corrected;
						}
					}
				}
			}
			else {

				pItem pi=(pItem)(objs.getObject());

				if( pi->isInWorld() )
					continue;

				// find the container if theres one.
				pChar pc_j= cSerializable::findCharBySerial(pi->getContSerial());
				pItem pi_j= cSerializable::findItemBySerial(pi->getContSerial());

				// if container serial is invalid
				if( ((pc_j==NULL) ) &&
					((pi_j==NULL) ) )
				{
					pi->Delete();
					++removed;
				}
			}
		}
		rtotal+=removed;
		first=false;
	} while (removed>0 && (++loopexit < MAXLOOPS) );

	if(rtotal>0)
	{
		LogMessage("Gargbage Collector removed %i items",rtotal);
		WarnOut("Gargbage Collector removed %i items\n",rtotal);
	}
	if(corrected>0)
	{
		LogMessage("Gargbage Collector corrected %i char",corrected);
		WarnOut("Gargbage Collector corrected %i char\n",corrected);
	}
}
