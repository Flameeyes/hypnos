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

#ifndef WIN32
// For now I hope we can use signals anywhere but Windows, in the future
// we'll see better
#define USE_SIGNALS
#endif

#ifdef USE_SIGNALS

bool pollHUPStatus();
bool pollCloseRequests();
void setup_signals();
void start_signal_thread();

#endif // USE_SIGNALS

#endif // __ARCHS_SIGNALS_H__