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
\brief Map related functions
*/

#include "common_libs.hpp"
#include "map.hpp"
#include "data.hpp"
#include "inlines.hpp"

//@{
/*!
\author Luxor
\name Client Z-values
\brief Values used by the client to acknowledge Z-moving
*/
	//! If we've got a bridge flagged tile or we're moving in map0
	//! (stairs or whatever) we can elevate our Z by 14.
	static const int8_t MAX_Z_CLIMB_BRIDGE = 14;
	
	//! If we are simply climbing to get on a new surface, the client
	//! tolerates only a Z step of 2.
	static const int8_t MAX_Z_CLIMB = 2;
	
	//! If our Z is lowering more than 6 tiles, we're falling.
	static const int8_t MIN_Z_FALL = 6;
	
	//! A char cannot fall more than 20 tiles in altitude.
	//! \todo Check if this is true
	static const int8_t MAX_Z_FALL = 20;
//@}

/*!
\author Luxor
\brief Constructs a line from a location A to a location B
*/
cLine::cLine( sLocation A, sLocation B )
{
	m_xDist = int32_t( A.x - B.x );
	m_yDist = int32_t( A.y - B.y );
	m_zDist = int32_t( A.z - B.z );
	x1 = A.x;
	y1 = A.y;
	z1 = A.z;
}

/*!
\author Luxor
*/
int8_t cLine::calcZAtX( uint32_t x )
{
	int8_t z;
	z = int8_t( ( float( ( int32_t(x - x1) * m_zDist ) + (z1 * m_xDist) ) / R32( m_xDist ) ) + 0.5);
	return z;
}

/*!
\author Luxor
*/
sLocation cLine::getPosAtX( uint32_t x )
{
	sLocation pos = sLocation( x, 0, 0 );
	pos.y = uint32_t( ( float( ( int32_t(x - x1) * m_yDist ) + ( int32_t(y1) * m_xDist) ) / R32( m_xDist ) ) + 0.5);
	pos.z = calcZAtX( pos.x );
	return pos;
}

/*!
\author Luxor
*/
sLocation cLine::getPosAtY( uint32_t y )
{
	sLocation pos = sLocation( 0, y, 0 );
	pos.x = uint32_t( ( float( ( int32_t(y - y1) * m_xDist ) + ( int32_t(x1) * m_yDist) ) / R32( m_yDist ) ) + 0.5);
	pos.z = calcZAtX( pos.x );
	return pos;
}



