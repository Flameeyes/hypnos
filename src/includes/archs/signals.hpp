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
/*!
\file
\brief Unix's Singals handling

This file contains functions to handle the Unix signals, like SIGHUP and
similar.
*/

#ifndef __ARCHS_SIGNALS_H__
#define __ARCHS_SIGNALS_H__

#include "common_libs.h"

#ifdef HAVE_SIGNAL_H

#include <wefts_thread.h>

/*!
\brief Signal handling class

This class is used to allow signal handling on platform which supports it.
At the moment are supported Linux, the *BSD platforms and Windows (that in
a lighter way).

This class can be only used when the target has signal.h header (and so
supports signals handling).

The handled signals (where present) are these:
	\li \b SIGHUP reloads all the scripts' data (Resync)
	\li \b SIGUSR1 reloads the account file, deleting the old accounts
		and adding the new ones
	\li \b SIGUSR2 saves the world
	\li \b SIGTERM, \b SIGQUIT, \b SIGINT Close the server (gracefully)

\note Windows hasn't SIGHUP, SIGUSR1 and SIGUSR2 signals, so we must check for
	single signals to be sure to not harm...

\todo The handlers functions are for now undefined, must be wrote.
*/
class tSigHandler : public Wefts::Thread
{
private:
	void handleHup();
	void handleUsr1();
	void handleUsr2();
	void handleTerm();
public:
	static tSigHandler *instance;
	
	tSigHandler();
	
	void *run();
};

#endif // HAVE_SIGNAL_H

#endif // __ARCHS_SIGNALS_H__
