/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "spawn.h"
#include "map.h"
#include "inlines.h"

cSpawns* Spawns=NULL;


cSpawnArea::cSpawnArea( AREA_ITER area )
{
	disabled = false;
	nextspawn=getclock();
	where=area;
	current=0;
	items_spawned.clear();
	npcs_spawned.clear();
}

cSpawnArea::~cSpawnArea()
{

}

bool cSpawnArea::findValidLocation( sLocation& location )
{
	int a=0;
	do {

		location.x = RandomNum( where->second.x1, where->second.x2 );
		location.y = RandomNum( where->second.y1, where->second.y2 );
		location.z = mapElevation( location.x, location.y );

		if( canNpcWalkHere( location ) )
			return true;
	} while (++a < 100);
	return false;

}

bool cSpawnArea::needSpawn()
{
	return ( TIMEOUT( this->nextspawn ) ); // && ( this->current < this->ma
}




cSpawnScripted::cSpawnScripted( uint32_t serial )
{
	this->serial=serial;
}

cSpawnScripted::~cSpawnScripted()
{
}

void cSpawnScripted::safeCreate( pChar npc, cSpawnArea& single  )
{
	if ( ! npc ) return;

	sLocation location;
	if( single.findValidLocation( location ) ) {
		npc->npcWander = WANDER_FREELY_BOX;
		npc->fx1 = single.where->second.x1;
		npc->fy1 = single.where->second.y1;
		npc->fx2 = single.where->second.x2;
		npc->fy2 = single.where->second.y2;
		npc->fz1 = INVALID;
		single.current++;
		single.nextspawn=getclock()+ (60*RandomNum( mintime, maxtime)*SECS);
		npc->spawnregion=this->serial;
		single.npcs_spawned.insert( npc->getSerial() );
		npc->MoveTo( location );
		npc->teleport();
	}
	else {
		WarnOut("[ERROR] Problem with scripted spawn %i [%s] found. Nothing will be spawned.\n", serial, name.c_str() );
		npc->Delete();
	}
}

void cSpawnScripted::safeCreate( pItem pi, cSpawnArea& single  )
{
	if ( ! pi ) return;
	
	sLocation location;
	if( single.findValidLocation( location ) ) {
		pi->MoveTo(location);
		single.current++;
		single.nextspawn=getclock()+ (60*RandomNum( mintime, maxtime)*SECS);
		pi->spawnregion=this->serial;
		single.items_spawned.insert( pi->getSerial() );
		pi->MoveTo( location );
		pi->Refresh();
	}
	else {
		WarnOut("[ERROR] Problem with scripted spawn %i [%s] found. Nothing will be spawned.\n", serial, name.c_str() );
		pi->Delete();
	}
}

void cSpawnScripted::doSpawn( cSpawnArea& c ) {
	if ( c.disabled ) {
		c.nextspawn=getclock()+ (60*RandomNum( mintime, maxtime)*SECS);
		return;
	}
	if( c.current >= max ) {
		c.nextspawn=getclock()+ (60*RandomNum( mintime, maxtime)*SECS);
		return;
	}

	if( npclists.size() > 0 )
	{
		uint32_t counter = rand()%npclists.size();
		{
			sLocation location;
			if( c.findValidLocation( location ) )
			{
				pChar npc = npcs::AddNPCxyz( INVALID, npclists[counter], location );
				if( npc )
				{
					safeCreate( npc, c );
					return;
				}
			}
		}
	}

	if( itemlists.size() > 0 )
	{

		uint32_t counter = rand()%itemlists.size();
		{
			sLocation location;
			if( c.findValidLocation( location) )
			{
				char list[512];
				sprintf( list, "%i", itemlists[counter] ); // morrolan
				int num = item::CreateRandomItem( list );
				pItem item = item::CreateScriptItem( INVALID, num, 0 );
				if( item ) {
					safeCreate( item, c );
					return;
				}
			}
		}
	}

	if( npcs.size()>0 )
	{
		uint32_t counter = rand()%npcs.size();
		{
			sLocation location;
			if( c.findValidLocation( location ) )
			{
				pChar npc = npcs::AddNPCxyz( INVALID, npcs[counter], location  );
				if ( npc )
				{
					safeCreate( npc, c );
					return;
				}
			}
		}
	}

	c.disabled = true;
	WarnOut("Scripted spawn %i [%s] couldn't find anything to spawn, check scripts.\n",serial, name.c_str());

}

