/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\author Luxor
\brief Header of Map related functions
*/

#ifndef __MAP_H__
#define __MAP_H__

#include "structs.hpp"

/*!
\author Luxor
\brief Check flags for isWalkable function
*/
enum {
	WALKFLAG_NONE = 0x0, WALKFLAG_MAP = 0x1, WALKFLAG_STATIC = 0x2, WALKFLAG_DYNAMIC = 0x4, WALKFLAG_CHARS = 0x8, WALKFLAG_ALL = 0xF
};

class cLine {
public:
	cLine( sLocation A, sLocation B );
	sLocation getPosAtX( uint32_t x );
	sLocation getPosAtY( uint32_t y );
	int8_t calcZAtX( uint32_t x );
private:
	uint32_t x1, y1;
	int8_t z1;
	int32_t m_xDist, m_yDist, m_zDist;
};

int8_t isWalkable( sLocation pos, uint8_t flags = WALKFLAG_ALL, pChar pc = NULL );
bool lineOfSight( sLocation pos1, sLocation pos2 );
bool canNpcWalkHere( sLocation pos );
int8_t staticTop( sLocation pos );
int8_t tileHeight( uint16_t id );
int8_t mapElevation( sPoint p );
int8_t dynamicElevation( sLocation pos );
int8_t getHeight( sLocation pos );

inline int32_t line_of_sight( int32_t s, sLocation a, sLocation b, int32_t checkfor )
{ return lineOfSight( a, b ); }

inline int32_t line_of_sight( int32_t s, int32_t x1, int32_t y1, int32_t z1, int32_t x2, int32_t y2, int32_t z2, int32_t checkfor )
{ return lineOfSight( sLocation( x1, y1, z1 ), sLocation( x2, y2, z2 ) ); }


#endif //__MAP_H__
