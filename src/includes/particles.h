/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief ParticleFX class
*/
#ifndef __PARTICLES_H__
#define __PARTICLES_H__

#include "magic.h"

/*!
\brief Handling of ParticleFX
*/
class ParticleFx {
private:

public:
	int effect[18];
	void initWithSpellStatEffects_item( magic::SpellId num);
	void initWithSpellStatEffects_post( magic::SpellId num);
	void initWithSpellStatEffects_pre( magic::SpellId num);
	void initWithSpellStatEffect( magic::SpellId num );
	void initWithSpellMoveEffect( magic::SpellId num );
};

#endif //__PARTICLES_H__