void cSpawnScripted::doSpawn( )
{

	if( max==0 )
		return;

	SPAWNAREA_VECTOR::iterator iter( this->singles.begin() ), end( this->singles.end() );
	for( ; iter!=end; iter++ ) {
		if( (*iter).needSpawn() )
			this->doSpawn( *iter );
	}

}


void cSpawnScripted::doSpawnAll()
{
	if( max==0 )
		return;

	SPAWNAREA_VECTOR::iterator iter( this->singles.begin() ), end( this->singles.end() );
	for( ; iter!=end; iter++ ) {
		int loopexit=0;
		while( ( (*iter).current < max ) && ( ++loopexit<MAXLOOPS ) )
			this->doSpawn( *iter );
	}

}

void cSpawnScripted::removeObject( pItem pi )
{
	SPAWNAREA_VECTOR::iterator iter( this->singles.begin() ), end( this->singles.end() );
	for( ; iter!=end; iter++ ) {
		uint32_set::iterator itm( (*iter).items_spawned.find( pi->getSerial() ) );
		if( itm!=(*iter).items_spawned.end() ) {
			(*iter).items_spawned.erase( itm );
			if( (*iter).current>0 )
				(*iter).current--;
			return;
		}
	}
}

void cSpawnScripted::removeObject( pChar pc )
{
	SPAWNAREA_VECTOR::iterator iter( this->singles.begin() ), end( this->singles.end() );
	for( ; iter!=end; iter++ ) {
		uint32_set::iterator npc( (*iter).npcs_spawned.find( pc->getSerial() ) );
		if( npc!=(*iter).npcs_spawned.end() ) {
			(*iter).npcs_spawned.erase( npc );
			if( (*iter).current>0 )
				(*iter).current--;
			return;
		}
	}
}

cSpawns::cSpawns()
{
	this->dinamic.clear();
	this->scripted.clear();
	this->check=getclock();
}

cSpawns::~cSpawns()
{
}

