/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
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
\param faxey Y-coord to face to
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
void cChar::pathFind( Location pos, bool bOverrideCurrentPath )
{
	if ( hasPath() ) {
		if ( bOverrideCurrentPath )
			safedelete( path );
		else
			return;
	}

        bool bOk = true;
	Location loc = pos;
	if ( isWalkable( pos, WALKFLAG_ALL, this ) == illegal_z ) { // If it isn't walkable, we can only reach the nearest tile
		bOk = false;
		for ( uint32_t i = 1; i < 4; i++ ) {
                        // East
			loc = Location( pos.x + i, pos.y, pos.z );
			if ( isWalkable( loc, WALKFLAG_ALL, this ) != illegal_z ) {
				bOk = true;
				break;
			}

			// West
			loc = Location( pos.x - i, pos.y, pos.z );
			if ( isWalkable( loc, WALKFLAG_ALL, this ) != illegal_z ) {
				bOk = true;
				break;
			}

			// South
			loc = Location( pos.x, pos.y + i, pos.z );
			if ( isWalkable( loc, WALKFLAG_ALL, this ) != illegal_z ) {
				bOk = true;
				break;
			}

			// North
			loc = Location( pos.x, pos.y - i, pos.z );
			if ( isWalkable( loc, WALKFLAG_ALL, this ) != illegal_z ) {
				bOk = true;
				break;
			}

			// North-East
			loc = Location( pos.x + i, pos.y - i, pos.z );
			if ( isWalkable( loc, WALKFLAG_ALL, this ) != illegal_z ) {
				bOk = true;
				break;
			}

			// North-West
			loc = Location( pos.x - i, pos.y - i, pos.z );
			if ( isWalkable( loc, WALKFLAG_ALL, this ) != illegal_z ) {
				bOk = true;
				break;
			}

			// South-East
			loc = Location( pos.x + i, pos.y + i, pos.z );
			if ( isWalkable( loc, WALKFLAG_ALL, this ) != illegal_z ) {
				bOk = true;
				break;
			}

			// South-West
			loc = Location( pos.x - i, pos.y + i, pos.z );
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

	Location pos = path->getNextPos();

	if ( pos == getPosition() )
		return;

	if ( isWalkable( pos, WALKFLAG_DYNAMIC|WALKFLAG_CHARS, this ) == illegal_z ) {
                safedelete( path );
		return;
	}


	P_CREATURE_INFO creature = creatures.getCreature( getId() );
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
	VALIDATEPC( pc );

	if( seconds!=INVALID )
		fleeTimer=uiCurrentTime +MY_CLOCKS_PER_SEC*seconds;
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
	pChar pc_att = pointers::findCharBySerial( attackerserial );
	if ( !ISVALIDPC( pc_att ) )
		pc_att = pointers::findCharBySerial( targserial );
	if ( !ISVALIDPC( pc_att ) )
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
			pChar pc = pointers::findCharBySerial( ftargserial );
			if ( !ISVALIDPC( pc ) )
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
			pChar target = pointers::findCharBySerial( targserial );
			if (ISVALIDPC(target)) {
				if ( distFrom( target ) < VISRANGE )
					getDirFromXY( target->getPosition().x, target->getPosition().y );
				npcwalk( this, npcSelectDir( this, (  getDirFromXY( target->getPosition().x, target->getPosition().y ) +4 )%8 )%8,0);
			}
		}
			break;
		case WANDER_AMX: // Sparhawk: script controlled movement
		{
			uint32_t l = dir;
			if (amxevents[EVENT_CHR_ONWALK])
			{
				g_bByPass = false;
				amxevents[EVENT_CHR_ONWALK]->Call(getSerial(), dir, dir);
				if (g_bByPass==true)
					return;
			}
			/*
			pc_i->runAmxEvent( EVENT_CHR_ONWALK, pc_i->getSerial(), pc_i->dir, pc_i->dir);
			if (g_bByPass==true)
				return;
			*/
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
	Location pcpos= getPosition();

	if ( targetX < pcpos.x )
		if ( targetY < pcpos.y )
			direction = NORTHWEST;
		else if ( targetY > pcpos.y )
			direction = SOUTHWEST;
		else
			direction = WEST;
	else
		if ( targetX > pcpos.x )
			if ( targetY < pcpos.y )
				direction = NORTHEAST;
			else if ( targetY > pcpos.y )
				direction = SOUTHEAST;
			else
				direction = EAST;
		else if ( targetY < pcpos.y )
			direction = NORTH;
		else
			direction = SOUTH;
	return direction;
}
