/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __ARCHS_TINTERFACE_H__
#define __ARCHS_TINTERFACE_H__

#include "common_libs.hpp"

#include <wefts_thread.h>

/*!
\brief Abstract class for interfaces

This generic class is inherited by single interfaces (like the console
interface), and is used to send messages to the user.
This replaces the old ConOut() and relatives functions and also my strange
nNotify stuff.
*/
class tInterface : public Wefts::Thread
{
public:
	enum Level {
		levPlain,	//!< Plaint non-formatted output
		levError,	//!< Error output
		levWarning,	//!< Warning output
		levInformation,	//!< Information output
		levPanic	//!< Fatal error output
	};
	
	void outputf(Level lev, char *txt, ...) PRINTF_LIKE(3, 4);
	virtual void output(Level lev, const string &str) = 0;
	
	virtual ~tInterface()
	{ }
	
	static tInterface *instance;
protected:
	tInterface();
};

/*!
\brief Null-output interface

This interface is used by daemons and services which doesn't have an interface
at all, to avoid have tInterface::instance NULL-ified.
*/
class tNullInterface : public tInterface
{
public:
	tNullInterface() : tInterface()
	{ }
	
	void output(tInterface::Level lev, const string &str)
	{ assert(lev >= 0 && (str != "" || str == "")); }
	
	void *run()
	{ return NULL; }
};

#define outPlain(x)	tInterface::instance->output(tInterface::levPlain, x)
#define outPlainf(...)	tInterface::instance->outputf(tInterface::levPlain, __VA_ARGS__)

#define outError(x)	tInterface::instance->output(tInterface::levError, x)
#define outErrorf(...)	tInterface::instance->outputf(tInterface::levError, __VA_ARGS__)

#define outWarning(x)	tInterface::instance->output(tInterface::levWarning, x)
#define outWarningf(...) tInterface::instance->outputf(tInterface::levWarning, __VA_ARGS__)

#define outInfo(x)	tInterface::instance->output(tInterface::levInformation, x)
#define outInfof(...)	tInterface::instance->outputf(tInterface::levInformation, __VA_ARGS__)

#define outPanic(x)	tInterface::instance->output(tInterface::levPanic, x)
#define outPanicf(...)	tInterface::instance->outputf(tInterface::levPanic, __VA_ARGS__)

#endif
