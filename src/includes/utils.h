  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

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

// Day and Night related prototypes
void setabovelight(unsigned char);

void scriptcommand (pClient client, std::string script1, std::string script2);
void endmessage(int x);
void dooruse(pClient client, pItem pi/*int item*/);
int calcGoodValue(int npcnum, int i, int value,int goodtype); // by Magius(CHE) for trade system

int whichbit( int number, int bit );
int numbitsset( int number );

void getSextantCoords(int32_t x, int32_t y, bool t2a, char *sextant);
void splitline();

int hexnumber(int countx);

void location2xyz(int loc, int& x, int& y, int& z);

uint32_t getclock();
uint32_t getsysclock();
uint32_t getclockday();
void initclock();

//@{
/*!
\name Strings
\brief Strings functions
*/

char *linestart(char *line);
void strupr(std::string &str);
void strlwr(std::string &str);

int strtonum(int countx, int base= 0);
//@}

#endif
