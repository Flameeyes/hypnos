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
	if (! ServerScp::g_nDeamonMode )
	{
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
	initclock();

	std::cout << "Starting Hypnos..." << std::endl << std::endl;

	loadServer();

	if (ServerScp::g_nDeamonMode!=0)
		init_deamon();

	std::cout << "Applying interface settings... ";
	constart();
	std::cout << "[ OK ]" << std::endl;

	lowlevelOutput(std::cout, "\n");
	cwmWorldState->loadNewWorld();

	endtime=0;
	lclock=0;

	std::cout << std::endl << std::endl;

	clearscreen();

	outputHypnosIntro(std::cout);
	
	if (SrvParms->server_log)
		ServerLog.Write(
			"-=Server Startup=-\n"
			"=======================================================================\n");

	std::cout << "Map size :" << map_width << " x " << map_height;

	if ((map_width==768)&&(map_height==512))
		std::cout << " [standard Britannia/Sosaria map size]" << std::endl;
	else if ((map_width==288)&&(map_height==200))
		std::cout << " [standard Ilshenar map size]" << std::endl;
	else
		std::cout << " [custom map size]" << std::endl;

	// print allowed clients
	std::vector<std::string>::const_iterator vis( clientsAllowed.begin() ), vis_end( clientsAllowed.end() );

	lowlevelOutput(std::cout, "\nAllowed clients : ");
	for ( ; vis != vis_end;  ++vis)
	{
		if ( (*vis) == "SERVER_DEFAULT" )
		{
			lowlevelOutput(std::cout, "%s : %s\n", (*vis).c_str(), strSupportedClient);
			break;
		}
		else if ( t == "ALL" )
		{
			lowlevelOutput(std::cout, "ALL\n");
			break;
		}

		lowlevelOutput(std::cout, "%s,", (*vis).c_str());
	}
	lowlevelOutput(std::cout, "\n");
	
	pointers::init(); //Luxor

	std::cout << "Server started" << std::endl;

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

		CheckClientIdle=((SrvParms->inactivitytimeout/2)*SECS)+getClockmSecs();

		for (int r=0;r<now;r++)
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

		checktimers();
		checkauto();
	}
	
	shutdownServer();

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