/*!
\author Luxor
\brief Looks if a char can walk on the given sLocation
\return The next z value of char position, illegal_z if the tile isn't walkable
*/
int8_t isWalkable( sLocation pos, uint8_t flags, pChar pc )
{
	int8_t zRes = 0;
	int32_t height = 0;

	//
        // DYNAMIC ITEMS -- Check for dynamic items Z elevation and block flag
        //
	if ( flags & WALKFLAG_DYNAMIC ) {
		NxwItemWrapper si;
		pItem pi = NULL;
		si.fillItemsAtXY( pos );

		for( si.rewind(); !si.isEmpty(); si++ )	{
			pi = si.getItem();
			if( !pi )
				continue;

			height = tiledataStatic->getHeight( pi->getId() );
			
			uint32_t tflags = tiledataStatic->getFlags( pi->getId() );
			
			if ( tflags & flagTileBridge ) // Stairs, ladders
				height /= 2;

                	if ( pi->getPosition().z < (pos.z + MaxZstep) ) { // We cannot walk under it
				if ( tflags & flagTileImpassable ) // Block flag
					return illegal_z;

				if ( (pi->getPosition().z + height) <= (pos.z + 3) ) { // We can walk on it
	                                if ( (pi->getPosition().z + height) > zRes )
						zRes = pi->getPosition().z + height;
				} else// if ( pi->type != 12 ) // Doors can be opened or avoided by passing under them
					return illegal_z;
                	}
		}
	} // WALKFLAG_DYNAMIC

        //
        // MAP -- Check for map Z elevation and denied textures (as water, mountains etc)
        //
	if ( flags & WALKFLAG_MAP ) {
		int32_t mapid = 0;
		map_st map1;
		data::seekMap( pos.x, pos.y, map1 );
		mapid = map1.id;

		// Z elevation
		if ( map1.z == illegal_z || map1.z > (pos.z + 3) )
			return illegal_z;
		else if ( map1.z > zRes )
			zRes = map1.z;

		// Denied textures
		if (
			(mapid >= 0x00DC && mapid <= 0x00E7) ||
			(mapid >= 0x00EC && mapid <= 0x00F7) ||
			(mapid >= 0x00FC && mapid <= 0x0107) ||
			(mapid >= 0x010C && mapid <= 0x0117) ||
			(mapid >= 0x011E && mapid <= 0x0129) ||
			(mapid >= 0x0141 && mapid <= 0x0144) ||
			(mapid >= 0x01AF && mapid <= 0x01B0) ||
			(mapid >= 0x01DE && mapid <= 0x01DB) ||
			(mapid >= 0x021F && mapid <= 0x0244) ||
			(mapid >= 0x025A && mapid <= 0x026D) ||
			(mapid >= 0x02BC && mapid <= 0x02CB) ||
			(mapid >= 0x06CD && mapid <= 0x06DD) ||
			(mapid >= 0x06EB && mapid <= 0x073E) ||
			(mapid >= 0x0745 && mapid <= 0x075C) ||
			(mapid >= 0x07BD && mapid <= 0x07D4) ||
			(mapid >= 0x0834 && mapid <= 0x08C6)
			)
			return illegal_z;

		if ( !pc || !(pc->nxwflags[0] & cChar::flagWaterWalk) ) {
			if ( mapid >= 0x0A8 && mapid <= 0x0AB) 	// Water
				return illegal_z;
		}
	} // WALKFLAG_MAP

        //
        // STATIC TILES -- Check for static tiles Z elevation
        //
	if ( flags & WALKFLAG_STATIC ) {

		staticVector s;
		data::collectStatics( pos.x, pos.y, s );

		for( uint32_t i = 0; i < s.size(); i++ )
		{
			uint32_t tflags = tiledataStatic->getFlags( pi->getId() );

			// Z elevation
			int8_t theight = height = tiledataStatic->getHeight( pi->getId() );
			if ( tflags & flagTileBridge ) // Stairs, ladders
				height /= 2;

			if ( s[i].z < (pos.z + MaxZstep) ) { // We cannot walk under it
				if ( tflags & flagTileImpassable ) // Block flag
					return illegal_z;

				if ( (s[i].z + height) <= (pos.z + 3) ) { // We can walk on it
					if ( (s[i].z + theight) > zRes )
						zRes = s[i].z + theight;
				} else
					return illegal_z;
			}
		}
	} // WALKFLAG_STATIC

	//
	// CHARACTERS -- Check for characters at given position
	//
	if ( flags & WALKFLAG_CHARS ) {
		NxwCharWrapper sc;
		pChar pc_curr = NULL;
		sc.fillCharsAtXY( pos );

		for( sc.rewind(); !sc.isEmpty(); sc++ )	{
			pc_curr = sc.getChar();
			if ( ! pc_curr )
				continue;

			if ( abs( pc_curr->getPosition().z - zRes ) < MaxZstep )
				return illegal_z;
		}
	} // WALKFLAG_CHARS

	return zRes;
}



