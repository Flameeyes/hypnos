/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief cChar's effects (graphics and sound) methods
*/

/*!
\author Flameeyes (port)
\brief Plays a monster sound effect
\param sfx sound effect
\note ported from sndpkg.cpp, i'm not the original author - Flameeyes
*/
void cChar::playMonsterSound(MonsterSound sfx)
{
	P_CREATURE_INFO creature = creatures.getCreature( getId() );
	if( creature==NULL )
		return;

	UI16 s = creature->getSound( sfx );
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
void cChar::movingFX(P_CHAR destination, short id, SI32 speed, SI32 loop, bool explode, ParticleFx* part)
{
	movingeffect(DEREF_P_CHAR(this), DEREF_P_CHAR(destination), id >> 8, id & 0xFF,
		speed & 0xFF, loop & 0xFF, explode ? '\1' : '\0', part!=NULL, part);
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
void cChar::staticFX(short id, SI32 speed, SI32 loop, ParticleFx* part)
{
	if (part!=NULL) {
		if (id<=-1) id = (part->effect[0] << 8) + part->effect[1];
		if (speed<=-1) speed = part->effect[2];
		if (loop<=-1) loop = part->effect[3];
	}
	staticeffect(DEREF_P_CHAR(this), id >> 8, id & 0xFF, speed, loop, part!=NULL, part);
}

/*!
\brief Bolts a char
\author Xanathar
\param bNoParticles true if NOT to use particles
\todo backport
*/
void cChar::boltFX(bool bNoParticles)
{
	UI08 effect[28]={ 0x70, 0x00, };

 	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var

	Location pos2;
	pos2.x = 0; pos2.y = 0; pos2.z = 0;
	MakeGraphicalEffectPkt_(effect, 0x01, getSerial32(), 0, 0, getPosition(), pos2, 0, 0, 1, 0);

	if (bNoParticles) // no UO3D effect ? lets send old effect to all clients
	{
		 NxwSocketWrapper sw;
		 sw.fillOnline( this, false );
		 for( sw.rewind(); !sw.isEmpty(); sw++ )
		 {
			 NXWSOCKET j=sw.getSocket();
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
			 NXWSOCKET j=sw.getSocket();
			 if( j!=INVALID )
			 {
			 if (clientDimension[j]==2) // 2D client, send old style'd
			 {
				 Xsend(j, effect, 28);
//AoS/				Network->FlushBuffer(j);
			 } else if (clientDimension[j]==3) // 3d client, send 3d-Particles
			 {
				//TODO!!!! fix it!
				//Magic->doStaticEffect(DEREF_P_CHAR(this), 30);
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
	bolteffect2(DEREF_P_CHAR(this),id >> 8,id & 0xFF);
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
	if( !isPermaHidden() )
	{
		Location charpos= getPosition();

		switch( gmMoveEff )
		{
		case 1:	// flamestrike
			staticeffect3(charpos.x+1, charpos.y+1, charpos.z+10, 0x37, 0x09, 0x09, 0x19, 0);
			client->playSFX( 0x0802);
			break;

		case 2: // sparklie (fireworks wand style)
			staticeffect3(charpos.x+1, charpos.y+1, charpos.z+10, 0x37, 0x3A, 0x09, 0x19, 0); break;

		case 3: // sparklie (fireworks wand style)
			staticeffect3(charpos.x+1, charpos.y+1, charpos.z+10, 0x37, 0x4A, 0x09, 0x19, 0); break;

		case 4: // sparklie (fireworks wand style)
			staticeffect3(charpos.x+1, charpos.y+1, charpos.z+10, 0x37, 0x5A, 0x09, 0x19, 0); break;

		case 5: // sparklie (fireworks wand style)
			staticeffect3(charpos.x+1, charpos.y+1, charpos.z+10, 0x37, 0x6A, 0x09, 0x19, 0); break;

		case 6: // sparklie (fireworks wand style)
			staticeffect3(charpos.x+1, charpos.y+1, charpos.z+10, 0x37, 0x7A, 0x09, 0x19, 0); break;
		}
	}
	return;
}
