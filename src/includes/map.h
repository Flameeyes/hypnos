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
\author Luxor
\brief Header of Map related functions
*/

#ifndef __MAP_H__
#define __MAP_H__

#include "basics.h"

/*!
\author Luxor
\brief Check flags for isWalkable function
*/
enum {
	WALKFLAG_NONE = 0x0, WALKFLAG_MAP = 0x1, WALKFLAG_STATIC = 0x2, WALKFLAG_DYNAMIC = 0x4, WALKFLAG_CHARS = 0x8, WALKFLAG_ALL = 0xF
};

class cLine {
public:
	cLine( Location A, Location B );
	Location getPosAtX( uint32_t x );
	Location getPosAtY( uint32_t y );
	int8_t calcZAtX( uint32_t x );
private:
	uint32_t x1, y1;
	int8_t z1;
	int32_t m_xDist, m_yDist, m_zDist;
};

int8_t isWalkable( Location pos, uint8_t flags = WALKFLAG_ALL, P_CHAR pc = NULL );
LOGICAL lineOfSight( Location pos1, Location pos2 );
LOGICAL canNpcWalkHere( Location pos );
int8_t staticTop( Location pos );
int8_t tileHeight( uint16_t id );
int8_t mapElevation( uint32_t x, uint32_t y );
int8_t dynamicElevation( Location pos );
int8_t getHeight( Location pos );
void getMultiCorners( P_ITEM pi, uint32_t &x1, uint32_t &y1, uint32_t &x2, uint32_t &y2 );

inline int32_t line_of_sight( int32_t s, Location a, Location b, int32_t checkfor )
{ return lineOfSight( a, b ); }

inline int32_t line_of_sight( int32_t s, int32_t x1, int32_t y1, int32_t z1, int32_t x2, int32_t y2, int32_t z2, int32_t checkfor )
{ return lineOfSight( Location( x1, y1, z1 ), Location( x2, y2, z2 ) ); }


#endif //__MAP_H__
