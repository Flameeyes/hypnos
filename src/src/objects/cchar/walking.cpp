/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief cChar's walking methods
*/

#include "objects/cchar.h"
#include "walking.h"

/*!
\brief Changes the orientation of a player
\author Flameeyes
\param facex X-coord to face to	
\param facey Y-coord to face to
*/
void cChar::facexy(uint16_t facex, uint16_t facey)
{
	uint8_t olddir = dir;
	dir = getDirFromXY(facex, facey);

	if ( dir != olddir )
		teleport(TELEFLAG_NONE);
}

/*!
\author Luxor
\brief Calls the pathfinding algorithm and creates a new path
*/
void cChar::pathFind( sLocation pos, bool bOverrideCurrentPath )
{
	if ( hasPath() ) {
		if ( bOverrideCurrentPath )
			safedelete( path );
		else
			return;
	}

        bool bOk = true;
	sLocation loc = pos;
	if ( isWalkable( pos, WALKFLAG_ALL, this ) == illegal_z ) { // If it isn't walkable, we can only reach the nearest tile
		bOk = false;
		for ( uint32_t i = 1; i < 4; i++ ) {
                        // East
			loc = sLocation( pos.x + i, pos.y, pos.z );
			if ( isWalkable( loc, WALKFLAG_ALL, this ) != illegal_z ) {
				bOk = true;
				break;
			}

			// West
			loc = sLocation( pos.x - i, pos.y, pos.z );
			if ( isWalkable( loc, WALKFLAG_ALL, this ) != illegal_z ) {
				bOk = true;
				break;
			}

			// South
			loc = sLocation( pos.x, pos.y + i, pos.z );
			if ( isWalkable( loc, WALKFLAG_ALL, this ) != illegal_z ) {
				bOk = true;
				break;
			}

			// North
			loc = sLocation( pos.x, pos.y - i, pos.z );
			if ( isWalkable( loc, WALKFLAG_ALL, this ) != illegal_z ) {
				bOk = true;
				break;
			}

			// North-East
			loc = sLocation( pos.x + i, pos.y - i, pos.z );
			if ( isWalkable( loc, WALKFLAG_ALL, this ) != illegal_z ) {
				bOk = true;
				break;
			}

			// North-West
			loc = sLocation( pos.x - i, pos.y - i, pos.z );
			if ( isWalkable( loc, WALKFLAG_ALL, this ) != illegal_z ) {
				bOk = true;
				break;
			}

			// South-East
			loc = sLocation( pos.x + i, pos.y + i, pos.z );
			if ( isWalkable( loc, WALKFLAG_ALL, this ) != illegal_z ) {
				bOk = true;
				break;
			}

			// South-West
			loc = sLocation( pos.x - i, pos.y + i, pos.z );
			if ( isWalkable( loc, WALKFLAG_ALL, this ) != illegal_z ) {
				bOk = true;
				break;
			}
		}
	}

        if ( bOk )
		path = new cPath( getPosition(), loc, this );
}

/*!
\author Luxor
*/
void cChar::walkNextStep()
{
	if ( isFrozen() )
		return;
	if ( !hasPath() )
		return;
	if ( !path->pathFound() )
		path->exec();

	sLocation pos = path->getNextPos();

	if ( pos == getPosition() )
		return;

	if ( isWalkable( pos, WALKFLAG_DYNAMIC|WALKFLAG_CHARS, this ) == illegal_z ) {
                safedelete( path );
		return;
	}


	pCreatureInfo creature = creatures.getCreature( getId() );
	if( creature!=NULL ) {
		if( creature->canFly() && ( fly_steps>0 ) )
			if ( chance( 20 ) )
				playAction( 0x13 ); // Flying animation
	}

	int8_t dirXY = getDirFromXY( pos.x, pos.y );
	dir = dirXY & 0x0F;
	MoveTo( pos );
	sendToPlayers( this, dirXY );
	setNpcMoveTime();
}

/*!
\brief Flee from target
\author Endymion
\param pc the character
\param seconds the seconds or INVALID if is hp fear
*/
void cChar::flee( pChar pc, int32_t seconds )
{
	if ( ! pc ) return;

	if( seconds!=INVALID )
		fleeTimer=getclock() +SECS*seconds;
	else
		fleeTimer=INVALID;

	oldnpcWander = npcWander;
	npcWander = WANDER_FLEE;
	targserial=pc->getSerial();

}


