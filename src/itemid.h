/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#ifndef _ITEMID_H
#define _ITEMID_H

typedef enum {
	WEAPON_INVALID=INVALID,
	SWORD1H=0,
	SWORD2H,
	AXE1H,
	AXE2H,
	MACE1H,
	MACE2H,
	FENCING1H,
	FENCING2H,
	STAVE1H,
	STAVE2H,
	BOW,
	CROSSBOW,
	HEAVYCROSSBOW,
} WEAPONTYPE;

typedef std::map<UI16,WEAPONTYPE> WEAPONINFOMAP;

void loadweaponsinfo();

bool isWeaponLike( UI16 id, WEAPONTYPE type1, WEAPONTYPE type2=WEAPON_INVALID, WEAPONTYPE type3=WEAPON_INVALID );

#endif