void cSpawns::loadFromScript()
{

	cScpIterator*	iter = NULL;
	std::string script1;
	std::string script2;

	int idxspawn=0;

	int loopexit=0;
	do
	{
		safedelete(iter);
		uint32_t current = idxspawn;
		iter = Scripts::Spawn->getNewIterator("SECTION REGIONSPAWN %i", idxspawn++);
		if( iter==NULL ) continue;

		Area area;
		uint16_t check=0;

		cSpawnScripted* dummy=new cSpawnScripted(current);

		do
		{
			iter->parseLine(script1, script2);
			if ((script1[0]!='}')&&(script1[0]!='{'))
			{
				if("NLIST"==script1) 
					dummy->npclists.push_back(str2num(script2));
				else if("NAME"==script1) 
					dummy->name=script2;
				else if("NPC"==script1) 
					dummy->npcs.push_back(str2num(script2));
				else if("ILIST"==script1)
					dummy->itemlists.push_back(str2num(script2));
				else if("ITEM"==script1)
					dummy->itms.push_back(str2num(script2));
				else if("MAX"==script1) 
					dummy->max=str2num(script2);
				else if("X1"==script1) 
					{ area.x1=str2num(script2); check|=0x000F; }
				else if("Y1"==script1)
					{ area.y1=str2num(script2); check|=0x00F0; }
				else if("X2"==script1)
					{ area.x2=str2num(script2); check|=0x0F00; }
				else if("Y2"==script1)
					{ area.y2=str2num(script2); check|=0xF000; }
				else if("MINTIME"==script1) 
					dummy->mintime=str2num(script2);
				else if("MAXTIME"==script1) 
					dummy->maxtime=str2num(script2);
				else if("AREA"==script1) {
					AREA_ITER newarea = Areas->allareas.find( str2num(script2) );
					if( newarea!=Areas->allareas.end() )
						dummy->singles.push_back( cSpawnArea( newarea ) );
					else 
						ConOut("[ERROR] on parse of spawn.xss, can't add a new area\n" );
				}
				else ConOut("[ERROR] on parse of spawn.xss [%s]\n", script1.c_str() );
			}

		}
        while ( (script1[0]!='}') && (++loopexit < MAXLOOPS) );

		if( check==0xFFFF ) { //use old method x1, x2 so need to add a new region
			uint32_t insarea = Areas->insert( area );
			if( insarea!=INVALID ) {
				AREA_ITER newarea = Areas->allareas.find( insarea );
				if( newarea!=Areas->allareas.end() )
					dummy->singles.push_back( cSpawnArea( newarea ) );
				else 
					ConOut("[ERROR] on parse of spawn.xss, can't add a new area\n" );
			}
			else 
				ConOut("[ERROR] on parse of spawn.xss, can't add a new area\n" );
		}
		else if( check!=0 ) {
			ConOut("[ERROR] on parse of spawn.xss [%i], incomplete X1 Y1 X2 Y2\n", current );
			continue;
		}

		Spawns->scripted.insert( make_pair( current, (*dummy) ) );

    }
	while ( (strcmp("EOF", script1.c_str())) && (++loopexit < MAXLOOPS) );


    safedelete(iter);

}

/*!
\author Luxor
\brief Clears the dynamic spawners map.
*/
void cSpawns::clearDynamic()
{
	this->dinamic.clear();
}

/*!
\author Luxor
\brief Creates a new dynamic spawner getting data from an item.
*/
void cSpawns::loadFromItem( pItem pi )
{
	if ( ! pi ) return;

	//
	// If it's not a spawner, return.
	//
	if ( !pi->isSpawner() )
		return;

	//
	// Insert the spawner in the map.
	//
	Spawns->dinamic.insert( make_pair( pi->getSerial(), cSpawnDinamic(pi) ) );
}

void cSpawns::doSpawn()
{
	SPAWN_SCRIPTED_DB::iterator iter_scr( this->scripted.begin() );
	for( ; iter_scr!=this->scripted.end(); iter_scr++ ) {
		iter_scr->second.doSpawn();
	}

	SPAWN_DINAMIC_DB::iterator iter_din( this->dinamic.begin() );
	for( ; iter_din!=this->dinamic.end(); iter_din++ ) {
		if( iter_din->second.needSpawn() )
			iter_din->second.doSpawn();
	}

	if( speed.srtime!=UINVALID )
		check = getclock()+ speed.srtime*SECS; //Don't check them TOO often (Keep down the lag)
	else
		check = getclock()+ 30*SECS;
}

void cSpawns::doSpawnAll()
{
	SPAWN_SCRIPTED_DB::iterator iter( this->scripted.begin() ), end( this->scripted.end() );
	for( ; iter!=end; iter++ ) {
		iter->second.doSpawnAll();
	}

	SPAWN_DINAMIC_DB::iterator iter_din( this->dinamic.begin() );
	for( ; iter_din!=this->dinamic.end(); iter_din++ ) {
		iter_din->second.doSpawn();
	}

	if( speed.srtime!=UINVALID )
		check = getclock()+ speed.srtime*SECS; //Don't check them TOO often (Keep down the lag)
	else
		check = getclock()+ 30*SECS;
}

void cSpawns::doSpawnAll( uint32_t spawn )
{
	SPAWN_SCRIPTED_DB::iterator iter( this->scripted.find( spawn) );
	if( iter!= this->scripted.end() )
		iter->second.doSpawnAll();

}


