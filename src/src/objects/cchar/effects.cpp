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
\author Xanathar
\brief Plays a moving effect from this to target char
\param destination the target char
\param id id of the effect
\param speed speed of the effect
\param loop loops
\param explode true if should do a final explosion
\param part particle effects structure
\todo backport
*/
void cChar::movingFX(pChar dst, uint16_t eff, uint8_t speed, uint8_t loop, bool explode, ParticleFx* part)
{
	if ( ! dst ) return;

	uint8_t effect[28]={ 0x70, 0x00, };

	MakeGraphicalEffectPkt_(effect, 0x00, getSerial(), dst->getSerial(), eff, getPosition(), dst->getPosition(), speed, loop, 0, explode);

	if (!part) // no UO3D effect ? lets send old effect to all clients
	{
		NxwSocketWrapper sw;
		sw.fillOnline( );
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			 pClient j =sw.getSocket();
			 pChar pj = cSerializable::findCharBySerial(currchar[j]);
			 if ( src->hasInRange(pj) && pj->hasInRange(dst) && clientInfo[j]->ingame )
			 {
				Xsend(j, effect, 28);
//AoS/				Network->FlushBuffer(j);
			 }
		}
		return;
	} else {
		// UO3D effect -> let's check which client can see it

		NxwSocketWrapper sw;
		sw.fillOnline();
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			 pClient j =sw.getSocket();
			 pChar pj = cSerializable::findCharBySerial(currchar[j]);
			 if ( src->hasInRange(pj) && pj->hasInRange(dst) && clientInfo[j]->ingame )
			 {
				if (clientDimension[j]==2) // 2D client, send old style'd
				{
					Xsend(j, effect, 28);
//AoS/					Network->FlushBuffer(j);
				} else if (clientDimension[j]==3) // 3d client, send 3d-Particles
				{
					movingeffectUO3D(source, dest, part);
					unsigned char particleSystem[49];
					Xsend(j, particleSystem, 49);
//AoS/					Network->FlushBuffer(j);
				}
				else if (clientDimension[j] != 2 && clientDimension[j] !=3 )
					LogError"Invalid Client Dimension: %i\n", clientDimension[j]);
			}
		}
	}
}

void cChar::movingFX2(pItem dest, uint16_t eff, uint8_t speed, uint8_t loop, bool explode)
{
	if ( ! dest ) return;

	uint8_t effect[28]={ 0x70, 0x00, };

	MakeGraphicalEffectPkt_(effect, 0x00, getSerial(), pi->getSerial(), eff, getPosition(), pi->getPosition(), speed, loop, 0, explode ? 0x01 : 0x00);

	NxwSocketWrapper sw;
	sw.fillOnline( );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient j =sw.getSocket();
		if( j!=INVALID )
		{
			Xsend(j, effect, 28);
//AoS/			Network->FlushBuffer(j);
		}
	}
}

/*!
\brief Plays a static effect on a char
\author Xanathar
\param id id of 2d effect; if -1, 2d effect is get from particles obj
\param speed speed of effect, -1 and it will be get from particles data
\param loop loop factor - -1 and it will be get from particles data
\param part optional particles data
\note if part == NULL then id, speed and loop MUST be >= 0
*/
void cChar::staticFX(uint16_t id, uint8_t speed, uint8_t loop, ParticleFx* part)
{
	if (part!=NULL) {
		if (id<=-1) id = (part->effect[0] << 8) + part->effect[1];
		if (speed<=-1) speed = part->effect[2];
		if (loop<=-1) loop = part->effect[3];
	}
	staticeffect(this, id, speed, loop, part!=NULL, part);
}

