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

#include "clock.hpp"
#include "common_libs.hpp"
#include "data.hpp"
#include "hypnos.hpp"
#include "logsystem.hpp"
#include "mainloop.hpp"
#include "version.hpp"
#include "archs/console.hpp"
#include "archs/signals.hpp"
#include "backend/admincmds.hpp"

#include "libhypnos/strings.hpp"

#ifdef HAVE_WINCON_H
#include <wincon.h>
#endif

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#ifdef HAVE_IOSTREAM
	#include <iostream>
	using std::endl;
	using std::cout;
	using std::cerr;
#elif defined HAVE_IOSTREAM_H
	#include <iostream.h>
#endif

#ifdef HAVE_IOS
	#include <ios>
	using std::sync_with_stdio;
#elif defined HAVE_IOS_H
	#include <ios.h>
#endif

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
	ios::sync_with_stdio(false);
	
	cout << "Starting Hypnos..." << endl << endl;
	
	outputHypnosIntro(cout);
	
	// sets the window title
	#ifdef __unix__
		cout << "\033]0;Hypnos " << strVersion << "\007";
	#elif defined(HAVE_WINCON_H)
		string tmp = "Hypnos " + strVersion;
		SetConsoleTitle(tmp.c_str());
	#endif
	
	#ifdef WIN32
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
void tConsoleInterface::output(tInterface::Level lev, const string &str)
{
	static Wefts::Mutex m;
	m.lock();
	
	ostream *outs = &cout;
	
	// Set the color
	switch(lev)
	{
	case tInterface::levPlain:
		break;
	case tInterface::levError:
		outs = &cerr;
		AnsiOut(*outs, "\x1B[1;31m");
		*outs << "E " << getDateString() << " - ";
		break;
	case tInterface::levWarning:
		outs = &cerr;
		AnsiOut(*outs, "\x1B[1;33m");
		*outs << "W " << getDateString() << " - ";
		break;
	case tInterface::levInformation:
		AnsiOut(*outs, "\x1B[1;34m");
		*outs << "i " << getDateString() << " - ";
		break;
	case tInterface::levPanic:
		outs = &cerr;
		AnsiOut(*outs, "\x1B[1;31m");
		*outs << "! " << getDateString() << " - ";
		break;
	}
	
	*outs << str;
	
	// Close the colored part
	if ( lev != tInterface::levPlain )
		AnsiOut(*outs, "\x1B[0m");
	
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
	string str;
	
	while(true)
	{
		getline(cin, str);
		
		if ( ! str.length() ) continue;
	
		if ( str == "S" )
		{
			if (secure)
				cout << "Secure mode disabled. Press ? for a commands list." << endl;
			else
				cout << "Secure mode re-enabled." << endl;
			
			secure = ! secure;
			continue;
		}
		
		if (secure && str != "?")  //Allows help in secure mode.
		{
			cout << "Secure mode prevents keyboard commands! Press 'S' to disable." << endl;
			continue;
		}
		
		nAdminCommands::parseCommand(str, cout);
	}
}

int main(int argc, char *argv[])
{
	bool daemon = false;

#ifdef HAVE_FORK
	// Better way to handle daemons on nix platforms: --daemon argument :)
	if ( argc > 1 && ! strcmp(argv[1], "--daemon") )
		daemon = true;
#endif

	if ( daemon )
	{
		cout << "Going into daemon mode." << endl;
		new tNullInterface();
	} else
		new tConsoleInterface();
	
	new tMainLoop();
	
	tMainLoop::instance->join();
	
	delete tMainLoop::instance;
	
	return 0;
}
