/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Effect functions

This file contains the function used for the graphical effects
*/

#include "effects.h"
#include "objects/cpacket.h"
#include "logsystem.h"	

/*!
\brief Plays a moving effect from this to target serializable
\param source Source of the effect
\param destination Target of the effect
\param speed Speed of the effect
\param explode If true should do a final explosion
\param part particle effects structure
*/
void movingFX(pSerializable source, pSerializable destination, uint16_t eff, uint8_t speed, uint8_t loops, bool explode, ParticleFx* part)
{
	if ( !source || !destination ) return;

	nPackets::Sent::GraphicalEffect pk(etBolt, source, destination, eff, speed, loops, false, explode);

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
				j->sendPacket(&pk);
			}
		}
		return;
	}
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
				j->sendPacket(&pk);
			} else if (clientDimension[j]==3) // 3d client, send 3d-Particles
			{
				//! \todo packet 0xc7
				movingeffectUO3D(source, dest, part);
				unsigned char particleSystem[49];
				Xsend(j, particleSystem, 49);
			}
			else if (clientDimension[j] != 2 && clientDimension[j] !=3 )
				LogError"Invalid Client Dimension: %i\n", clientDimension[j]);
		}
	}
}

/*!
\brief Plays a static effect on a serializable
\param source Object where to show the effect
\param eff id of 2d effect; if -1, 2d effect is get from particles obj
\param speed speed of effect, -1 and it will be get from particles data
\param loop loop factor - -1 and it will be get from particles data
\param part optional particles data
\note if part == NULL then id, speed and loop MUST be >= 0
*/
void staticFX(pSerializable source, uint16_t eff, uint8_t speed, uint8_t loop, ParticleFx* part)
{
	if (part)
	{
		if (effect<=-1) eff = (part->effect[0] << 8) + part->effect[1];
		if (speed<=-1) speed = part->effect[2];
		if (loop<=-1) loop = part->effect[3];
	}

	int a0,a1,a2,a3,a4;

	nPackets::Sent::GraphicalEffect pk(etFollowObject, source, NULL, effect, speed, loop, true, false);


	if (!part) // no UO3D effect ? lets send old effect to all clients
	{

		NxwSocketWrapper sw;
		sw.fillOnline( pc, false );
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			pClient client = sw.getSocket();
			client->sendPacket(&pk);
		}
		return;
	}

	// UO3D effect -> let's check which client can see it
	unsigned char particleSystem[49];


	NxwSocketWrapper sw;
	sw.fillOnline( pc, false );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient j =sw.getSocket();
		if(!j) continue;
		if (clientDimension[j]==2) // 2D client, send old style'd
		{
			j->sendPacket(&pk);
		}
		else if (clientDimension[j]==3) // 3d client, send 3d-Particles
		{
			//!\todo packet 0xc7
			
			staticeffectUO3D(player, sta);

			// allow to fire up to 4 layers at same time (like on OSI servers)
			a0 = sta->effect[10];

			a1 = ( ( a0 & 0x000000ff )       );
			a2 = ( ( a0 & 0x0000ff00 ) >> 8  );
			a3 = ( ( a0 & 0x00ff0000 ) >> 16 );
			a4 = ( ( a0 & 0xff000000 ) >> 24 );

			if (a1!=0xff) { particleSystem[46] = a1; Xsend(j, particleSystem, 49); }
			if (a2!=0xff) { particleSystem[46] = a2; Xsend(j, particleSystem, 49); }
			if (a3!=0xff) { particleSystem[46] = a3; Xsend(j, particleSystem, 49); }
			if (a4!=0xff) { particleSystem[46] = a4; Xsend(j, particleSystem, 49); }

//AoS/			Network->FlushBuffer(j);
		}
		else if (clientDimension[j] != 2 && clientDimension[j] !=3 )
			LogError("Invalid Client Dimension: %i\n",clientDimension[j]);
	} // end for

	// remark: if a UO:3D effect is send and ALL clients are UO:3D ones, the pre-calculation of the 2-d packet
	// is redundant. but we can never know, and probably it will take years till the 2d cliet dies.
	// I think it's too infrequnet to consider this as optimization.
}


