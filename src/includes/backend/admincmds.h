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
#include <iostream>

#ifndef __BACKEND_ADMINCMDS_H__
#define __BACKEND_ADMINCMDS_H__

/*!
\brief Administrative commands

This namespace contains functions for parse and execute administrative commands
like the ones sent by the cli or the gui of Hypnos and the ones given by the
RAC clients.

\see tRACReceiver
*/
namespace nAdminCommands
{
	void parseCommand(const std::string &str, std::ostream &outs)
};

#endif