/*!
\brief Bolts a char
\author Xanathar
\param bNoParticles true if NOT to use particles
\todo backport
*/
void cChar::boltFX(bool bNoParticles)
{
	uint8_t effect[28]={ 0x70, 0x00, };

 	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var

	Location pos2;
	pos2.x = 0; pos2.y = 0; pos2.z = 0;
	MakeGraphicalEffectPkt_(effect, 0x01, getSerial(), 0, 0, getPosition(), pos2, 0, 0, 1, 0);

	if (bNoParticles) // no UO3D effect ? lets send old effect to all clients
	{
		NxwSocketWrapper sw;
		sw.fillOnline( this, false );
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			pClient j =sw.getSocket();
			if( j!=INVALID )
			{
				Xsend(j, effect, 28);
//AoS/				Network->FlushBuffer(j);
			}
		}
		return;
	}
	else
	{
		 NxwSocketWrapper sw;
		 sw.fillOnline( this, false );
		 for( sw.rewind(); !sw.isEmpty(); sw++ )
		 {
			 pClient j =sw.getSocket();
			 if( j!=INVALID )
			 {
			 if (clientDimension[j]==2) // 2D client, send old style'd
			 {
				 Xsend(j, effect, 28);
//AoS/				Network->FlushBuffer(j);
			 } else if (clientDimension[j]==3) // 3d client, send 3d-Particles
			 {
				//!\todo!!!! fix it!
				//Magic->doStaticEffect(DEREF_pChar(this), 30);
				unsigned char particleSystem[49];
				Xsend(j, particleSystem, 49);
//AoS/				Network->FlushBuffer(j);
			 }
			 else if (clientDimension[j] != 2 && clientDimension[j] !=3 ) { sprintf(temp, "Invalid Client Dimension: %i\n",clientDimension[j]); LogError(temp); }
		 }
	   }
	}
}

/*!
\brief Plays <i>circle of blood</i> or similar effect
\author Xanathar
\param id effect id
\todo backport
*/
void cChar::circleFX(short id)
{
	uint8_t effect[28]={ 0x70, 0x00, };

	int x,y;
	Location charpos = getPosition(), pos2;

	y=rand()%36;
	x=rand()%36;

	if (rand()%2==0) x=x*-1;
	if (rand()%2==0) y=y*-1;
	pos2.x = charpos.x + x;
	pos2.y = charpos.y + y;
	if (pos2.x<0) pos2.x=0;
	if (pos2.y<0) pos2.y=0;
	if (pos2.x>6144) pos2.x=6144;
	if (pos2.y>4096) pos2.y=4096;

	charpos.z = 0; pos2.z = 127;
	MakeGraphicalEffectPkt_(effect, 0x00, getSerial(), 0, id, charpos, pos2, 0, 0, 1, 0);

	 NxwSocketWrapper sw;
	 sw.fillOnline( pc );
	 for( sw.rewind(); !sw.isEmpty(); sw++ )
	 {
		pClient j =sw.getSocket();
		if( j!=INVALID )
		{
			Xsend(j, effect, 28);
//AoS/			Network->FlushBuffer(j);
		}
	}
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
		staticeffect3( Location(getPosition().x+1, getPosition().y+1, getPosition().z+10), 0x3709, 9, 25, 0);
		client->playSFX( 0x0802);
		break;

	case 2: // sparklie (fireworks wand style)
		staticeffect3( Location(getPosition().x+1, getPosition().y+1, getPosition().z+10), 0x373A, 9, 25, 0);
		break;

	case 3: // sparklie (fireworks wand style)
		staticeffect3( Location(getPosition().x+1, getPosition().y+1, getPosition().z+10), 0x374A, 9, 25, 0);
		break;

	case 4: // sparklie (fireworks wand style)
		staticeffect3( Location(getPosition().x+1, getPosition().y+1, getPosition().z+10), 0x375A, 9, 25, 0);
		break;

	case 5: // sparklie (fireworks wand style)
		staticeffect3( Location(getPosition().x+1, getPosition().y+1, getPosition().z+10), 0x376A, 9, 25, 0);
		break;

	case 6: // sparklie (fireworks wand style)
		staticeffect3( Location(getPosition().x+1, getPosition().y+1, getPosition().z+10), 0x377A, 9, 25, 0);
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

	nPackets::Sent::Action pa(serial, action);

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
