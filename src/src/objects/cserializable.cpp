/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "objects/cserializable.h"
#include "objects/cchar.h"
#include "objects/citem.h"
#include "logsystem.h"

SerializableMap cSerializable::objects;

/*!
\brief Finds an object by its serial
\param serial Serial of the object
*/
pSerializable cSerializable::findBySerial(uint32_t serial)
{
	/*! \note This function checks if serial has the MSB set, and if so
	 *  returns null without check into the cSerializable::objects map.
	 */
	if ( serial & 0x80000000 ) return NULL;
	SerializableMap::iterator it = objects.find(serial);
	
	return (it == objects.end()) ? NULL : (*it).second;
}

pChar cSerializable::findCharBySerial(uint32_t serial)
{
	/*! \note The function calls isCharSerial() function to check if the
	 *  given serial is valid, this is a quicker check than a search
	 *  into the map and a dynamic_cast to NULL if item (or invalid) serial
	 *  :)
	 */
	if ( ! isCharSerial(serial) ) return NULL;
	return dynamic_cast<pChar>(findBySerial(serial));
}

pItem cSerializable::findItemBySerial(uint32_t serial)
{
	/*! \note The function calls isItemSerial() function to check if the
	 *  given serial is valid, this is a quicker check than a search
	 *  into the map and a dynamic_cast to NULL if char (or invalid) serial
	 *  :)
	 */
	if ( ! isItemSerial(serial) ) return NULL;
	return dynamic_cast<pItem>(findBySerial(serial));
}

/*!
\brief Constructor with given serial
\param givenSerial serial to use for the object

This constructor should be used by all the serializable classes, using passing
it the getNewSerial() function when creating a new serial.

\note This function can't generate a new serial for duplicated serials, so
	call it with a surely new serial
*/
cSerializable::cSerializable(uint32_t givenSerial)
{
	serial = givenSerial;
	if ( objects.find(givenSerial) != objects.end() )
	{
		LogWarning("Tried to create a new object with an existing serial number %08x.", givenSerial);
		delete this;
		return;
	}
	
	objects.insert( std::make_pair( serial, this ) );
	
	// We must set them here because we can't use directly
	// the setMulti() and setOwner() functions of resetData()
	// else we can try to dereference a dirt pointer.
	multi = NULL;
	owner = NULL;
}

/*!
\brief Destructor

The destructor take care of remove the object from the objects list.
*/
cSerializable::~cSerializable()
{
	SerializableMap::iterator it = objects.find(getSerial());
	if ( it != objects.end() )
		objects.erase(it);
}

/*!
\brief Resets the data inside the instance of the class

This function is similar to cObject::resetData() and is called by the two
cItem::resetData() and cChar::resetData() to reset the data derived from the
serialization of the object iself.
*/
void cSerializable::resetData()
{
	setMulti(NULL);
	setOwner(NULL);
}

/*!
\brief Plays a moving effect from this to target char
\param destination the target
\param id id of the effect
\param speed speed of the effect
\param duration loops
\param explode true if should do a final explosion
\param part particle effects structure
*/
void cSerializable::movingFX(pSerializable destination, uint16_t eff, uint8_t speed, uint8_t duration, bool explode, ParticleFx* part)
{
	if ( ! destination ) return;

	nPackets::Sent::GraphicalEffect pk(etBolt, this, destination, eff, speed, duration, 0, explode);

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
\param bNoParticles true if NOT to use particles
*/
void cChar::boltFX(bool bNoParticles)
{
	uint8_t effect[28]={ 0x70, 0x00, };

 	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var

	sLocation pos2;
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
\param id effect id
*/
void cChar::circleFX(short id)
{
	uint8_t effect[28]={ 0x70, 0x00, };

	int x,y;
	sLocation charpos = getPosition(), pos2;

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
\brief Sets the object inside the given multi
\param nMulti New multi to set (if NULL, remove from multi)

This function calls the cMulti::remove() function to remove itself if already
in a multi and then cMulti::add() function to add itself if nMulti is not NULL.
*/
void cSerializable::setMulti(pMulti nMulti)
{
	if ( multi )
		multi->remove(this);
	
	if ( nMulti )
		nMulti->add(this);
	
	multi = nMulti;
}
