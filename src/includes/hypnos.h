/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Base Emulator's Functions
*/

#ifndef __PYUO_H_
#define __PYUO_H_

#include "typedefs.h"

void StartClasses();
void SetGlobalVars();
void BuildPointerArray();
void InitMultis();
void DeleteClasses();
void telltime( NXWCLIENT ps );
void npcattacktarget(P_CHAR npc, P_CHAR target);
void enlist(int s, int listnum);		//!< For enlisting in army
void gcollect();
void initque();
void StoreItemRandomValue(P_ITEM pi,int tmpreg);
void loadmetagm();

// Profiling
void StartMilliTimer(unsigned long &Seconds, unsigned long &Milliseconds);
unsigned long CheckMilliTimer(unsigned long &Seconds, unsigned long &Milliseconds);

void callguards( int p );
void usepotion(P_CHAR pc, P_ITEM pi);

void newScriptsInit();
void deleteNewScripts();

#endif
