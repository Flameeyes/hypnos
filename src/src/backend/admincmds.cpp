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

#include "mainloop.h"
#include "backend/admincmds.h"
#include "networking/cclient.h"

#ifdef HAVE_OSTREAM
	#include <ostream>
	using std::endl;
#elif defined HAVE_OSTREAM_H
	#include <ostream.h>
#endif

/*!
\brief Parses an administrative command
\param str Command to parse
\param outs Stream where to output the command's... output!
*/
void nAdminCommands::parseCommand(const string &str, ostream &outs)
{
	switch(str[0])
	{
	case 'T':
	case 't':
		tMainLoop::instance->requestShutdown(2);
		break;
	case '#':
		tMainLoop::instance->requestSave();
		break;
	case 'W':
	case 'w':				// Display logged in chars
		outs	<< "----------------------------------------------------------------" << endl;
		
		cClient::listConnected(outs);
		break;
	case 'r':
	case 'R':
		tMainLoop::instance->requestResync();
		break;
	case '?':
		outs	<< "Console commands:" << endl
			<< "\tQ - Shutdown the server (in 2 minutes, with broadcast)." << endl
			<< "\t# - Save world" << endl
			<< "\tW - Display logged in characters" << endl
			<< "\tR - Total server reload" << endl
			<< "\t? - Commands list (this)" << endl
			<< "End of commands list." << endl;
		break;
	default:
		outs << "Command " << str << " does not perform a function." << endl;
		break;
	}
}