/*!
\brief Do an effect on a specified location
\param pos Position where to send the effect
\param eff Effect's ID
\param speed Effect's speed
*/
void locationFX(sLocation pos, uint16_t eff, uint8_t speed, uint8_t loop, uint8_t explode)
{

	nPackets::Sent::GraphicalEffect pk(etStayInPlace, pos, pos, eff, speed, loop, true, explode);
	pos.z = 0; //!< \todo verify this...

	NxwSocketWrapper sw;
	sw.fillOnline( pos );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient j =sw.getSocket();
		if( j ) j->sendPacket(&pk);
	}
}

void throwFX(pSerializable source, sLocation pos, uint16_t eff, uint8_t speed, uint8_t loop, bool explode)
{
	if ( ! source ) return;

	nPackets::Sent::GraphicalEffect pk(etBolt, source, pos, eff, speed, loop, false, explode);

	NxwSocketWrapper sw;
	sw.fillOnline( source );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient j =sw.getSocket();
		if( j ) j->sendPacket(&pk);
	}
}

void pullFX(sLocation src_pos, pSerializable destination, uint16_t eff, uint8_t speed, uint8_t loop, bool explode)
{
	if ( ! destination ) return;

	nPackets::Sent::GraphicalEffect pk(etBolt, src_pos, destination, eff, speed, loop, false, explode);

	NxwSocketWrapper sw;
	sw.fillOnline( destination );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient j =sw.getSocket();
		if( j ) j->sendPacket(&pk);
	}
}

void locationtolocationFX(sLocation src_pos, sLocation dst_loc, uint16_t eff, uint8_t speed, uint8_t loop, bool explode)
{

	nPackets::Sent::GraphicalEffect pk(etBolt, src_pos, dst_pos, eff, speed, loop, false, explode);

	NxwSocketWrapper sw;
	sw.fillOnline( src_pos );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient j =sw.getSocket();
		if( j ) j->sendPacket(&pk);
	}
}

/*!
\brief Bolts a cSerializable
\param target Target of the effect
\param bNoParticles If true \b not use particles
*/
void boltFX(pSerializable target, bool bNoParticles)
{
	nPackets::Sent::GraphicalEffect pk(etLightning, target, NULL, 0, 0, 0, true, false);

	if (bNoParticles) // no UO3D effect ? lets send old effect to all clients
	{
		NxwSocketWrapper sw;
		sw.fillOnline( target, false );
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			pClient j =sw.getSocket();
			if( j ) j->sendPacket(&pk);
		}
		return;
	}

	NxwSocketWrapper sw;
	sw.fillOnline( target, false );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient j =sw.getSocket();
		if( j )
		{
			if (clientDimension[j]==2) // 2D client, send old style'd
			{
				j->sendPacket(&pk);
			}
			else if (clientDimension[j]==3) // 3d client, send 3d-Particles
			{
			//!\todo!!!! fix it!
				//Magic->doStaticEffect(DEREF_pChar(this), 30);
				unsigned char particleSystem[49];
				Xsend(j, particleSystem, 49);
//AoS/				Network->FlushBuffer(j);
			}
			else if (clientDimension[j] != 2 && clientDimension[j] !=3 )
			{
				sprintf(temp, "Invalid Client Dimension: %i\n",clientDimension[j]); LogError(temp);
			}
		}
	}
}

/*!
\brief Plays <i>circle of blood</i> or similar effect
\param target Target of the effect
\param id Effect's ID
*/
void circleFX(pSerializable target, uint16_t id)
{

	sint_16 x,y;
	sLocation charpos = target->getPosition(), pos2;

	y=rand()%36;
	x=rand()%36;

	if (!rand()%2) x = -x;
	if (!rand()%2) y = -y;
	pos2.x = charpos.x + x;
	pos2.y = charpos.y + y;
	if (pos2.x<0) pos2.x=0;
	if (pos2.y<0) pos2.y=0;

	//! \todo use a variable or setting or something else for maximum x and y of map
	if (pos2.x>6144) pos2.x=6144;
	if (pos2.y>4096) pos2.y=4096;

	charpos.z = 0; pos2.z = 127;
	nPackets::Sent::GraphicalEffect pk(etBolt, target, pos2, id, 0, 0, true, false);

	NxwSocketWrapper sw;
	sw.fillOnline( target );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient j =sw.getSocket();
		if( j ) j->sendPacket(&pk);
	}
}
