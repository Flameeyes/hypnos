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
#include "clock.h"
#include "arch/console.h"
#include "arch/signals.h"

#ifdef HAVE_WINCON_H
#include <wincon.h>
#endif

#include <iostream>

/*!
\brief Do a low-level output on the console
\param str String to print (printf-formatted)

This function is used by consoleOutput to actually do an output. Must \b not
be accessed in other ways!
*/
void lowlevelOutput(std::ostream &outs, const char *str, ...)
{
	char *buf = NULL;
	va_list argptr;

	va_start( argptr, str );
	vasprintf( &buf, str, argptr );
	va_end( argptr );

	outs << buf;
	
	free(buf);
}

// Only on unix-es we do colored output
#if defined(__unix__)
#define AnsiOut(s, x) s << x
#else
#define AnsiOut(s, x)
#endif

/*!
\brief Output a given string to the console at a given level
\param lev Level to output the string at
\param str String to output

This function outputs a message at the given level, setting te color of the
text and changing the output stream (stdout for plain and info messages,
stderr for other).

\note This function is thread-safe, a Mutex prevent that the console is
	accessed more than one time.
*/
void consoleOutput(nNotify::Level lev, const std::string str)
{
	static Wefts::Mutex m;
	m.lock();
	
	std::ostream outs = std::cout;
	
	// Set the color
	switch(lev)
	{
	case levPlain:
		break;
	case levError:
		outs = std::cerr;
		AnsiOut(outs, "\x1B[1;31m");
		lowlevelOutput(outs, "E %s - ", nNotify::getDate().c_str());
		break;
	case levWarning:
		outs = std::cerr;
		AnsiOut(outs, "\x1B[1;33m");
		lowlevelOutput(outs, "W %s - ", nNotify::getDate().c_str());
		break;
	case levInformation:
		AnsiOut(outs, "\x1B[1;34m");
		lowlevelOutput(outs, "i %s - ", nNotify::getDate().c_str());
		break;
	case levPanic:
		outs = std::cerr;
		AnsiOut(outs, "\x1B[1;31m");
		lowlevelOutput(outs, "! %s - ", nNotify::getDate().c_str());
		break;
	}
	
	lowlevelOutput(outfp, str.c_str());
	
	// Close the colored part
	if ( lev != levPlain )
		AnsiOut(outs, "\x1B[0m");
	
	m.unlock();
}

