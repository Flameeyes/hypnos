/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifdef __unix__

#include "common_libs.h"
#include <fstream>

termios termstate;
unsigned long int oldtime, newtime;
	
/*!
\brief signal handlers
\param signal the signal received
\todo Rewrite completely this! the signal handler is a thread so we should ITC
to the main loop to request the save or the reload.

The signals are these:
	\li \b SIGHUP reloads all the scripts' data (Resync)
	\li \b SIGUSR1 reloads the account file, deleting the old accounts
		and adding the new ones
	\li \b SIGUSR2 saves the world
	\li \b SIGTERM CLose the server (gracefully)
*/
void signal_handler(int signal)
{
	switch (signal)
	{
	case SIGHUP:
		break ;
	case SIGUSR1:
		break ;
	case SIGUSR2:
		break ;
	case SIGTERM:
		break ;
	default:
		break ;
	}
}

void init_deamon()
{
	int i ;
	pid_t pid ;

	if ((pid = fork() ) != 0)
		_exit(0) ;
	
	setsid() ;
	signal(SIGHUP, SIG_IGN);
	if ((pid=fork()) != 0)
	{
		std::fstream fPid("nxwpid", std::ios::out);
		fPid << pid << std::endl;
		fPid.close() ;
		_exit(0);
	}
	// We should close any dangling descriptors we have
	for (i=0 ; i < 64 ; i++)
		close(i) ;

	signal(SIGUSR2,&signal_handler);
	signal(SIGHUP,&signal_handler);
	signal(SIGUSR1,&signal_handler);
	signal(SIGTERM,&signal_handler);
}

char *strlwr(char *str) {
  for (unsigned int i=0;i<strlen(str);i++)
    str[i]=tolower(str[i]);
  return str;
}

char *strupr(char *str) {
  for (unsigned int i=0;i<strlen(str);i++)
    str[i]=toupper(str[i]);
  return str;
}

/*!
\brief Sleeps some milliseconds
\author AnomCwrd

  thread-safe
*/
void Sleep(unsigned long msec)
{
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = msec * 1000;
	select(0, 0, 0, 0, &timeout);
}

static volatile bool g_bHUPReceived = false; // Xan : this is not thread safe, but worst case
				//       we skip an HangUP.. shouldn't be much a
				//       problem.
static volatile bool g_bShouldClose = false;


/*!
\brief Checks for a SIGHUP was handled
\author Xanatar
\return true if a SIGHUP was handled

Can miss a HUP, but should never be a problem
*/
bool pollHUPStatus ()
{
	if (g_bHUPReceived) {
		g_bHUPReceived = false;
		return true;
	}
	return false;
}


/*!
\brief Checks for a SIGQUIT (or similar) was handled
\author Xanatar
\return true if a SIGQUIT was handled
*/
bool pollCloseRequests()
{
	return g_bShouldClose;
}


/*!
\brief Signal-handling thread
\author AnomCwrd
*/
static void* SignalThread(void*)
{
	sigset_t signals_to_catch;

		// clear out the list of signals to catch
	sigemptyset(&signals_to_catch);

		// now, start adding signals we care about
	sigaddset(&signals_to_catch, SIGHUP);
	sigaddset(&signals_to_catch, SIGINT);
	sigaddset(&signals_to_catch, SIGQUIT);
	sigaddset(&signals_to_catch, SIGTERM);
	sigaddset(&signals_to_catch, SIGUSR1);
	sigaddset(&signals_to_catch, SIGUSR2);

	// we need to know which signal we caught
	int current_signal;

	for (;;) { // loop forever waiting on signals
		sigwait(&signals_to_catch, &current_signal);

		// check what we caught
		if (current_signal == SIGINT || current_signal == SIGQUIT || current_signal == SIGTERM) {
			// tell others about the signal
			printf("Termination signal handled...\n");
			g_bShouldClose = true;
			return NULL;
			//pthread_exit(0);
		}

		// check for a HUP
		if (current_signal == SIGHUP) {
			g_bHUPReceived = true;
			return NULL;
			//pthread_exit(0);
		}
		Sleep(100);
	}
}


/*!
\brief Starts signal handling thread
\author AnomCwrd
*/
void start_signal_thread()
{
	// frist thing we need to do is setup POSIX signals
	setup_signals();

	// now we can start the signal  thread
	pthread_attr_t thread_attr;
	pthread_t signal_thread_id;

	pthread_attr_init(&thread_attr);
	pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&signal_thread_id, &thread_attr, SignalThread, 0);
}


/*!
\brief Setup signals to handle
\author AnomCwrd
*/
void setup_signals ()
{
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
}

std::string getOSVersionString()
{
	char *temp;
	struct utsname info;
	uname(&info);
	asprintf(&temp, "%s %s on a %s", info.sysname, info.release, info.machine);
	
	std::string s(temp);
	free(temp);
	
	return s;
}

OSVersion getOSVersion()
{
	return OSVER_NONWINDOWS;
}

//@{
/*!
\name Clock functions
*/

unsigned long initialserversec = 0;
unsigned long initialservermill = 0;

void initclock()
{
	timeval t ;
	gettimeofday(&t,NULL) ; // ftime has been obseloated
	initialserversec = t.tv_sec ;
	initialservermill = t.tv_usec/ 1000 ;
}

uint32_t getclockday()
{
	uint32_t seconds;
	uint32_t days ;
	timeval buffer ;
	gettimeofday(&buffer,NULL) ;
	seconds = buffer.tv_sec ;
	days = seconds/86400 ;  // (60secs/minute * 60 minute/hour * 24 hour/day)
	return days ;
}

uint32_t getclock()
{
	uint32_t milliseconds;
	uint32_t seconds ;
	timeval buffer ;
	gettimeofday(&buffer,NULL) ;
	seconds = buffer.tv_sec ;
	milliseconds = buffer.tv_usec/1000 ;
	if (milliseconds < initialservermill)
	{
		milliseconds = milliseconds + 1000 ;
		seconds  = seconds - 1 ;
	}
	milliseconds = ((seconds - initialserversec) * 1000) + (milliseconds -initialservermill ) ;
	return milliseconds ;
}

/*!
\author Keldan
\since 0.82r3
\brief get current system clock time

used by getSystemTime amx function
*/
uint32_t getsysclock()
{
	uint32_t seconds;
	timeval buffer;
	gettimeofday(&buffer,NULL);
	seconds = buffer.tv_sec;
	return seconds;
}

//@}

#endif
