/*!
\mainpage PyUO documentation

	<h1>PyUO Emulator</h1>
	UO Server Emulation Program
	<i>Work based on NoX-Wizard project.</i>

	This Project started on January 2004, by Kheru & Flameeyes

	Copyright 1997, 98 by Marcus Rating (Cironian)

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

<hr />

	You can find info about the authors in the AUTHORS file.

	NoX-Wizard project can be found at <a
	href="http://noxwizard.sf.net/">NoX-Wizard site</a>

*/

#include "common_libs.h"
#include "basics.h"
#include "cmdtable.h"
#include "speech.h"
#include "sndpkg.h"
#include "sregions.h"
#include "remadmin.h"
#include "srvparms.h"
#include "crontab.h"
#include "version.h"
#include "calendar.h"
#include "ntservice.h"
#include "trigger.h"
#include "magic.h"
#include "set.h"
#include "addmenu.h"
#include "race.h"
#include "party.h"
#include "npcai.h"
#include "network.h"
#include "tmpeff.h"
#include "layer.h"
#include "menu.h"
#include "telport.h"
#include "mount.h"
#include "accounts.h"
#include "jail.h"
#include "ai.h"
#include "itemid.h"
#include "worldmain.h"
#include "pointer.h"
#include "data.h"
#include "boats.h"
#include "scp_parser.h"
#include "area.h"
#include "spawn.h"
#include "archive.h"
#include "trade.h"
#include "newbie.h"
#include "sndpkg.h"
#include "titles.h"
#include "timing.h"

#include "inlines.h"
#include "basics.h"
#include "skills.h"
#include "containers.h"
#include "classes.h"
#include "map.h"
#include "scripts.h"
#include "pyuo.h"

#ifdef WIN32
	#include "archs/pywin32.h"
#elif defined (__unix__)
	#include "archs/pyunix.h"
#endif

extern void initSignalHandlers();

extern void checkGarbageCollect(); //!< Remove items which were in deleted containers

bool g_bInMainCycle = false;
void LoadOverrides ();
extern "C" int g_nTraceMode;

#include "debug.h"

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
				pChar stablemaster=pointers::findCharBySerial(p_pet->getStablemaster());
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