static inline uint32_t CheckMilliTimer(uint32_t &Seconds, uint32_t &Milliseconds)
{
	uint32_t newSec, newMill;
	getClock(newSec, newMill);

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
		lowlevelOutput(std::cout, "\033]0;%s\007", temp); // xterm code
	#elif defined(HAVE_WINCON_H)
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

// Define stubs to avoid conditional compilation inside the loop

/*!
\brief facilitate console control. SysOp keys and localhost controls
*/
void checkkey ()
{
	std::string str;
	getline(std::cin, str);
	
	if ( ! str.lenght() ) return;

	if ( str == "S" )
	{
		if (secure)
			lowlevelOutput(std::cout, "Secure mode disabled. Press ? for a commands list.\n");
		else
			lowlevelOutput(std::cout, "Secure mode re-enabled.\n");
		
		secure = ! secure;
		return;
	}
	
	if (secure && str != "?")  //Allows help in secure mode.
	{
		lowlevelOutput(std::cout, "Secure mode prevents keyboard commands! Press 'S' to disable.\n");
		return;
	}
	
	nAdminCommands::parseCommand(str, std::cout);
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

	initclock();

	serverstarttime=getClockmSecs();

	initConsole();
	lowlevelOutput(std::cout, "Starting Hypnos...\n\n");

	loadServer();

	if (ServerScp::g_nDeamonMode!=0) {
		lowlevelOutput(std::cout, "Going into deamon mode... bye...\n");
		init_deamon();
	}

	lowlevelOutput(std::cout, "Applying interface settings... ");
	constart();
	lowlevelOutput(std::cout, "[ OK ]\n");

	openings = 0;

	serverstarttime=getClockmSecs();

	lowlevelOutput(std::cout, "\n");
	cwmWorldState->loadNewWorld();

	exitOnError(error); // LB prevents file corruption

	FD_ZERO(&conn);
	starttime=getClockmSecs();
	endtime=0;
	lclock=0;

	lowlevelOutput(std::cout, "\n\n");

	clearscreen();

	lowlevelOutput(std::cout,
		"Hypnos UO Server Emulator %s\n
		"Programmed by: %s\n"
		"Based on NoX-Wizard 20031228\n"
		"Website: http://hypnos.berlios.de/\n"
		"\n"
		"Original copyright (C) 1997, 98 Marcus Rating (Cironian)\n\n"
		"This program is free software; you can redistribute it and/or modify\n"
		"it under the terms of the GNU General Public License as published by\n"
		"the Free Software Foundation; either version 2 of the License, or\n"
		"(at your option) any later version.\n
		"See LICENSE file for more information\n"
		"\n"
		"Running on %s\n",
		strVersion, strDevelopers,
		getOSVersionString().c_str() );
	
	if (SrvParms->server_log)
		ServerLog.Write(
			"-=Server Startup=-\n"
			"=======================================================================\n");

	serverstarttime=getClockmSecs(); // dont remove, its absolutly necassairy that its 3 times in the startup sequence for several timing reasons.

	exitOnError(error);

	lowlevelOutput(std::cout, "\nMap size : %dx%d", map_width, map_height);

	if ((map_width==768)&&(map_height==512))
		lowlevelOutput(std::cout, " [standard Britannia/Sosaria map size]\n");
	else if ((map_width==288)&&(map_height==200))
		lowlevelOutput(std::cout, " [standard Ilshenar map size]\n");
	else lowlevelOutput(std::cout, " [custom map size]\n");

	if (ServerScp::g_nAutoDetectIP==1)  {
		lowlevelOutput(std::cout, "\nServer waiting connections on all interfaces at TCP port %i\n", g_nMainTCPPort);
	} else {
		lowlevelOutput(std::cout, "\nServer waiting connections at IP %s, TCP port %i\n", serv[0][1], g_nMainTCPPort);
	}

	// print allowed clients
	std::string t;
	std::vector<std::string>::const_iterator vis( clientsAllowed.begin() ), vis_end( clientsAllowed.end() );

	lowlevelOutput(std::cout, "\nAllowed clients : ");
	for ( ; vis != vis_end;  ++vis)
	{
		t = (*vis);  // a bit pervert to store c++ strings and operate with c strings, admitably

		if ( t == "SERVER_DEFAULT" )
		{
			lowlevelOutput(std::cout, "%s : %s\n", t.c_str(), strSupportedClient);
			break;
		}
		else if ( t == "ALL" )
		{
			lowlevelOutput(std::cout, "ALL\n");
			break;
		}

		lowlevelOutput(std::cout, "%s,", t.c_str());
	}
	lowlevelOutput(std::cout, "\n");
	
	pointers::init(); //Luxor

	lowlevelOutput(std::cout, "Server started\n");

	Spawns->doSpawnAll();

	while (keeprun)
	{
		keeprun = (!pollCloseRequests());
		
		checkkey();
		//OnLoop

		if(loopTimeCount >= 1000)
		{
			loopTimeCount = 0;
			loopTime = 0;
		}
		loopTimeCount++;

		getClock(loopSecs, loopMilli);
                //testAI();
		if(networkTimeCount >= 1000)
		{
			networkTimeCount = 0;
			networkTime = 0;
		}

		getClock(tempSecs, tempMilli);

		if ( TIMEOUT( CheckClientIdle ) )
		{
			CheckClientIdle=((SrvParms->inactivitytimeout/2)*SECS)+getClockmSecs();

			for (r=0;r<now;r++)
			{
				pChar pc_r=cSerializable::findCharBySerial(currchar[r]);
				if(! pc_r )
					continue;
				if (!pc_r->IsGM()
					&& pc_r->clientidletime<getClockmSecs()
					&& clientInfo[r]->ingame
					)
				{
					lowlevelOutput(std::cout, "Player %s disconnected due to inactivity !\n", pc_r->getCurrentName().c_str());
					//sysmessage(r,"you have been idle for too long and have been disconnected!");
					nPackets::Sent::IdleWarning pk(0x7);
					client->sendPacket(&pk);
					Network->Disconnect(r);
				}

			}
		}

		tempTime = CheckMilliTimer(tempSecs, tempMilli);

		networkTime += tempTime;
		networkTimeCount++;

		if(timerTimeCount >= 1000)
		{
			timerTimeCount = 0;
			timerTime = 0;
		}

		getClock(tempSecs, tempMilli);

		checktimers();

		tempTime = CheckMilliTimer(tempSecs, tempMilli);
		timerTime += tempTime;
		timerTimeCount++;

		if(autoTimeCount >= 1000)
		{
			autoTimeCount = 0;
			autoTime = 0;
		}
		getClock(tempSecs, tempMilli);

		checkauto();

		tempTime = CheckMilliTimer(tempSecs, tempMilli);
		autoTime += tempTime;
		autoTimeCount++;

		tempTime = CheckMilliTimer(loopSecs, loopMilli);
		loopTime += tempTime;
	}
	
	shutdownServer();

	if (NewErrorsLogged())
		lowlevelOutput(stderr, "New ERRORS have been logged. Please send the logs/error*.log and logs/critical*.log files to the dev team !\n");
	if (NewWarningsLogged())
		lowlevelOutput(stderr, "New WARNINGS have been logged. Probably scripting errors. See the logs/warnings*.log for details !\n");

	if (error) {
		lowlevelOutput(stderr, "ERROR: Server terminated by error!\n");

		if (SrvParms->server_log)
			ServerLog.Write("Server Shutdown by Error!\n");
	} else {
		lowlevelOutput(std::cout, "Hypnos: Server shutdown complete!\n");
		if (SrvParms->server_log)
			ServerLog.Write("Server Shutdown!\n");
	}
	
	return 0;
}