void cSpawns::removeObject( uint32_t spawn, pItem pi )
{
	SPAWN_SCRIPTED_DB::iterator iter( this->scripted.find( spawn) );
	if( iter!= this->scripted.end() )
		iter->second.removeObject( pi );
}

void cSpawns::removeObject( uint32_t spawn, pChar pc )
{
	SPAWN_SCRIPTED_DB::iterator iter( this->scripted.find( spawn) );
	if( iter!= this->scripted.end() )
		iter->second.removeObject( pc );
}

void cSpawns::removeSpawnDinamic( pItem pi )
{
	if ( ! pi ) return;
	SPAWN_DINAMIC_DB::iterator iter( this->dinamic.find( pi->getSerial() ) );
	if( iter!=this->dinamic.end() ) {
		this->dinamic.erase( iter );
	}
	else {
		if( pi->spawnserial!=INVALID ) {
			iter= this->dinamic.find( pi->spawnserial );
			if( iter!=this->dinamic.end() ) {
				iter->second.remove( pi->getSerial() );
			}
		}
	}

}

void cSpawns::removeSpawnDinamic( pChar pc )
{
	if ( ! pc ) return;
	if( pc->spawnserial!=INVALID ) {
		SPAWN_DINAMIC_DB::iterator iter= this->dinamic.find( pc->spawnserial );
		if( iter!=this->dinamic.end() ) {
			iter->second.remove( pc->getSerial() );
		}
	}

}

cSpawnDinamic::cSpawnDinamic( pItem pi )
{
	this->item=pi->getSerial();
	this->item_spawned.clear();
	this->npc_spawned.clear();
	this->current=0;
	this->nextspawn=getclock()+ (60*RandomNum( pi->morey, pi->morez)*SECS);
}

cSpawnDinamic::~cSpawnDinamic()
{

}

void cSpawnDinamic::doSpawn()
{
	pItem spawn=cSerializable::findItemBySerial( item );
	if ( ! spawn ) return;

	if( current>=spawn->getAmount() || spawn->morex == 0)
		return;
	if( !spawn->isInWorld() )
		return; //npc spawned in container? ahhah i have a gremlin in backpack :P

	if( spawn->type == ITYPE_ITEM_SPAWNER ) {
		
		pItem pi=item::CreateFromScript( spawn->morex );
		if( pi ) {
			current++;
			item_spawned.insert( pi->getSerial() );
			pi->spawnserial=item;
			pi->MoveTo( spawn->getPosition() );
			pi->Refresh();
		}

		this->nextspawn=getclock()+ (60*RandomNum( spawn->morey, spawn->morez)*SECS);
	}
	else if( spawn->type == ITYPE_NPC_SPAWNER ) {
		pChar npc=npcs::addNpc( spawn->morex, spawn->getPosition().x, spawn->getPosition().y, spawn->getPosition().z );
		if( npc ) {
			current++;
			npc_spawned.insert( npc->getSerial() );
			npc->spawnserial=item;
			npc->MoveTo( spawn->getPosition() );
			npc->teleport();
		}

		nextspawn=getclock()+ (60*RandomNum( spawn->morey, spawn->morez)*SECS);
	}

}

void cSpawnDinamic::remove( uint32_t serial )
{
	
	if( isCharSerial( serial ) ) {
		uint32_set::iterator iter( this->npc_spawned.find( serial ) );
		if( iter!=this->npc_spawned.end() ) {
			this->npc_spawned.erase( iter );
			if( current>0 )
				current--;
		}
	}
	else {
		uint32_set::iterator iter( this->item_spawned.find( serial ) );
		if( iter!=this->item_spawned.end() ) {
			this->item_spawned.erase( iter );
			if( current>0 )
				current--;
		}
	}

		
}

bool cSpawnDinamic::needSpawn()
{
	return ( TIMEOUT( this->nextspawn ) );//&& ( this->current<this->max
}
