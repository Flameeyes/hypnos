  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "nxwcommn.h"
#include "range.h"
#include "npcai.h"
#include "set.h"
#include "items.h"
#include "chars.h"

/*!
\author Luxor
\brief Returns distance between two points.
*/
const R64 dist( const Location a, const Location b, bool countZ )
{
        SI16 xDiff = a.x - b.x;
        SI16 yDiff = a.y - b.y;
	R64 distance = hypot( abs( xDiff ), abs( yDiff ) );
	if ( !countZ || a.z == b.z )
		return distance;

	R64 distZ = abs( a.z - b.z );
	return hypot( distance, distZ );
}

/*!
\brief Check if a char is near a banker
\param i Character index
\return true if in range, else false
\todo Remove the index, use directly cChar
*/
LOGICAL inbankrange(int i)
{
	P_CHAR pc=MAKE_CHAR_REF(i);
	VALIDATEPCR(pc,false);

	NxwCharWrapper sc;
	sc.fillCharsNearXYZ( pc->getPosition(), 6, true, false );
	for( sc.rewind(); !sc.isEmpty(); sc++ ) {
		P_CHAR pcm=sc.getChar();
	
		if (ISVALIDPC(pcm) && pcm->npcaitype==NPCAI_BANKER)
		{
			return true;
		}
	}
	return false;
}

