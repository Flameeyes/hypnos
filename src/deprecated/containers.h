/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief All containers related stuff
*/

#ifndef _CONTAINERS_H
#define _CONTAINERS_H

void loadcontainers();

typedef struct {
	int16_t x;
	int16_t y;
} BasicPosition;


typedef struct {
	uint32_t gump;
	BasicPosition upperleft;
	BasicPosition downright;
} cont_gump_st;

typedef std::map< uint32_t, cont_gump_st > CONTINFOGUMPMAP;
typedef std::map< uint32_t, CONTINFOGUMPMAP::iterator > CONTINFOMAP;

extern CONTINFOGUMPMAP contInfoGump;
extern CONTINFOMAP contInfo;

#endif
