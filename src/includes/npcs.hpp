/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief NPC Stuff
*/

#ifndef NPCS_INCLUDE
#define NPCS_INCLUDE

#include "common_libs.hpp"
#include "structs.hpp"
#include "constants.hpp"
#include "libhypnos/hypstl/vector.hpp"

char* getRandomName(char * namelist);
void setrandomname(pChar pc, char * namelist);
uint16_t addrandomcolor(cObject* po, char *colorlist);

namespace npcs
{
	enum {
		MAGIC_SPHERE_CLASSIC = 0,
		MAGIC_SPHERE_WATER,
		MAGIC_SPHERE_FIRE,
		MAGIC_SPHERE_EARTH,
		MAGIC_SPHERE_AIR,
		MAGIC_SPHERE_MENTAL,
		MAGIC_SPHERE_CLERICAL,
		MAGIC_SPHERE_NECROMANCY,
		MAGIC_SPHERE_POISON,
		MAGIC_MAXSPHERES
	};

	void SpawnGuard( pChar pc, pChar pc_i, sLocation where);		// added by Anthalir
	void initNpcSpells();
	void npcCastSpell(pChar pc_att, pChar pc_def);
	void npcMagicAttack(pChar pc_att, pChar pc_def);

	pChar AddNPC(pClient client, pItem pi, int npcNum, uint16_t x1, uint16_t y1, int8_t z1);
	pChar AddNPC(pClient client, pItem pi, int npcNum, sLocation where);	// added by Anthalir
	pChar addNpc(int npcNum, int x, int y, int z);

	pChar AddRespawnNPC(pItem pi, int npcNum);
	pChar AddRespawnNPC(pChar pc, int npcNum);

	pChar AddNPCxyz(pClient client, int npcNum, sLocation where);		// added by anthalir
	pChar AddNPCxyz(pClient client, int npcNum, int x1, int y1, signed char z1);

	pItem AddRandomLoot(pItem pack, char * lootlist);

	int AddRandomNPC(int s, char *npclist);
	
	pChar SpawnRandomMonster(pChar pc, char* cList, char* cNpcID);


}	//namespace npcs


typedef struct {
	uint32_t script;
	uint16_t color;
	uint32_t amount;
	uint32_t id;
} vendor_item;



/*
\soundflags	
	0 - normal, 5 sounds (attack-started, idle, attack, defence, dying, see common_libs.h)
	1 - birds .. only one "bird-shape" and zillions of sounds ...
	2 - only 3 sounds -> (attack,defence,dying)
	3 - only 4 sounds ->	(attack-started,attack,defnce,dying)
	4 - only 1 sound !!
\who_am_i
	1 - creature can fly (must have the animations, so better dont change)
	2 - anti-blink: these creatures dont have animation,  if not set creaure will randomly dissapear in battle
					if you find a creature that blinks while fighting, set that bit
icon: used for tracking, to set the appropriate icon
*/

#define CREATURE_CAN_FLY 1
#define CREATURE_ANTI_BLINK 2

class cCreatureInfo {

private:

	vector<uint16_t>* sounds[ALL_MONSTER_SOUND];

public:

	uint8_t flag;
	uint32_t icon;

	cCreatureInfo();
	~cCreatureInfo();

	bool canFly() {	return flag&CREATURE_CAN_FLY; }
	bool needAntiBlink() { return flag&CREATURE_ANTI_BLINK; }
	uint16_t getSound( MonsterSound type );
	void addSound( MonsterSound type, uint16_t sound );

};

#define CREATURE_COUNT 2048

class cAllCreatures {

	pCreatureInfo allCreatures[CREATURE_COUNT];

public:

	cAllCreatures();
	~cAllCreatures();

	void load();
	pCreatureInfo getCreature( uint16_t id );

};

extern cAllCreatures creatures;

#endif
