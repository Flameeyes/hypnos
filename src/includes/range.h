  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#ifndef __RANGE_H__
#define __RANGE_H__

#include "globals.h"
#include "basics.h"

#define VERY_VERY_FAR 50000;

R64 dist( Location a, Location b, LOGICAL countZ = (server_data.disable_z_checking == 0) );

/*!
\deprecated Too many parameters!!!
*/
inline R64 dist( UI32 xa, UI32 ya, SI08 za, UI32 xb, UI32 yb, SI08 zb, LOGICAL countZ = (server_data.disable_z_checking == 0) )
{
	return dist( Location( xa, ya, za ), Location( xb, yb, zb ), countZ );
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
inline const UI16 item_dist(pChar a, pItem b)
{
	VALIDATEPCR(a, VERY_VERY_FAR);
	return a->distFrom( b );
}

LOGICAL inbankrange(int i);

#endif
