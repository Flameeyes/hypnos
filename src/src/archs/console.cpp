/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2003 - NoX-Wizard Project                                  |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "common_libs.h"
#include "hypnos.h"
#include "version.h"
#include "arch/console.h"
#include "arch/signals.h"

/*!
\brief Do a low-level output on the console
\param str String to print (printf-formatted)

This function is used by consoleOutput to actually do an output. Must \b not
be accessed in other ways!
*/
void lowlevelOutput(const char *str, ...)
{
	static char buffer[2048];
	va_list argptr;

	va_start( argptr, str );
	vsnprintf( buffer, sizeof(buffer)-1, str, argptr );
	va_end( argptr );

#ifndef _WINDOWS
	fprintf(s_fileStdOut, buffer);
#else
	xwprintf("%s", buffer);
#endif
}

// Only on unix-es we do colored output
#if defined(__unix__)
#define AnsiOut lowlevelOutput
#else
#define AnsiOut(x)
#endif

/*!
\brief Output a given string to the console at a given level
\param lev Level to output the string at
\param str String to output

\note This function is thread-safe, a Mutex prevent that the console is
	accessed more than one time.
*/
void consoleOutput(nNotify::Level lev, const std::string str)
{
	static Wefts::Mutex;
	m.lock();
	
	// Set the color
	switch(lev)
	{
	case levPlain:
		break;
	case levError:
		AnsiOut("\x1B[1;31m");
		lowlevelOutput("E %s - ", nNotify::getDate().c_str());
		break;
	case levWarning:
		AnsiOut("\x1B[1;33m");
		lowlevelOutput("W %s - ", nNotify::getDate().c_str());
		break;
	case levInformation:
		AnsiOut("\x1B[1;34m");
		lowlevelOutput("i %s - ", nNotify::getDate().c_str());
		break;
	case levPanic:
		AnsiOut("\x1B[1;31m");
		lowlevelOutput("! %s - ", nNotify::getDate().c_str());
		break;
	}
	
	lowlevelOutput(str.c_str());
	
	// Close the colored part
	if ( lev != levPlain )
		AnsiOut("\x1B[0m");
	
#ifndef _WINDOWS
	// This flushes the output only when all the output is actually done
	fflush(s_fileStdOut);
#endif
	
	m.unlock();
}

static inline void StartMilliTimer(unsigned long &Seconds, unsigned long &Milliseconds)
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

static inline unsigned long CheckMilliTimer(unsigned long &Seconds, unsigned long &Milliseconds)
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

static FILE *s_fileStdOut = NULL;

void setWinTitle(char *str, ...)
{
	if (ServerScp::g_nDeamonMode!=0) return;

	char *temp; //xan -> this overrides the global temp var
	va_list argptr;

	va_start( argptr, str );
	vasprintf( &temp, str, argptr );
	va_end( argptr );
	
	#ifdef __unix__
		lowlevelOutput("\033]0;%s\007", temp); // xterm code
	#elif defined(WIN32)
		SetConsoleTitle(temp);
	#endif
	
	free(temp);
}

void constart( void )
{
	setWinTitle("Hypnos %s", strVersion);
	#ifdef WIN32
	if (ServerScp::g_nDeamonMode==0) {
		HANDLE Buff = GetStdHandle(STD_OUTPUT_HANDLE);
		COORD coord; coord.X = 80; coord.Y = (short)ServerScp::g_nLineBuffer;
		WORD arr[80];
		DWORD  w;

		SetConsoleScreenBufferSize(Buff, coord);

		unsigned short color;

		color=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN;


		
		SetConsoleTextAttribute(Buff,color);

		coord.X = coord.Y = 0;
		for (int i = 0; i<80; i++)
			arr[i] = color;
		
		for (coord.Y = 0; coord.Y<1024; coord.Y++) WriteConsoleOutputAttribute(Buff, (CONST WORD *)arr,80,coord,(LPDWORD )&w);       // actual number written

	}
	#endif
}

