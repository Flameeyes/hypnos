/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#include "common_libs.h"
#include "inlines.h"

/*!
\brief Graphical effects function
\author Chronodt

The functions in this namespace are used to show to the client the graphical
effects, both for the 2D and the 3D version.

Usually an effect has a source and a target, sometimes they are the same.
Both the source and the target can be a serializable (so a cChar or a cItem) or
a sLocation.

The effects are identified by an ID which is a 16-bit unsigned integer.
*/
namespace nEffects {

	void staticFX(pSerializable source, uint16_t eff, uint8_t speed, uint8_t loop, class ParticleFx* part = NULL);

	void locationFX(sLocation &pos, uint16_t eff, uint8_t speed, uint8_t loop, bool explode);

	void movingFX(pSerializable source, pSerializable destination, uint16_t eff, uint8_t speed, uint8_t duration, bool explode, class ParticleFx* part = NULL);

	void throwFX(pSerializable source, sLocation &pos, uint16_t eff, uint8_t speed, uint8_t loop, bool explode);

	void pullFX(sLocation &src_pos, pSerializable destination, uint16_t eff, uint8_t speed, uint8_t loop, bool explode);

	void locationtolocationFX(sLocation &src_pos, sLocation &dst_loc, uint16_t eff, uint8_t speed, uint8_t loop, bool explode);

	void boltFX(pSerializable target, bool bNoParticles);

	void lighningFX(sLocation &target_pos, bool bNoParticles);

	void circleFX(pSerializable target, uint16_t id);
};

#endif
