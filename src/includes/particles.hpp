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

#include "magic.hpp"

/*!
\brief Handling of ParticleFX
*/
class ParticleFx {
private:

public:
	int effect[18];
	void initWithSpellStatEffects_item( SpellId num);
	void initWithSpellStatEffects_post( SpellId num);
	void initWithSpellStatEffects_pre( SpellId num);
	void initWithSpellStatEffect( SpellId num );
	void initWithSpellMoveEffect( SpellId num );
};

#endif //__PARTICLES_H__
