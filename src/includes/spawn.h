/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#ifndef _SPAWN_H
#define _SPAWN_H

#include "area.h"

class cSpawnArea {

public:
	bool findValidLocation( Location& location );
	bool disabled;

public:

	cSpawnArea( AREA_ITER area );
	~cSpawnArea();

	uint32_t current; //!< currente number of spawned
	uint32_t nextspawn; //!< next spawn

	uint32_set items_spawned;
	uint32_set npcs_spawned;
	AREA_ITER where;

	bool needSpawn();


};

typedef std::vector< cSpawnArea > SPAWNAREA_VECTOR;

class cSpawnScripted {

private:

	void safeCreate( pChar npc, cSpawnArea& single );
	void safeCreate( pItem pi, cSpawnArea& single  );


public:

	cSpawnScripted( uint32_t serial );
	~cSpawnScripted();

	uint32_t serial; //!< region serial

	uint32_t mintime; //!< min time value	
	uint32_t maxtime; //!< max time value
	uint32_vector npclists; //!< npclist to spawn
	uint32_vector npcs; //!< npc to spawn
	uint32_vector itemlists; //!< itemlist to spawn
	uint32_vector itms; //!< item to spawn
	std::string name; //!< region name
	uint32_t max; //!< max number of spanwed

	SPAWNAREA_VECTOR singles; //!< spawn areas

	void doSpawn( cSpawnArea& c );
	void doSpawn( );
	void doSpawnAll();

	void removeObject( pItem pi );
	void removeObject( pChar pc );

};

class cSpawnDinamic {
public:

	cSpawnDinamic( pItem pi );
	~cSpawnDinamic();

	uint32_t item; //!< the spawner
	uint32_set item_spawned; //!< spawned items
	uint32_set npc_spawned; //!< spawned npcs

	uint32_t current;
	uint32_t nextspawn; //!< next spawn

	void doSpawn();
	bool needSpawn();
	void remove( uint32_t serial );
};

typedef std::map< uint32_t, cSpawnScripted > SPAWN_SCRIPTED_DB;
typedef std::map< uint32_t, cSpawnDinamic  > SPAWN_DINAMIC_DB;

class cSpawns {
private:

	SPAWN_SCRIPTED_DB scripted; //!< list of scripted spawn
	SPAWN_DINAMIC_DB dinamic; //!< list of dinamic spawn

public:
	cSpawns();
	~cSpawns();

	uint32_t check; //!< check respawn

	void loadFromScript();
	void loadFromItem( pItem pi );

	void clearDynamic();
	void doSpawnScripted( uint32_t spawn );
	void doSpawn();
	void doSpawnAll( );
	void doSpawnAll( uint32_t spawn );

	void removeObject( uint32_t spawn, pItem pi );
	void removeObject( uint32_t spawn, pChar pc );
	void removeSpawnDinamic( pItem pi );
	void removeSpawnDinamic( pChar pc );

};

extern cSpawns* Spawns;


#endif