void initConsole()
{
	if ((ServerScp::g_nRedirectOutput)||(ServerScp::g_nDeamonMode)) {
		if(s_fileStdOut==NULL) s_fileStdOut = fopen(ServerScp::g_szOutput,"wt");
		if(s_fileStdOut==NULL) s_fileStdOut = fopen("nxwout","wt");
		if(s_fileStdOut==NULL) exit(1);
	} else s_fileStdOut = stdout;
}

static inline void exitOnError(bool error)
{
	//! \todo Replace with exception handling
	if ( ! error ) return;
	
	shutdownServer();
	exit(-1);
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
				outInfo("Secure mode disabled. Press ? for a commands list.\n");
				secure=0;
				return;
			}
			else
			{
				outInfo("Secure mode re-enabled.\n");
				secure=1;
				return;
			}
		} else {
			if (secure && c != '?')  //Allows help in secure mode.
			{
				outInfo("Secure mode prevents keyboard commands! Press 'S' to disable.\n");
				return;
			}

			switch(c)
			{
			case '\x1B':
				keeprun=false;
				break;
			case 'Q':
			case 'q':
				outInfo("Immediate Shutdown initialized!\n");
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
					outInfo( "Disconnecting account 0 players... ");
					for (i=0;i<now;i++)
						if (acctno[i]==0 && clientInfo[i]->ingame)
						{
							found++;
							Network->Disconnect(i);
						}
					if (found>0)
						outInfof( "[ OK ] (%d disconnected)\n", found);
					else	
						outInfof( "[FAIL] (no account 0 players online)\n", found);
				}
				break;
			case 'W':
			case 'w':				// Display logged in chars
				outInfo("----------------------------------------------------------------\n");
				outInfo("Current Users in the World:\n");
				j = 0;  //Fix bug counting ppl online.
				for (i=0;i<now;i++)
				{
					pChar pc_i=cSerializable::findCharBySerial(currchar[i]);
					if(pc_i && clientInfo[i]->ingame) //Keeps NPC's from appearing on the list
					{
						outInfof("%i) %s [ %08x ]\n", j, pc_i->getCurrentName().c_str(), pc_i->getSerial());
						j++;
					}
				}
				outInfof("Total Users Online: %d\n", j);
				break;
			case 'r':
			case 'R':
				outInfo("Hypnos: Total server reload!");
				//! \todo Need to freeze and unfreeze all the clients here for the resync
				//! \todo Need to call a function exported by hypnos.h
				loadServer();
				break;
			case '?':
				outInfo("Console commands:\n");
				outInfo("	<Esc> or Q: Shutdown the server.\n");
				outInfo("	T - System Message: The server is shutting down in 2 minutes.\n");
				outInfo("	# - Save world\n");
				outInfo("	D - Disconnect Account 0\n");
				outInfo("	W - Display logged in characters\n");
				outInfo("	R - Total server reload\n");
				outInfof("	S - Toggle Secure mode %s\n", secure ? "[enabled]" : "[disabled]" );
				outInfo("	? - Commands list (this)\n");
				outInfo("End of commands list.\n");
				break;
			default:
				outInfof("Key %c [%x] does not preform a function.\n",c,c);
				break;
			}
		}
	}
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

	serverstarttime=getclock();

	initConsole();
	lowlevelOutput("Starting Hypnos...\n\n");

	loadServer();

#ifdef USE_SIGNALS
	//thx to punt and Plastique :]
	signal(SIGPIPE, SIG_IGN);
	initSignalHandlers();
