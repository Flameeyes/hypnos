/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2003 - NoX-Wizard Project                                  |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __SKILLS_ALCHEMY_H__
#define __SKILLS_ALCHEMY_H__

#include "common_libs.h"
#include "skills.h"

namespace nSkills {
	//@{
	/*!
	\name Alchemy stuff
	*/
	void CreatePotion(pChar pc, uint8_t type, uint8_t sub, pItem mortar);
	void DoPotion(pClient client, uint8_t type, uint8_t sub, pItem mortar);
	void target_alchemy(pClient client, pTarget t);
	void target_bottle(pClient client, pTarget t);
	void PotionToBottle(pChar pc, pItem mortar);
	//@}
}

#endif