/*!
\author Luxor
\brief Tells if the line of sight between two locations is not interrupted
\return True if the line of sight is clean, false if not
*/
bool lineOfSight( sLocation A, sLocation B )
{
	cLine line( A, B );

	uint32_t max_x = max( A.x, B.x );
	uint32_t max_y = max( A.y, B.y );
	uint32_t max_i = max( max_x, max_y );
	uint32_t i = ( max_i == max_x ) ? min( A.x, B.x ) : min( A.y, B.y );

	sLocation pos;
	for ( i++; i < max_i; i++ ) {
		pos = ( max_i == max_x ) ? line.getPosAtX( i ) : line.getPosAtY( i );
		if ( isWalkable( pos, WALKFLAG_DYNAMIC+WALKFLAG_MAP+WALKFLAG_STATIC ) == illegal_z )
			return false;
	}
	return true;
}

/*!
\author Luxor
\brief Tells if an npc can move in the given position
\todo Implement special features based on the npc (fire elemental should walk on lava passages etc...)
*/
bool canNpcWalkHere( sLocation pos )
{
	return ( isWalkable( pos ) != illegal_z );
}

/*!
\author Luxor
*/
int8_t staticTop( sLocation pos )
{
	int8_t max_z = illegal_z, temp_z;

	staticVector s;
	data::collectStatics( pos.x, pos.y, s );
	for( uint32_t i = 0; i < s.size(); i++ ) {
		temp_z = s[i].z + tileHeight( s[i].id );
		if ( temp_z < ( MaxZstep + pos.z ) && temp_z > max_z )
			max_z = temp_z;
	}
	return max_z;
}

/*!
\author Luxor
*/
int8_t tileHeight( uint16_t id )
{
	int8_t height = tiledataStatic->getHeight(id);
	if ( tiledataStatic->getFlags(id) & flagTileBridge )
		height /= 2;

	return height;
}

/*!
\author Luxor
*/
int8_t mapElevation( sPoint p )
{
	map_st m;
	if ( !data::seekMap( p.x, p.y, m ) )
		return illegal_z;
	return m.z;
}

/*!
\author Luxor
*/
static int8_t mapAverageElevation( sPoint p )
{
	int8_t map1_z = mapElevation( p );
	if ( map1_z == illegal_z )
		return illegal_z;

	int8_t map2_z = mapElevation( p + sPositonOffset(1,0) );
	int8_t map3_z = mapElevation( p + sPositonOffset(0,1) );
	int8_t map4_z = mapElevation( p + sPositonOffset(1,1) );
	int8_t z;

	if ( abs( map1_z - map4_z ) <= abs( map2_z - map3_z ) ) {
		if ( map4_z == illegal_z )
			return map1_z;
		z = (int8_t)( ( map1_z + map4_z ) >> 1 );
		if ( z % 2 < 0 )
			z--;
		return z;
	} else {
		if ( map2_z == illegal_z || map3_z == illegal_z )
			return map1_z;
		z = (int8_t)( ( map2_z + map3_z ) >> 1 );
		if ( z % 2 < 0 )
			z--;
		return z;
	}

	return illegal_z;
}

/*!
\author Luxor
*/
int8_t dynamicElevation( sLocation pos )
{
	int8_t max_z = illegal_z, temp_z;
	NxwItemWrapper si;
	si.fillItemsAtXY( pos.x, pos.y );
	for( si.rewind(); !si.isEmpty(); si++ ) {
		pItem pi = si.getItem();

		temp_z = pi->getPosition().z + tileHeight( pi->getId() );
		if ( temp_z < ( pos.z + MaxZstep ) && temp_z > max_z )
			max_z = temp_z;
	}
	return max_z;
}

