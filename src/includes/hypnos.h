/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Base Emulator's Functions
*/

#ifndef __HYPNOS_H_
#define __HYPNOS_H_

#include "typedefs.h"
#include "libhypnos/muls/mulfiles.h"

void StartClasses();
void SetGlobalVars();
void BuildPointerArray();
void DeleteClasses();
void StoreItemRandomValue(pItem pi,int tmpreg);
void loadmetagm();

// Profiling
void StartMilliTimer(unsigned long &Seconds, unsigned long &Milliseconds);
unsigned long CheckMilliTimer(unsigned long &Seconds, unsigned long &Milliseconds);

void newScriptsInit();
void deleteNewScripts();

extern nMULFiles::fTiledataLand *tiledataLand;
extern nMULFiles::fTiledataStatic *tiledataStatic;

#endif
