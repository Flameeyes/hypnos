/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "debug.h"
#include "version.h"
#include "inlines.h"

#include <signal.h>

//for Linux exception handling
//we use it to emulate asynch exceptions in a synch system :)
bool g_bExceptionCaught = false;

// Akron
// newIsDebuggerPresent and canBreakpoint are used only when compiling with MSVC++
// so them need to be compiled only if _MSC_VER is defined

void signalhandler (int x)
{ g_bExceptionCaught = true; }

void interrupthandler (int x)
{ keeprun = false; }


void initSignalHandlers()
{
	g_bExceptionCaught = false;
    start_signal_thread();
/*
#ifdef __unix__
	signal(SIGSEGV, signalhandler);
	signal(SIGILL, signalhandler);
	signal(SIGFPE, signalhandler);
	signal(SIGABRT, signalhandler);
	signal(SIGINT, interrupthandler);
	signal(SIGTERM, interrupthandler);
#endif
*/
}

#define BOUNDCHECKING
//Xan : when boundchecking, we often must use release rtl in debug builds
#ifdef BOUNDCHECKING
extern "C" {
int _CrtDbgReport( int reportType, const char *filename, int linenumber, const char *moduleName, const char *format, ... )
{
	return 0;
}
}
#endif







