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

#include "clock.h"
#include "common_libs.h"
#include "data.h"
#include "hypnos.h"
#include "logsystem.h"
#include "version.h"
#include "archs/console.h"
#include "archs/daemon.h"
#include "archs/signals.h"
#include "backend/admincmds.h"

#ifdef HAVE_WINCON_H
#include <wincon.h>
#endif

#include <stdarg.h>
#include <iostream>

// Only on unix-es we do colored output
#if defined(__unix__)
#define AnsiOut(s, x) s << x
#else
#define AnsiOut(s, x)
#endif

/*!
\brief Constructor for console interface thread

This constructor replaces the old constart() function, setting the stuff needed
by console interface, and preparing to expect orders from standard input.
*/
tConsoleInterface::tConsoleInterface() : tInterface()
{
	std::ios::sync_with_stdio(false);
	
	std::cout << "Starting Hypnos..." << std::endl << std::endl;
	
	outputHypnosIntro(std::cout);
	
	// sets the window title
	#ifdef __unix__
		std::cout << "\033]0;Hypnos " << strVersion << "\007";
	#elif defined(HAVE_WINCON_H)
		std::string tmp = "Hypnos " + strVersion;
		SetConsoleTitle(tmp.c_str());
	#endif
	
	#ifdef WIN32
	if ( ServerScp::g_nDeamonMode )
		return;
		
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
	
	for (coord.Y = 0; coord.Y<1024; coord.Y++)
		WriteConsoleOutputAttribute(Buff, (CONST WORD *)arr,80,coord,(LPDWORD )&w);       // actual number written

	#endif
}

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
void tConsoleInterface::output(tInterface::Level lev, const std::string &str)
{
	static Wefts::Mutex m;
	m.lock();
	
	std::ostream outs = std::cout;
	
	// Set the color
	switch(lev)
	{
	case tInterface::levPlain:
		break;
	case tInterface::levError:
		outs = std::cerr;
		AnsiOut(outs, "\x1B[1;31m");
		outs << "E " << getDateString() << " - ";
		break;
	case tInterface::levWarning:
		outs = std::cerr;
		AnsiOut(outs, "\x1B[1;33m");
		outs << "W " << getDateString() << " - ";
		break;
	case tInterface::levInformation:
		AnsiOut(outs, "\x1B[1;34m");
		outs << "i " << getDateString() << " - ";
		break;
	case tInterface::levPanic:
		outs = std::cerr;
		AnsiOut(outs, "\x1B[1;31m");
		outs << "! " << getDateString() << " - ";
		break;
	}
	
	outs << str;
	
	// Close the colored part
	if ( lev != tInterface::levPlain )
		AnsiOut(outs, "\x1B[0m");
	
	m.unlock();
}

/*!
\brief Thread function for console interface

This is the function which does all the dirt work for the console interface
thread. It waits for input on standard input stream, and then parse the
received commands.

This is an infinite loop (<pre>while(true)</pre>), but can be cancelled on
the getline.
*/
void *tConsoleInterface::run()
{
	static bool secure = true;
	std::string str;
	
	while(true)
	{
		getline(std::cin, str);
		
		if ( ! str.length() ) continue;
	
		if ( str == "S" )
		{
			if (secure)
				std::cout << "Secure mode disabled. Press ? for a commands list." << std::endl;
			else
				std::cout << "Secure mode re-enabled." << std::endl;
			
			secure = ! secure;
			continue;
		}
		
		if (secure && str != "?")  //Allows help in secure mode.
		{
			std::cout << "Secure mode prevents keyboard commands! Press 'S' to disable." << std::endl;
			continue;
		}
		
		nAdminCommands::parseCommand(str, std::cout);
	}
}

int main(int argc, char *argv[])
{
	// Better way to handle daemons on nix platforms: --daemon argument :)
	if ( argc > 1 && ! strcmp(argv[1], "--daemon") )
		init_daemon();
	
	new tConsoleInterface();
	new tMainLoop();
	
	tMainLoop::instance->join();
	
	delete tMainLoop::instance;
	
	return 0;
}

#if 0
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

	std::cout << std::endl << "Allowed clients : ";
	for ( ; vis != vis_end;  ++vis)
	{
		if ( (*vis) == "SERVER_DEFAULT" )
		{
			std::cout << (*vis) << " : " << strSupportedClient << std::endl;
			break;
		}
		else if ( t == "ALL" )
		{
			std::cout << "ALL" << std::endl;
			break;
		}

		std::cout << (*vis) << ", ";
	}
	std::cout << std::endl;
	
	std::cout << "Server started" << std::endl;
	
	shutdownServer();

	if (error) {
		std::cerr << "ERROR: Server terminated by error!" << std::endl;

		if (SrvParms->server_log)
			ServerLog.Write("Server Shutdown by Error!\n");
	} else {
		std::cout << "Hypnos: Server shutdown complete!" << std::endl;
		if (SrvParms->server_log)
			ServerLog.Write("Server Shutdown!\n");
	}
	
	return 0;
}
#endif