/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Utility functions
*/

#ifndef __UTILS_H__
#define __UTILS_H__

#include "common_libs.h"

int checkBoundingBox(int xPos, int yPos, int fx1, int fy1, int fz1, int fx2, int fy2);
int checkBoundingCircle(int xPos, int yPos, int fx1, int fy1, int fz1, int radius);
int fielddir(pChar pc, int x, int y, int z);
void npcsimpleattacktarget(int target2, int target);

void endmessage(int x);
void dooruse(pClient client, pItem pi/*int item*/);
int calcGoodValue(pChar npc, pItem pi, int value,int goodtype); // by Magius(CHE) for trade system

int whichbit( int number, int bit );
int numbitsset( int number );

void getSextantCoords(int32_t x, int32_t y, bool t2a, char *sextant);
void splitline();

#endif
