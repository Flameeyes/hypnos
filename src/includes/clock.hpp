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
\brief Functions to handle clock
*/

#ifndef __CLOCK_H__
#define __CLOCK_H__

#include "common_libs.hpp"

extern uint32_t initialServerSecs;
extern uint32_t initialServermSecs;

void getClock(uint32_t &secs, uint32_t &msecs);

inline void initclock()
{ getClock(initialServerSecs, initialServermSecs); }

uint32_t getClockmSecs();
uint32_t getClockSys();
uint32_t getClockDay();

#define TIMEOUT(X) ((X) <= getClockmSecs())

#endif
