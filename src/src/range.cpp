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
\brief Check if a location is in the range of another one
\param a Base location to check
\param b Location to check if in range
\param range range to use
*/
inline const bool inRange(const Location a, const Location b, const UI16 range = VISRANGE)
{
	return (dist(a,b)<= range);
}

/*!
\brief Check if a location is in the visual range of another one
\param a Base location to check
\param b Location to check if in range
\deprecated Should be changed to inRange() function with default range paramter
*/
inline const bool inVisRange(const Location a, const Location b)
{
	return (dist(a,b)<=VISRANGE);
}

/*!
\brief Check if two characters are in visual range
\param a First char to check
\param b Second char to check
\deprecated Should be changed to inRange() function with default range paramter
*/
inline const bool char_inVisRange(pChar a, pChar b)
{
	 return char_inRange( a, b, VISRANGE );
}

/*!
\brief Check if an item is in the visual range of a char
\param a Char to check the range of
\param b Item to check if in range
\depreated Should be changed to inRange() function with default range parameter
*/
inline const bool item_inVisRange(pChar a, pItem b )
{
	 return item_inRange( a, b, VISRANGE );
}

/*!
\brief Check if two characters are in a definited range
\param a First char
\param b Second char
\param range Range to check for
*/
inline const bool inRange(pChar a, pChar b, UI16 range = VISRANGE)
{
	if ( ! a || ! b )
		return false;
	return ( a->distFrom( b ) <= range );
}

/*!
\brief Check if an item is in the range of character
\param a The char
\param b The item
\param range Range to check for
*/
inline const bool inRange(pChar a, pItem b, UI16 range = VISRANGE)
{
	if( ! a || ! b )
		return false;
	return ( a->distFrom( b ) <= range );
}

/*!
\brief Return the distance between a char and a item
\param a The char
\param b The item
\deprecated use directly cChar::distFrom() function
*/
UI32 item_dist(P_CHAR a, P_ITEM b)
{
	VALIDATEPCR(a, VERY_VERY_FAR);
	return a->distFrom( b );
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