/*!
\author Luxor
*/
void cChar::follow( pChar pc )
{
	if ( isFrozen() ) {
		if ( hasPath() )
			safedelete( path );
		return;
	}
	if ( dist( getPosition(), pc->getPosition() ) <= 1.0f ) { // Target reached
		if ( hasPath() )
			safedelete( path );
		facexy( pc->getPosition().x, pc->getPosition().y );
		return;
	}
	if ( !hasPath() || path->targetReached() ) { // We haven't got a right path, call the pathfinding.
		pathFind( pc->getPosition(), true );
		walkNextStep();
		return;
	}

	double distance = dist( path->getFinalPos(), pc->getPosition() );
	if ( distance <= 3.0 ) { // Path finalPos is pretty near... let's not overhead the processor
		walkNextStep();
	} else { // Path finalPos is too far, call the pathfinding.
		pathFind( pc->getPosition(), true );
		walkNextStep();
	}
}

/*!
\author Luxor
*/
void cChar::walk()
{
	pChar pc_att = cSerializable::findCharBySerial( attackerserial );
	if ( !pc_att )
		pc_att = cSerializable::findCharBySerial( targserial );
	if ( !pc_att )
		war = 0;

	if ( war && npcWander != WANDER_FLEE && ( pc_att->IsOnline() || pc_att->npc ) ) { //We are following a combat target
                follow( pc_att );
                return;
        }

	switch( npcWander )
	{
		case WANDER_NOMOVE: //No movement
			break;
		case WANDER_FOLLOW: //Follow the follow target
		{
			pChar pc = cSerializable::findCharBySerial( ftargserial );
			if ( !pc )
				break;
			if ( pc->dead )
				break;
			if ( pc->questDestRegion == region )
				((pNPC)this)->clearedEscordQuest( (pPC) pc );
			follow( pc );
		}
			break;
		case WANDER_FREELY_CIRCLE: // Wander freely, in a defined circle
			npcwalk( this, (chance( 20 ) ? rand()%8 : dir), 2 );
			break;
		case WANDER_FREELY_BOX: // Wander freely, within a defined box
			npcwalk( this, (chance( 20 ) ? rand()%8 : dir), 1 );
			break;
		case WANDER_FREELY: // Wander freely, avoiding obstacles
			npcwalk( this, (chance( 20 ) ? rand()%8 : dir), 0 );
			break;
		case WANDER_FLEE: //FLEE!!!!!!
		{
			pChar target = cSerializable::findCharBySerial( targserial );
			if ( target ) {
				if ( distFrom( target ) < VISRANGE )
					getDirFromXY( target->getPosition().x, target->getPosition().y );
				npcwalk( this, npcSelectDir( this, (  getDirFromXY( target->getPosition().x, target->getPosition().y ) +4 )%8 )%8,0);
			}
		}
			break;
		case WANDER_AMX: // Sparhawk: script controlled movement
		{
			uint32_t l = dir;

			pFunctionHandle evt = getEvent(cChar::evtChrOnWalk);
			if( evt ) {
				tVariantVector params = tVariantVector(3);
				params[0] = pc->getSerial(); params[1] = dir; params[2] = dir;
				evt->setParams(params);
				evt->execute();
				if( evt->isBypassed() )
					return;
			}

			int k = dir;
			dir = l;
			l = npcmovetime;
			npcwalk( this, k, 0);
			if ( l != npcmovetime ) // it's been changed through small
				return;
		}
			break;
		default:
			ErrOut("cChar::walk() unknown npcwander [%i] serial %u\n", npcWander, getSerial() );
			break;
	}
	setNpcMoveTime();
}

/*!
\brief Return direction to target coordinate
\param targetX the target X-coordinate
\param targetY the target Y-coordinate
\return the direction to the coordinats
*/
uint8_t cChar::getDirFromXY( uint16_t targetX, uint16_t targetY )
{
	uint8_t direction = dir;
	sLocation pcpos= getPosition();

	if ( targetX < pcpos.x )
		if ( targetY < pcpos.y )
			direction = dirNorthWest;
		else if ( targetY > pcpos.y )
			direction = dirSouthWest;
		else
			direction = dirWest;
	else
		if ( targetX > pcpos.x )
			if ( targetY < pcpos.y )
				direction = dirNorthEast;
			else if ( targetY > pcpos.y )
				direction = dirSouthEast;
			else
				direction = dirEast;
		else if ( targetY < pcpos.y )
			direction = dirNorth;
		else
			direction = dirSouth;
	return direction;
}
