/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Output notifying functions
*/

#ifndef __BACKEND_NOTIFY_H__
#define __BACKEND_NOTIFY_H__

#include "common_libs.h"

namespace nNotify {
	enum Level {
		levPlain,	//!< Plaint non-formatted output
		levError,	//!< Error output
		levWarning,	//!< Warning output
		levInformation,	//!< Information output
		levPanic	//!< Fatal error output
	};
	
	void outputf(Level lev, char *txt, ...) PRINTF_LIKE(2, 3);
	void output(Level lev, const std::string str);
	
	void getDate();
}

#define outPlain(x) nNotify::output(nNotify::levPlain, x)
#define outPlainf(...) nNotify::outputf(nNotify::levPlain, __VA_ARGS__)

#define outError(x) nNotify::output(nNotify::levError, x)
#define outErrorf(...) nNotify::outputf(nNotify::levError, __VA_ARGS__)

#define outWarning(x) nNotify::output(nNotify::levWarning, x)
#define outWarningf(...) nNotify::outputf(nNotify::levWarning, __VA_ARGS__)

#define outInfo(x) nNotify::output(nNotify::levInformation, x)
#define outInfof(...) nNotify::outputf(nNotify::levInformation, __VA_ARGS__)

#define outPanic(x) nNotify::output(nNotify::levPanic, x)
#define outPanicf(...) nNotify::outputf(nNotify::levPanic, __VA_ARGS__)

#endif
