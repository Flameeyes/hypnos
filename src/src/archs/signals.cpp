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

// If not HAVE_SIGNAL_H simply compile an empty unit
#ifdef HAVE_SIGNAL_H

#include <signal.h>

tSigHandler *tSigHandler::instance = NULL;

/*!
\brief Constructor
\author Flameeyes based on AnomCwrd

This constructor is used to ignore some signals on platform which supports
sigaction call. At the moment we know that Windows doesn't support sigaction
call, so we simply put this under HAVE_SIGACTION conditioned compilation.
The check is done by configure script.

\todo Throw exception when the singleton is already instanced.
*/
tSigHandler::tSigHandler()
{
	instance = this;
#ifdef HAVE_SIGACTION
	sigset_t signals_to_block;
	struct sigaction ignore_handler;

	// we have to use 'struct sigaction' since there is also a
	// function called sigaction in the same scope as the struct. That
	// is okay for C, but C++ is cool and does not require struct to
	// declare a variable of struct type.

	// set the sigaction struct to all zeros
	memset(&ignore_handler, 0, sizeof(struct sigaction));

	// set the ignore_handler to SIG_IGN
	ignore_handler.sa_handler = SIG_IGN;

	// fill the 'signals_to_block' variable will all possible signals
	sigfillset(&signals_to_block);

	// set the thread sigmask (add all threads from now on)
	// to one that blocks all signals except SIGPIPE
	sigdelset(&signals_to_block, SIGPIPE);
	sigdelset(&signals_to_block, SIGFPE);
	sigdelset(&signals_to_block, SIGILL);
	sigdelset(&signals_to_block, SIGSEGV);
	sigdelset(&signals_to_block, SIGBUS);
	pthread_sigmask(SIG_BLOCK, &signals_to_block, 0);

	// ignore SIGPIPE, we catch it on Socket::write()
	sigaction(SIGPIPE, &ignore_handler, 0);
#endif
	start(true);
}

/*!
\brief Signal-handling thread
\author Flameeyes based on AnomCwrd

This thread is used to catch the signal fromt he operating system.
Because Windows does have signal handling, but only for some signals,
we check for definition of the SIG*
*/
void* tSigHandler::run()
{
	sigset_t signals_to_catch;

	// clear out the list of signals to catch
	sigemptyset(&signals_to_catch);

	// now, start adding signals we care about
	#ifdef SIGHUP
	sigaddset(&signals_to_catch, SIGHUP);
	#endif
	#ifdef SIGINT
	sigaddset(&signals_to_catch, SIGINT);
	#endif
	#ifdef SIGQUIT
	sigaddset(&signals_to_catch, SIGQUIT);
	#endif
	#ifdef SIGTERM
	sigaddset(&signals_to_catch, SIGTERM);
	#endif
	#ifdef SIGUSR1
	sigaddset(&signals_to_catch, SIGUSR1);
	#endif
	#ifdef SIGUSR2
	sigaddset(&signals_to_catch, SIGUSR2);
	#endif

	// we need to know which signal we caught
	int current_signal;

	while(1)
	{ // loop forever waiting on signals
		sigwait(&signals_to_catch, &current_signal);

		switch (signal)
		{
		#ifdef SIGHUP
		case SIGHUP:
		#endif
			handleHup();
			break;
		
		#ifdef SIGUSR1
		case SIGUSR1:
		#endif
			handleUsr1();
			break;
		
		#ifdef SIGUSR2
		case SIGUSR2:
		#endif
			handleUsr2();
			break;
	
		#ifdef SIGINT
		case SIGINT:
		#endif
		#ifdef SIGQUIT
		case SIGQUIT:
		#endif
		#ifdef SIGTERM
		case SIGTERM:
		#endif
			handleTerm();
			break;
		
		}
		
		Wefts::OSSleep(0, 100*1000*1000);
	}
}

#endif