//taken from 6904t2(5/10/99) - AntiChrist
/*!
\todo backport into cChar
*/
void callguards( CHARACTER p )
{
	pChar	caller = MAKE_CHAR_REF( p );

	if ( !caller )
		return;

	if( !(region[caller->region].priv&0x01 ) || !SrvParms->guardsactive || !TIMEOUT( caller->antiguardstimer ) || caller->dead )
		return;

	caller->antiguardstimer=uiCurrentTime+(MY_CLOCKS_PER_SEC*10);

	/*
	Sparhawk:	1. when instant guard is set and offender nearby caller spawn guard near caller and leave attacking to checkAI
			2. when instant guard is not set and offender nearby caller walk toward caller and leave attacking to checkAI
	*/
	bool offenders = false;
	vector < pChar > guards;

//	int loopexit=0; // unused variable

	NxwCharWrapper sc;
	sc.fillCharsNearXYZ( caller->getPosition(), VISRANGE, true, false  );
	for( sc.rewind(); !sc.isEmpty(); sc++ ) {

		pChar character=sc.getChar();
		if(! character )
			continue;
		if( caller->getSerial() != character->getSerial32() && caller->distFrom( character )  <= 15 && !character->dead && !character->IsHidden())
		{
			if ((!character->IsInnocent() || character->npcaitype == NPCAI_EVIL) && !character->IsHidden() )
				offenders = true;
			else
				if ((character->npcaitype == NPCAI_TELEPORTGUARD || character->npcaitype == NPCAI_GUARD) && !character->war && character->npcWander != WANDER_FOLLOW)
					guards.push_back( character );
		}
	}
	if ( offenders )
	{
		if ( guards.empty() && nSettings::Server::hasInstantGuards() )
		{
			pNPC guard = npcs::AddNPCxyz( caller->getSocket(), region[caller->region].guardnum[(rand()%10)+1], caller->getPosition());

			if ( guard )
			{
				guard->npcaitype=NPCAI_TELEPORTGUARD;
				guard->npcWander=WANDER_FREELY_CIRCLE;
				guard->setNpcMoveTime();
				guard->summontimer = uiCurrentTime + MY_CLOCKS_PER_SEC * 25 ;

				guard->playSFX( 0x01FE );
				staticeffect( DEREF_pChar( guard ), 0x37, 0x2A, 0x09, 0x06);

				guard->teleport();
				guard->talkAll( TRANSLATE("Don't fear, help is near"), 0 );
			}
		}
		else
		{
			pChar guard;
			while( !guards.empty() )
			{
				guard = guards.back();
				guard->oldnpcWander = guard->npcWander;
				guard->npcWander = WANDER_FOLLOW;
				guard->ftargserial = caller->getSerial();
				guard->antiguardstimer=uiCurrentTime+(MY_CLOCKS_PER_SEC*10); // Sparhawk this should become server configurable
				guard->talkAll( TRANSLATE("Don't fear, help is on the way"), 0 );
				//guard->antispamtimer = uiCurrentTime+MY_CLOCKS_PER_SEC*5;
				guards.pop_back();
			}
		}
	}
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
					InfoOut("You must enable debugger starting PyUo with the -debug option.\n");
					break;
				}
				breakOnFirstFuncz();
				InfoOut("Debug break scheduled for next amx call\n");
				break;
			case 'C':
			case 'c':
				if (ServerScp::g_nLoadDebugger==0) {
					InfoOut("You must enable debugger starting PyUO with the -debug option.\n");
					break;
				}
				g_bStepInTriggers = true;
				InfoOut("Debug break scheduled for next trigger call\n");
				break;
			case 'N':
			case 'n':
				if (ServerScp::g_nLoadDebugger==0) {
					InfoOut("You must enable debugger starting PyUO with the -debug option.\n");
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
				endtime=uiCurrentTime+(MY_CLOCKS_PER_SEC*60*2);
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
					InfoOut("PyUO: Heartbeat Disabled\n");
				else
					InfoOut("PyUO: Heartbeat Enabled\n");
				heartbeat = !heartbeat;
				break;
			case 'P':
			case 'p':				// Display profiling information
				ConOut("----------------------------------------------------------------\n");
				ConOut("Performace Dump:\n");
				ConOut("Network code: %fmsec [%i samples]\n" , (float)((float)networkTime/(float)networkTimeCount) , networkTimeCount);
				ConOut("Timer code: %fmsec [%i samples]\n" , (float)((float)timerTime/(float)timerTimeCount) , timerTimeCount);
				ConOut("Auto code: %fmsec [%i samples]\n" , (float)((float)autoTime/(float)autoTimeCount) , autoTimeCount);
				ConOut("Loop Time: %fmsec [%i samples]\n" , (float)((float)loopTime/(float)loopTimeCount) , loopTimeCount);
				//ConOut("Characters: %i (Dynamic)		Items: %i (Dynamic)\n" , charcount , char_mem::cmem , itemcount , item_mem::imem);
				ConOut("Simulation Cycles: %f per sec\n" , (1000.0*(1.0/(float)((float)loopTime/(float)loopTimeCount))));
				break;
			case 'W':
			case 'w':				// Display logged in chars
				ConOut("----------------------------------------------------------------\n");
				ConOut("Current Users in the World:\n");
				j = 0;  //Fix bug counting ppl online.
				for (i=0;i<now;i++)
				{
					pChar pc_i=MAKE_CHAR_REF(currchar[i]);
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
/*				ConOut("PyUO: Reloading server.cfg, spawn.scp, and regions.scp...\n");
				loadspawnregions();
				loadregions();
				ConOut("Loading vital scripts... ");
				loadmetagm();
				loadmenuprivs();
				ConOut("[DONE]\n");
				loadserverscript();
				ConOut("PyUO: Reloading IP Blocking rules...");
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
				ConOut("	P - Preformance Dump\n");
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
#ifndef _WINDOWS
				ConOut("	N - Toggle tracing of native functions [DEBUG MODE ONLY]\n");
#endif
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
	if ((argc>1)&&(strstr(argv[1], "-check")))	// activate check if requested
		ServerScp::g_nCheckBySmall = 1;

	uiCurrentTime=getclock();
	serverstarttime=getclock();

	loadserverdefaults();
	preloadSections("config/server.cfg");
	preloadSections("custom/server.cfg");

	initConsole();

	ConOut("Starting PyUO...\n\n");

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
/*
	if ( (argc>1) && (strstr(argv[1], "-syra")) )
	{

	}
	else
*/
	{
		ConOut("\nLoading scripts with new method...\n");
		newScriptsInit();


		//Now lets load the custom scripts, if they have them defined...
		i=0;

		CIAO_IF_ERROR; // errors here crashes further startup process. so stop insted of crash

		ConOut("\n");
		SetGlobalVars();

		SkillVars();	// Set Creator Variables

		ConOut("Loading skills...");
		loadskills();
		ConOut("[DONE]\n");

		ConOut("Loading Weapon type...");
		loadweaponsinfo();
		ConOut("[DONE]\n");

		ConOut("Loading mountable creature...");
		loadmounts();
		ConOut("[DONE]\n");

		ConOut("Loading Creature info...");
		creatures.load();
		ConOut("[DONE]\n");

		ConOut("Loading container...");
		loadcontainers();
		ConOut("[DONE]\n");

		ConOut("Building pointer arrays...");
		BuildPointerArray();
		ConOut("[DONE]\n");

		ConOut("Loading accounts...");
		Accounts->LoadAccounts();
		ConOut("[DONE]\n");

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
	}

	data::init(); // Luxor


	if (!keeprun) error = 1;
	CIAO_IF_ERROR

	ConOut("Loading Teleport...");
	read_in_teleport();
	ConOut("[DONE]\n");

	ConOut("Initializing random number seed... [ OK ]\n");

	srand(uiCurrentTime); // initial randomization call
	ConOut("Loading vital scripts... ");
	loadmetagm();
	ConOut("[DONE]\n");
//	ConOut("Loading PyUO extensions...\n");

	npcs::initNpcSpells();

	TelnetInterface.Init();	// initialise remote admin interface

	initAmxEvents();
	LoadOverrides ();
	initCronTab();
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
	starttime=uiCurrentTime;
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


	ConOut("\n");


	ConOut("\n");


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

////////////////////////////////////////////////////////////////////////////////////////////////////
// Removed by Dupois July 18, 2000!
// - Crashes server during if '#' - Save World option is used and corrupts the world file
//	quite nicely I might add. safer to just use the old checkkey() call in the main's while loop.
//InitKbThread();
////////////////////////////////////////////////////////////////////////////////////////////////////

	//ConOut("PyUO: Startup Complete.\n\n");


	if (SrvParms->server_log)
		ServerLog.Write("-=Server Startup=-\n=======================================================================\n");

	uiCurrentTime=getclock();
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

	if( ServerScp::g_nCheckBySmall ) {
		InfoOut("Check of all object started...");

		AmxFunction checkItems( "__check_Item" );
		AmxFunction checkNpcs( "__check_Npc" );
		AmxFunction checkPlayers( "__check_Player" );

		cAllObjectsIter objs;
		pChar pc = NULL;
		pItem pi = NULL;
		for( objs.rewind(); !objs.IsEmpty(); objs++ ) {
			if ( isCharSerial( objs.getSerial() ) && ( pc=static_cast<pChar>(objs.getObject())) ) {
				if( pc->npc )
					checkNpcs.Call( pc->getSerial() );
				else
					checkPlayers.Call( pc->getSerial() );
			}
			else if ( isItemSerial( objs.getSerial() ) && ( pi=static_cast<pItem>(objs.getObject())) ) {
				checkItems.Call( pi->getSerial() );
			}
		}

		ConOut("[DONE]\n");
		ServerScp::g_nCheckBySmall = false;

		cwmWorldState->saveNewWorld();

		ConOut("Closing sockets...");
		Network->SockClose();
		ConOut("Saving server.cfg...\n");
		saveserverscript();
		ConOut("[DONE]\n");
		ConOut("Deleting Classes...");
		DeleteClasses();
		ConOut("[DONE]\n");
		return 0;
	}

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
		checkCronTab();
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
			CheckClientIdle=((SrvParms->inactivitytimeout/2)*MY_CLOCKS_PER_SEC)+uiCurrentTime;

			for (r=0;r<now;r++)
			{
				pChar pc_r=MAKE_CHAR_REF(currchar[r]);
				if(! pc_r )
					continue;
				if (!pc_r->IsGM()
					&& pc_r->clientidletime<uiCurrentTime
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
			uiNextCheckConn = (unsigned int)( uiCurrentTime + 250 );
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

		uiCurrentTime=getclock();//getclock() only once
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

	sysbroadcast(TRANSLATE("The server is shutting down."));
	ConOut("Closing sockets...");

	Network->SockClose();

	gcollect();		// cleanup before saving, especially items of deleted chars (Duke, 10.1.2001)

	ConOut(" Done.\n");
	ConOut("Saving server.cfg...\n");
	//saveserverscript(1);
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
			ServerLog.Write("Server Shutdown by Error!\n=======================================================================\n\n\n");

	} else {
		ConOut("PyUO: Server shutdown complete!\n");
		if (SrvParms->server_log)
			ServerLog.Write("Server Shutdown!\n=======================================================================\n\n\n");

	}
	return 0;
}

void telltime( NXWCLIENT ps )
{

	NXWSOCKET s = ps->toInt();

	char tstring[60];
	char tstring2[60];
	int hour = Calendar::g_nHour % 12;
	if (hour==0) hour = 12;
	int ampm = (Calendar::g_nHour>=12) ? 1 : 0;
	int minute = Calendar::g_nMinute;
	int lhour;
	lhour=hour;

	if ((minute>=0)&&(minute<=14)) strcpy(tstring,TRANSLATE("It is"));
	else if ((minute>=15)&&(minute<=30)) strcpy(tstring,TRANSLATE("It is a quarter past"));
	else if ((minute>=30)&&(minute<=45)) strcpy(tstring,TRANSLATE("It is half past"));
	else
	{
		strcpy(tstring,TRANSLATE("It is a quarter till"));
		lhour++;
		if (lhour==0) lhour=12;
	}
	switch( lhour )
	{
	case 1: sprintf( tstring2, TRANSLATE("%s one o'clock"), tstring );	   break;
	case 2: sprintf( tstring2, TRANSLATE("%s two o'clock"), tstring );	   break;
	case 3: sprintf( tstring2, TRANSLATE("%s three o'clock"), tstring );	   break;
	case 4: sprintf( tstring2, TRANSLATE("%s four o'clock"), tstring );	   break;
	case 5: sprintf( tstring2, TRANSLATE("%s five o'clock"), tstring );	   break;
	case 6: sprintf( tstring2, TRANSLATE("%s six o'clock"), tstring );	   break;
	case 7: sprintf( tstring2, TRANSLATE("%s seven o'clock"), tstring );	   break;
	case 8: sprintf( tstring2, TRANSLATE("%s eight o'clock"), tstring );	   break;
	case 9: sprintf( tstring2, TRANSLATE("%s nine o'clock"), tstring );	   break;
	case 10: sprintf( tstring2, TRANSLATE("%s ten o'clock"), tstring );	   break;
	case 11: sprintf( tstring2, TRANSLATE("%s eleven o'clock"), tstring );	   break;
	case 12:
		if( ampm )
			sprintf( tstring2, TRANSLATE("%s midnight."), tstring );
		else
			sprintf( tstring2, TRANSLATE("%s noon."), tstring );
		break;
	}

	if (lhour==12) strcpy(tstring, tstring2);
	else if (ampm)
	{
		if ((lhour>=1)&&(lhour<6)) sprintf(tstring,TRANSLATE("%s in the afternoon."),tstring2);
		else if ((lhour>=6)&&(lhour<9)) sprintf(tstring,TRANSLATE("%s in the evening."),tstring2);
		else sprintf(tstring,TRANSLATE("%s at night."),tstring2);
	}
	else
	{
		if ((lhour>=1)&&(lhour<5)) sprintf(tstring,TRANSLATE("%s at night."),tstring2);
		else sprintf(tstring,TRANSLATE("%s in the morning."),tstring2);
	}

	sysmessage(s,tstring);
}

/*!
\brief direction from character a to position x,y
\author ?
\return int direction
\param a first character
\param x X-coordinate
\param y Y-coordinate
*/
int chardirxyz(int a, int x, int y)
{
	pChar pc = MAKE_CHAR_REF( a );
	if ( ! pc ) return INVALID;

	int dir,xdif,ydif;

	xdif = x - pc->getPosition().x;
	ydif = y - pc->getPosition().y;

	if ((xdif==0)&&(ydif<0)) dir=0;
	else if ((xdif>0)&&(ydif<0)) dir=1;
	else if ((xdif>0)&&(ydif==0)) dir=2;
	else if ((xdif>0)&&(ydif>0)) dir=3;
	else if ((xdif==0)&&(ydif>0)) dir=4;
	else if ((xdif<0)&&(ydif>0)) dir=5;
	else if ((xdif<0)&&(ydif==0)) dir=6;
	else if ((xdif<0)&&(ydif<0)) dir=7;
	else dir=INVALID;

	return dir;
}


int fielddir(CHARACTER s, int x, int y, int z)
{
//WARNING: unreferenced formal parameter z

	pChar pc=MAKE_CHAR_REF(s);
	if ( ! pc ) return 0;

	int dir=chardirxyz(s, x, y);
	switch (dir)
	{
	case 0:
	case 4:
		return 0;
	case 2:
	case 6:
		return 1;
	case 1:
	case 3:
	case 5:
	case 7:
	case INVALID:
		switch(pc->dir) //crashfix, LB
		{
		case 0:
		case 4:
			return 0;

		case 2:
		case 6:
			return 1;

		case 1:
		case 3:
		case 5:
		case 7:
			return 1;

		default:
			LogError("Switch fallout. pyuo.cpp, fielddir()\n"); //Morrolan
			return 0;
		}
	default:
		LogError("Switch fallout. pyuo.cpp, fielddir()\n"); //Morrolan
		return 0;
	}
}

/*!
\brief makes an npc attacking someone
\author Luxor
\param pc the npc attacker
\param pc_target the victim
*/
void npcattacktarget(pChar pc, pChar pc_target)
{
	if (
		! pc || ! pc_target ||
		! pc->npc ||
		pc->isDead() || pc_target->isDead() ||
		pc->getSerial() == pc_target->getSerial() ||
		!pc->losFrom(pc_target)
	) return;
	
	if( pc->amxevents[ EVENT_CHR_ONBEGINATTACK ]!=NULL ) {
		pc->amxevents[ EVENT_CHR_ONBEGINATTACK ]->Call( pc->getSerial(), pc_target->getSerial32() );
		if (g_bByPass==true)
			return;
	}
	/*
	pc->runAmxEvent( EVENT_CHR_ONBEGINATTACK, pc->getSerial(), pc_target->getSerial32() );
	if (g_bByPass==true)
		return;
	*/
	if( pc->amxevents[ EVENT_CHR_ONBEGINDEFENSE ]!=NULL ) {
		pc->amxevents[ EVENT_CHR_ONBEGINDEFENSE ]->Call( pc_target->getSerial(), pc->getSerial32() );
		if (g_bByPass==true)
			return;
	}
	/*
	pc->runAmxEvent( EVENT_CHR_ONBEGINDEFENSE, pc_target->getSerial(), pc->getSerial32() );
	if (g_bByPass==true)
		return;
	*/

	pc->playMonsterSound(SND_STARTATTACK);

	pc->targserial = pc_target->getSerial();
	pc->attackerserial = pc_target->getSerial();
	pc->SetAttackFirst();

	if ( !pc->war )
		pc->toggleCombat();
	pc->setNpcMoveTime();

	pChar pc_target_targ = pointers::findCharBySerial(pc_target->targserial);
	if ( !pc_target_targ || pc_target_targ->dead || pc_target->distFrom(pc_target_targ) > 15 ) {
		if (!pc_target->npc && pc_target->war) {
			pc_target->targserial = pc->getSerial();
			pc_target->attackerserial = pc->getSerial();
		} else if (pc_target->npc) {
			if ( !pc_target->war )
				pc_target->toggleCombat();

			pc_target->targserial = pc->getSerial();
			pc_target->attackerserial = pc->getSerial();
			pc_target->setNpcMoveTime();
		}
	}

	pc->emoteall( "You see %s attacking %s!", 1, pc->getCurrentName().c_str(), pc_target->getCurrentName().c_str() );

	return;
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

void usepotion(pChar pc, pItem pi)
{
	int x;

	if ( ! pc ) return;

	NXWSOCKET s = pc->getSocket();

	switch(pi->morey)
	{
	case 1: // Agility Potion
		staticeffect(DEREF_pChar(pc), 0x37, 0x3a, 0, 15);
		switch(pi->morez)
		{
		case 1:
			tempfx::add(pc, pc, tempfx::SPELL_AGILITY, 5+RandomNum(1,10), 0, 0, 120);
			pc->sysmsg(TRANSLATE("You feel more agile!"));
			break;
		case 2:
			tempfx::add(pc, pc, tempfx::SPELL_AGILITY, 10+RandomNum(1,20), 0, 0, 120);
			pc->sysmsg(TRANSLATE("You feel much more agile!"));
			break;
		default:
			ErrOut("Switch fallout. pyuo.cpp, usepotion()\n");
			return;
		}
		pc->playSFX(0x01E7);
		if (s!=INVALID)
			pc->updateStats(2);
		break;

	case 2: // Cure Potion
		if (pc->poisoned<1)
			pc->sysmsg(TRANSLATE("The potion had no effect."));
		else
		{
			switch(pi->morez)
			{
			case 1:
				x=RandomNum(1,100);
				if (pc->poisoned==POISON_WEAK && x<81) pc->poisoned=POISON_NONE;
				if (pc->poisoned==POISON_NORMAL && x<41) pc->poisoned=POISON_NONE;
				if (pc->poisoned==POISON_GREATER && x<21) pc->poisoned=POISON_NONE;
				if (pc->poisoned==POISON_DEADLY && x< 6) pc->poisoned=POISON_NONE;
				break;
			case 2:
				x=RandomNum(1,100);
				if (pc->poisoned==POISON_WEAK) pc->poisoned=POISON_NONE;
				if (pc->poisoned==POISON_NORMAL && x<81) pc->poisoned=POISON_NONE;
				if (pc->poisoned==POISON_GREATER && x<41) pc->poisoned=POISON_NONE;
				if (pc->poisoned==POISON_DEADLY && x<21) pc->poisoned=POISON_NONE;
				break;
			case 3:
				x=RandomNum(1,100);
				if (pc->poisoned==POISON_WEAK) pc->poisoned=POISON_NONE;
				if (pc->poisoned==POISON_NORMAL) pc->poisoned=POISON_NONE;
				if (pc->poisoned==POISON_GREATER && x<81) pc->poisoned=POISON_NONE;
				if (pc->poisoned==POISON_DEADLY && x<61) pc->poisoned=POISON_NONE;
				break;
			default:
				ErrOut("Switch fallout. pyuo.cpp, usepotion()\n"); //Morrolan
				return;
			}
			if (pc->poisoned)
				pc->sysmsg(TRANSLATE("The potion was not able to cure this poison."));
			else
			{
				staticeffect(DEREF_pChar(pc), 0x37, 0x3A, 0, 15);
				pc->playSFX( 0x01E0); //cure sound - SpaceDog
				pc->sysmsg(TRANSLATE("The poison was cured."));
			}
		}
		impowncreate(s,pc,1); //Lb, makes the green bar blue or the blue bar blue !
		break;

	case 3: {// Explosion Potion
		if (region[pc->region].priv&0x01) // Ripper 11-14-99
		{
			pc->sysmsg(TRANSLATE(" You cant use that in town!"));
			return;
		}
		pc->sysmsg(TRANSLATE("Now would be a good time to throw it!"));
		tempfx::add(pc, pc, tempfx::EXPLOTIONMSG, 0, 1, 3);
		tempfx::add(pc, pc, tempfx::EXPLOTIONMSG, 0, 2, 2);
		tempfx::add(pc, pc, tempfx::EXPLOTIONMSG, 0, 3, 1);
		tempfx::add(pc, pi, tempfx::EXPLOTIONEXP, 0, 4, 0);

		P_TARGET targ= clientInfo[s]->newTarget( new cTarget() );
		targ->code_callback=target_expPotion;
		targ->buffer[0]= pi->getSerial();
		targ->send( getClientFromSocket(s) );
		sysmessage( s, "*throw*" );
		return;
	}
	case 4: // Heal Potion
		switch(pi->morez)
		{
		case 1:
			pc->hp=qmin(pc->hp+5+RandomNum(1,5)+pc->skill[17]/100,pc->getStrength());
			pc->sysmsg(TRANSLATE("You feel better!"));
			break;
		case 2:
			pc->hp=qmin(pc->hp+15+RandomNum(1,10)+pc->skill[17]/50,pc->getStrength());
			pc->sysmsg(TRANSLATE("You feel more healty!"));
			break;
		case 3:
			pc->hp=qmin(pc->hp+20+RandomNum(1,20)+pc->skill[17]/40, pc->getStrength());
			pc->sysmsg(TRANSLATE("You feel much more healty!"));
			break;

		default:
			ErrOut("Switch fallout. pyuo.cpp, usepotion()\n"); //Morrolan
			return;
		}

		if (s!=INVALID)
			pc->updateStats(0);

		staticeffect(DEREF_pChar(pc), 0x37, 0x6A, 0x09, 0x06); // Sparkle effect
		pc->playSFX(0x01F2); //Healing Sound - SpaceDog
		break;

	case 5: // Night Sight Potion
		staticeffect(DEREF_pChar(pc), 0x37, 0x6A, 0x09, 0x06);
		tempfx::add(pc, pc, tempfx::SPELL_LIGHT, 0, 0, 0,(720*secondsperuominute*MY_CLOCKS_PER_SEC));
		pc->playSFX(0x01E3);
		break;

	case 6: // Poison Potion
		if(pc->poisoned < (PoisonType)pi->morez)
			pc->poisoned=(PoisonType)pi->morez;
		if(pi->morez>4)
			pi->morez=4;
		pc->poisonwearofftime=uiCurrentTime+(MY_CLOCKS_PER_SEC*SrvParms->poisontimer); // lb, poison wear off timer setting
		impowncreate(s,pc,1); //Lb, sends the green bar !
		pc->playSFX(0x0246); //poison sound - SpaceDog
		pc->sysmsg(TRANSLATE("You poisoned yourself! *sigh*")); //message -SpaceDog
		break;

	case 7: // Refresh Potion
		switch(pi->morez)
		{
			case 1:
				pc->stm=qmin(pc->stm+20+RandomNum(1,10), pc->dx);
				pc->sysmsg(TRANSLATE("You feel more energetic!"));
				break;

			case 2:
				pc->stm=qmin(pc->stm+40+RandomNum(1,30), pc->dx);
				pc->sysmsg(TRANSLATE("You feel much more energetic!"));
				break;

			default:
				ErrOut("Switch fallout. pyuo.cpp, usepotion()\n"); //Morrolan
				return;
		}
		if (s!=INVALID)
			pc->updateStats(2);
		staticeffect(DEREF_pChar(pc), 0x37, 0x6A, 0x09, 0x06); // Sparkle effect
		pc->playSFX(0x01F2); //Healing Sound
		break;

	case 8: // Strength Potion
		staticeffect(DEREF_pChar(pc), 0x37, 0x3a, 0, 15);
		switch(pi->morez)
		{
		case 1:
			tempfx::add(pc, pc, tempfx::SPELL_STRENGHT, 5+RandomNum(1,10), 0, 0, 120);
			pc->sysmsg(TRANSLATE("You feel more strong!"));
			break;
		case 2:
			tempfx::add(pc, pc, tempfx::SPELL_STRENGHT, 10+RandomNum(1,20), 0, 0, 120);
			pc->sysmsg(TRANSLATE("You feel much more strong!"));
			break;
		default:
			ErrOut("Switch fallout. pyuo.cpp, usepotion()\n");
			return;
		}
		pc->playSFX(0x01EE);
		break;

	case 9: // Mana Potion
		switch(pi->morez)
		{
		case 1:
			pc->mn=qmin(pc->mn+10+pi->morex/100, (unsigned)pc->in);
			break;

		case 2:
			pc->mn=qmin(pc->mn+20+pi->morex/50, (unsigned)pc->in);
			break;

		default:
			ErrOut("Switch fallout. pyuo.cpp, usepotion()\n");
			return;
		}
		if (s!=INVALID)
			pc->updateStats(1);
		staticeffect(DEREF_pChar(pc), 0x37, 0x6A, 0x09, 0x06); // Sparkle effect
		pc->playSFX(0x01E7); //agility sound - SpaceDog
		break;

	case 10: //LB's LSD potion, 5'th november 1999
		if (pi->getId()!=0x1841) return; // only works with an special flask
		if (s==INVALID) return;
		if( clientInfo[s]->lsd )
		{
			pc->sysmsg(TRANSLATE("no,no,no,cant you get enough ?"));
			return;
		}
		tempfx::add(pc, pc, tempfx::LSD, 60+RandomNum(1,120), 0, 0); // trigger effect
		staticeffect(DEREF_pChar(pc), 0x37, 0x6A, 0x09, 0x06); // Sparkle effect
		pc->playSFX(0x00F8, true); // lsd sound :)
		break;

	default:
		ErrOut("Switch fallout. pyuo.cpp, usepotion()\n"); //Morrolan
		return;
	}

	pc->playSFX(0x0030);
	if (pc->HasHumanBody() && !pc->isMounting())
		pc->playAction(0x22);

	pi->ReduceAmount( 1 );

	if (pi->morey!=3)
	{
		int lsd=pi->morey; // save morey before overwritten

		pi = new cItem(cItem::nextSerial());

		pi->setId( 0x0F0E );

		if (lsd==10) // empty Lsd potions
		{
			pi->setId( 0x183d );
		}

		pi->pileable=1;

		pItem pack=pc->getBackpack();
		if (pack) {
			pack->AddItem( pi );
		}
		else {
			pi->MoveTo( pc->getPosition() );
			pi->setDecay();
			pi->Refresh();
		}
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
			pChar pc=pointers::findCharBySerial(pio->getContSerial());
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
	pChar pc=MAKE_CHAR_REF(currchar[s]);
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
\author ?
*/
void SetGlobalVars()
{
	int i=0;
	ConOut("Initializing global variables...");

	w_anim[0]=0; w_anim[1]=0; w_anim[2]=0;

	for (i=0; i>ALLSKILLS; i++) { strcpy(title[i].other, "old titles.scp error"); }
	completetitle = new char[1024];
	for (i=0;i<(MAXCLIENT);i++) { clientDimension[i]=2; } // LB
	for (i=0;i<MAXLAYERS;i++) layers[i]=0;

	save_counter=0;

	nettimeout.tv_sec=0;
	nettimeout.tv_usec=0;
	keeprun=true;
	error=0;
	now=0;
	secure=1;
//	charcount=0;
//	itemcount=0;
//	charcount2=1;
//	itemcount2=0x40000000;
	donpcupdate=0;
	wtype=0;

	g_nShowLayers=false;
	ConOut(" [ OK ]\n");
}


/*!
\brief ?
\author ?
*/
void BuildPointerArray()
{
	int memerrflg=0;
	for (int i=0;i<HASHMAX;i++)
	{
		//cownsp[i].pointer = NULL;
		//ownsp[i].pointer = NULL;
		//imultisp[i].pointer = NULL;
		//cmultisp[i].pointer = NULL;
//		glowsp[i].pointer = NULL;

#define PTRPREALLOC 25
		// init them
		// xan : we'll use realloc, so use malloc, not new for sake of portability
		//if(( imultisp[i].pointer = reinterpret_cast<int*>(malloc(PTRPREALLOC*sizeof(int)))) == NULL)
		//	memerrflg=1;
		//if(( cmultisp[i].pointer = reinterpret_cast<int*>(malloc(PTRPREALLOC*sizeof(int)))) == NULL)
		//	memerrflg=1;
		/*if(( glowsp[i].pointer = reinterpret_cast<int*>(malloc(PTRPREALLOC*sizeof(int)))) == NULL)
			memerrflg=1;*/

		if (memerrflg)
		{
			ConOut("\nFatal Error: Couldn't allocate pointer memory!\n");
			Network->kr=0;
			return;
		}
		//imultisp[i].max=cmultisp[i].max=/*glowsp[i].max=*/25;
		//for (int j=0;j<25;j++)
		//	imultisp[i].pointer[j]=cmultisp[i].pointer[j]=/*glowsp[i].pointer[j]=*/INVALID;
	}
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
	/*for (i=0;i<charcount;i++)
	{*/
		if ( !isCharSerial(objs.getSerial()) ) continue;
		pChar pc_i = (pChar)(objs.getObject());
		if(!pc_i)
			continue;

		pItem multi=findmulti( pc_i->getPosition() );
		if( multi )
		{
			if (multi->type==117)
				//setserial(i,DEREF_pItem(multi),8);
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
				if (multi->getSerial()!=pi->getSerial32())
					//setserial(DEREF_pItem(pi),DEREF_pItem(multi),7);
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
	Accounts = new cAccounts;
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
	delete Accounts;
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
\brief remove all the item without any parents
(not in any valid container)
\author ?
\since ?
\remarks \remark rewritten by Anthalir
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
						pChar own=pointers::findCharBySerial( pc->getOwnerSerial32() );
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
				pChar pc_j= pointers::findCharBySerial(pi->getContSerial());
				pItem pi_j= pointers::findItemBySerial(pi->getContSerial());

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
