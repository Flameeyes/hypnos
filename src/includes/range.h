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

LOGICAL inbankrange(int i);

#endif
