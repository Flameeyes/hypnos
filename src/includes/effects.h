/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file 
\brief Declaration of class cSerializable
*/

#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#include "common_libs.h"
#include "inlines.h"

/*!
\brief Graphic effects functions
\author Chronodt
\todo put it in a namespace "nEffects" ?
*/

//! static effect that follows source
void staticFX(pSerializable source, uint16_t eff, uint8_t speed, uint8_t loop, class ParticleFx* part = NULL);

//! static effect on xyz (unmoving)
void locationFX(sLocation pos, uint16_t eff, uint8_t speed, uint8_t loop, bool explode);

//! moving effect from source to another cSerializable
void movingFX(pSerializable source, pSerializable destination, uint16_t eff, uint8_t speed, uint8_t duration, bool explode, class ParticleFx* part = NULL);

//! moving effect from a cSerializable to an xyz
void throwFX(pSerializable source, sLocation pos, uint16_t eff, uint8_t speed, uint8_t loop, bool explode);

//! moving effect from xyz to a pSerializable
void pullFX(sLocation src_pos, pSerializable destination, uint16_t eff, uint8_t speed, uint8_t loop, bool explode);

//! moving effect from an xyz to another xyz
void locationtolocationFX(sLocation src_pos, sLocation dst_loc, uint16_t eff, uint8_t speed, uint8_t loop, bool explode);

//! sends a lighning bolt to a cSerializable
void boltFX(pSerializable target, bool bNoParticles);

//! sends a lighning bolt to an xyz
void lighningFX(sLocation target_pos, bool bNoParticles);

//! sends a "id" around target (random displacement)
void circleFX(pSerializable target, uint16_t id);