/*!
\author Luxor
\brief Returns the estimated height of walker's position.
*/
int8_t getHeight( sLocation pos )
{
	int8_t final_z = illegal_z, item_z = illegal_z, temp_z, base_z;
	uint32_t item_flags;

	NxwItemWrapper si;
	si.fillItemsAtXY( pos.x, pos.y );
	for( si.rewind(); !si.isEmpty(); si++ ) {
		pItem pi = si.getItem();

		uint32_t tflags = tiledataStatic->getFlags( pi->getId() );
		base_z = tflags & flagTileBridge ? pi->getPosition().z : pi->getPosition().z + tiledataStatic->getHeight(pi->getId());
		temp_z = pi->getPosition().z + tile.height;

		// Check if the tile is reachable.
		if ( base_z <= pos.z + MAX_Z_CLIMB && temp_z >= pos.z - MAX_Z_FALL ) {
			if (
				// Nothing has been chosen until now: pick up this as the first valid Z value.
				( item_z == illegal_z ) ||
				// We're choosing between two bridges: choose the highest one
				( item_flags & flagTileBridge && tflags & flagTileBridge && temp_z > item_z ) ||
				// We're choosing between two non-bridges: choose the highest one
				( !(item_flags & flagTileBridge) && !(tflags & flagTileBridge) && temp_z > item_z )

			) {
				item_z = temp_z;
				item_flags = tflags;
			}

			// We're choosing between a bridge and a non bridge: choose the highest one if the bridge is reachable only by falling down. Otherwise choose the bridge.
			if ( !(item_flags & flagTileBridge) && tflags & flagTileBridge ) {
				if ( temp_z >= pos.z - MIN_Z_FALL ) {
					item_z = temp_z;
					item_flags = tflags;
				}
			} else if ( item_flags & flagTileBridge && !(tflags & flagTileBridge) ) {
				if ( item_z < pos.z - MIN_Z_FALL && temp_z > item_z ) {
					item_z = temp_z;
					item_flags = tflags;
				}
			}
		}
	}

	staticVector s;
	data::collectStatics( pos.x, pos.y, s );
	for( uint32_t i = 0; i < s.size(); i++ )
	{
		uint32_t tflags = tiledataStatic->getFlags(s[i].id);

		base_z = tflags & flagTileBridge ? s[i].z : s[i].z + tiledataStatic->getHeight(s[i].id);
		temp_z = s[i].z + tile.height;

		// Check if the tile is reachable.
		if ( base_z <= pos.z + MAX_Z_CLIMB && temp_z >= pos.z - MAX_Z_FALL ) {
			if (
				// Nothing has been chosen until now: pick up this as the first valid Z value.
				( item_z == illegal_z ) ||
				// We're choosing between two bridges: choose the highest one
				( item_flags & flagTileBridge && tflags & flagTileBridge && temp_z > item_z ) ||
				// We're choosing between two non-bridges: choose the highest one
				( !(item_flags & flagTileBridge) && !(tflags & flagTileBridge) && temp_z > item_z )

			) {
				item_z = temp_z;
				item_flags = tflags;
			}

			// We're choosing between a bridge and a non bridge: choose the highest one if the bridge is reachable only by falling down. Otherwise choose the bridge.
			if ( !(item_flags & flagTileBridge) && tflags & flagTileBridge ) {
				if ( temp_z >= pos.z - MIN_Z_FALL ) {
					item_z = temp_z;
					item_flags = tflags;
				}
			} else if ( item_flags & flagTileBridge && !(tflags & flagTileBridge) ) {
				if ( item_z < pos.z - MIN_Z_FALL && temp_z > item_z ) {
					item_z = temp_z;
					item_flags = tflags;
				}
			}
		}
	}

	int8_t map_z = mapAverageElevation( pos.x, pos.y );
	if (
		// No Z value was found yet.
		( item_z == illegal_z ) ||
		// We're choosing between map and bridge: choose the highest one if the bridge is reachable only by falling down. Otherwise choose the bridge.
		( item_flags & TILEFLAG_BRIDGE && item_z < pos.z - MIN_Z_FALL ) ||
		// We're choosing between map and normal item: choose the highest one if the item is reachable only by falling down and map is reachable.
		( map_z <= pos.z + MAX_Z_CLIMB_BRIDGE && item_z < pos.z - MIN_Z_FALL )
	) {
		final_z = qmax( map_z, item_z );
	} else
		final_z = item_z;

	return final_z;
}
