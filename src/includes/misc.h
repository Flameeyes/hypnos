/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __MISC_H__
#define __MISC_H__

#include "common_libs.h"
#include "structs.h"

void npcattacktarget(pChar npc, pChar target);
void callguards( int p );
void usepotion(pChar pc, pItem pi);

int checkBoundingBox(int xPos, int yPos, int fx1, int fy1, int fz1, int fx2, int fy2);
int checkBoundingCircle(int xPos, int yPos, int fx1, int fy1, int fz1, int radius);
void npcsimpleattacktarget(int target2, int target);

void endmessage(int x);
void dooruse(pClient client, pItem pi/*int item*/);
int calcGoodValue(pChar npc, pItem pi, int value,int goodtype); // by Magius(CHE) for trade system

int whichbit( int number, int bit );
int numbitsset( int number );

std::string getSextantCoords(sPoint p, sPoint center);

const double dist( sLocation a, sLocation b, bool countZ = true);

int fillIntArray(char* str, int *array, int maxsize, int defval = -1, int base = baseAuto);
int RandomNum(int nLowNum, int nHighNum);

bool isWaterTile(sPoint pt);
bool isMovable(pChar pc, pItem pi);

#endif