#endif

	if (ServerScp::g_nDeamonMode!=0) {
		lowlevelOutput("Going into deamon mode... bye...\n");
		init_deamon();
	}

	lowlevelOutput("Applying interface settings... ");
	constart();
	lowlevelOutput("[ OK ]\n");

	openings = 0;

	keeprun=(Network->kr); //LB. for some technical reasons global varaibles CANT be changed in constructors in c++.
	error=Network->faul; // i hope i can find a cleaner solution for that, but this works !!!
	// has to here and not at the cal cause it would get overriten later

	serverstarttime=getclock();

	exitOnError(error);

	lowlevelOutput("\n");
	cwmWorldState->loadNewWorld();

	exitOnError(error); // LB prevents file corruption

	lowlevelOutput("Clearing all trades...");
	clearalltrades();
	lowlevelOutput(" [DONE]\n");

	FD_ZERO(&conn);
	starttime=getclock();
	endtime=0;
	lclock=0;

	lowlevelOutput("\n\n");

	clearscreen();

	lowlevelOutput("Hypnos %s [%s]\nProgrammed by: %s", strVersion, OS, strDevelopers);
	lowlevelOutput("\nBased on NoX-Wizard 20031228");
	lowlevelOutput("\nWeb-site : http://hypnos.berlios.de/\n");
	lowlevelOutput("\n");
	lowlevelOutput("Original copyright (C) 1997, 98 Marcus Rating (Cironian)\n\n");
	lowlevelOutput("This program is free software; you can redistribute it and/or modify\n");
	lowlevelOutput("it under the terms of the GNU General Public License as published by\n");
	lowlevelOutput("the Free Software Foundation; either version 2 of the License, or\n");
	lowlevelOutput("(at your option) any later version.\n\n");
	lowlevelOutput("Running on %s\n", getOSVersionString().c_str() );

	
	if (SrvParms->server_log)
		ServerLog.Write("-=Server Startup=-\n=======================================================================\n");

	serverstarttime=getclock(); // dont remove, its absolutly necassairy that its 3 times in the startup sequence for several timing reasons.

	exitOnError(error);

	lowlevelOutput("\nMap size : %dx%d", map_width, map_height);

	if ((map_width==768)&&(map_height==512))
		lowlevelOutput(" [standard Britannia/Sosaria map size]\n");
	else if ((map_width==288)&&(map_height==200))
		lowlevelOutput(" [standard Ilshenar map size]\n");
	else lowlevelOutput(" [custom map size]\n");

	if (ServerScp::g_nAutoDetectIP==1)  {
		lowlevelOutput("\nServer waiting connections on all interfaces at TCP port %i\n", g_nMainTCPPort);
	} else {
		lowlevelOutput("\nServer waiting connections at IP %s, TCP port %i\n", serv[0][1], g_nMainTCPPort);
	}

	// print allowed clients
	std::string t;
	std::vector<std::string>::const_iterator vis( clientsAllowed.begin() ), vis_end( clientsAllowed.end() );

	lowlevelOutput("\nAllowed clients : ");
	for ( ; vis != vis_end;  ++vis)
	{
		t = (*vis);  // a bit pervert to store c++ strings and operate with c strings, admitably

		if ( t == "SERVER_DEFAULT" )
		{
			lowlevelOutput("%s : %s\n", t.c_str(), strSupportedClient);
			break;
		}
		else if ( t == "ALL" )
		{
			lowlevelOutput("ALL\n");
			break;
		}

		lowlevelOutput("%s,", t.c_str());
	}
	lowlevelOutput("\n");
	
	pointers::init(); //Luxor

	outInfo("Server started\n");

	Spawns->doSpawnAll();

	//OnStart
	AMXEXEC(AMXT_SPECIALS,0,0,AMX_AFTER);
	while (keeprun)
	{

		keeprun = (!pollCloseRequests());
		
		checkkey();
		//OnLoop
		AMXEXEC(AMXT_SPECIALS,2,0,AMX_AFTER);

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
					lowlevelOutput("Player %s disconnected due to inactivity !\n", pc_r->getCurrentName().c_str());
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
	
	shutdownServer();

	if (NewErrorsLogged())
		lowlevelOutput("New ERRORS have been logged. Please send the logs/error*.log and logs/critical*.log files to the dev team !\n");
	if (NewWarningsLogged())
		lowlevelOutput("New WARNINGS have been logged. Probably scripting errors. See the logs/warnings*.log for details !\n");

	if (error) {
		lowlevelOutput("ERROR: Server terminated by error!\n");

		if (SrvParms->server_log)
			ServerLog.Write("Server Shutdown by Error!\n");

	} else {
		lowlevelOutput("Hypnos: Server shutdown complete!\n");
		if (SrvParms->server_log)
			ServerLog.Write("Server Shutdown!\n");

	}
	return 0;
}
