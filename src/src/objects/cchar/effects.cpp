/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief cChar's effects (graphics and sound) methods
*/

/*!
\author Luxor
\brief Does a missed sound effect for combat
*/
void cChar::doMissedSoundEffect()
{
	uint8_t a=RandomNum(0,2);

	switch (a)
	{
		case 0: playSFX(0x0238); break;
		case 1: playSFX(0x0239); break;
		default: playSFX(0x023A);
	}
}

/*!
\author Flameeyes (port)
\brief Plays a monster sound effect
\param sfx sound effect
\note ported from sndpkg.cpp, i'm not the original author - Flameeyes
*/
void cChar::playMonsterSound(MonsterSound sfx)
{
	pCreatureInfo creature = creatures.getCreature( getId() );
	if( creature==NULL )
		return;

	uint16_t s = creature->getSound( sfx );
	if( s != 0xFFFF )
		client->playSFX( s );
}



/*!
\brief Function for the different gm movement effects
\author Aldur
\remarks
	\remark if we can find new effects they can be added here and will be active
	for 'go 'goiter 'goplace 'whilst and 'tell for gm's and counselors
	\remark
		\li 0 = none
		\li 1 = flamestrike
		\li 2 - 6 = different sparkles
*/
void cChar::doGmEffect()
{
	if ( isPermaHidden() )
		return;
	
	switch( gmMoveEff )
	{
	case 1:	// flamestrike
		staticeffect3( sLocation(getPosition().x+1, getPosition().y+1, getPosition().z+10), 0x3709, 9, 25, 0);
		client->playSFX( 0x0802);
		break;

	case 2: // sparklie (fireworks wand style)
		staticeffect3( sLocation(getPosition().x+1, getPosition().y+1, getPosition().z+10), 0x373A, 9, 25, 0);
		break;

	case 3: // sparklie (fireworks wand style)
		staticeffect3( sLocation(getPosition().x+1, getPosition().y+1, getPosition().z+10), 0x374A, 9, 25, 0);
		break;

	case 4: // sparklie (fireworks wand style)
		staticeffect3( sLocation(getPosition().x+1, getPosition().y+1, getPosition().z+10), 0x375A, 9, 25, 0);
		break;

	case 5: // sparklie (fireworks wand style)
		staticeffect3( sLocation(getPosition().x+1, getPosition().y+1, getPosition().z+10), 0x376A, 9, 25, 0);
		break;

	case 6: // sparklie (fireworks wand style)
		staticeffect3( sLocation(getPosition().x+1, getPosition().y+1, getPosition().z+10), 0x377A, 9, 25, 0);
		break;
	}
}

/*!
\brief Makes the char doing an action
\author Luxor
*/
void cChar::playAction(uint16_t action)
{
	switch (action)
	{
		case 0x1A:// Mining-Gravedigging
		case 0x0B:
			if (onhorse)
				action = 0x1A;
			else
				action = 0x0b;
			break;
		case 0x1C:// LumberJacking-Bowcraft
		case 0x0D:
			if (onhorse)
				action = 0x1C;
			else
				action = 0x0D;
			break;
		case 0x1D:// Swordtarget
			// case 0x0D:
			if (onhorse)
				action = 0x1D;
			else
				action = 0x0D;
			break;
		case 0x0A:// Fist Fighting
			if (onhorse)
				action = 0x1A;
			else
				action = 0x0A;
			break;
		case 0x0E:// Smelting irons
			if (onhorse)
				action = 0x1C;
			else
				action = 0x0E;
			break;
		case 0x09:// Working ingots
			if (onhorse)
				action = 0x1A;
			else
				action = 0x09;
			break;
		case 0x14:// These can be done only if not onhorse
		case 0x22:
			if (onhorse)
				action = 0x00;
			break;
		default:
			break;
	}

	if ( ! action )
		return;

	nPackets::Sent::Action pa(this, action);

	cClientSet cs;
	cs.fillOnline( this, false );

	for( cs.rewind(); !cs.isEmpty(); cs++ )
		cs.sendPacket(&pa);
}

void cChar::impAction(uint16_t action)
{
	if ( isMounting() && (action==0x10 || action==0x11))
	{
		playAction(0x1b);
		return;
	}
	if ( isMounting() || ( getId() < 0x190 ) && action == 0x22 )
		return;
	playAction(action);
}

/*!
\brief plays the combat animation
\author Luxor
*/
void cChar::playCombatAction()
{
	if ( !body->isHuman() ) {
		playAction(4+rand()%3);
		playMonsterSound(SND_ATTACK);
	} else if ( onhorse ) {
		combatOnHorse();
	} else {
		combatOnFoot();
	}
}
