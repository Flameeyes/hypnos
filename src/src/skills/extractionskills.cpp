/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "skills/extractionskills.h"
#include "data.h"
#include "skills/skills.h"
#include "inlines.h"


cResources ores;


cResources::cResources( uint32_t areawidth, uint32_t areaheight )
{
	resources.clear();
	setDimArea( areawidth, areaheight );
	n=10;
	time=600;
	rate=3;
	stamina=-5;
}

cResources::~cResources()
{
}

void cResources::setDimArea( uint32_t areawidth, uint32_t areaheight )
{
	area_width=areawidth;
	area_height=areaheight;
}

uint64_t cResources::getBlocks( sLocation location  )
{
	return (uint64_t)( (uint64_t)(location.x / this->area_width) <<32 ) + ( location.y / this->area_height );
}


pResource cResources::getResource( sLocation location )
{

	uint64_t p = this->getBlocks( location );
	
	RESOURCE_MAP::iterator iter( this->resources.find( p ) );
	if( iter==this->resources.end() )
		return NULL;
	else 
		return &(iter->second); 
}

pResource cResources::createBlock( sLocation location )
{
	uint64_t p = this->getBlocks( location );
	
	this->resources.insert( make_pair( p, cResource( ) ) );
	RESOURCE_MAP::iterator iter( this->resources.find( p ) );
	return &iter->second;
}

void cResources::checkResource( sLocation location, pResource& res )
{

	if( res==NULL )
		return;

	if( this->checkRes( res ) ) { //delete
		res=NULL;
		this->deleteBlock( location );
	}

}

void cResources::decreaseResource( sLocation location, pResource res )
{
	if( res == NULL ) { //create it!
		res = this->createBlock( location );
	}
	res->consumed++;
}


bool cResources::thereAreSomething( pResource res )
{
	return ( res==NULL ) || ( res->consumed<=this->n );
}


bool cResources::checkRes( pResource res )
{

	if( TIMEOUT( res->timer ) )
	{
		if( res->consumed>ores.n )
			res->consumed=ores.n;
		for( uint32_t c=0; c<ores.n; c++ ) //Find howmany periods have been by, give 1 more ore for each period.
		{
			if(( TIMEOUT( res->timer+(c* ores.time*SECS)) ) && res->consumed>=ores.rate )
				res->consumed-=ores.rate;//AntiChrist
		}
		res->timer= getClockmSecs() + ores.time*SECS;
		if( res->consumed == 0)
			return true; // delete itself because FULL
	}

	return false;
}


void cResources::checkAll()
{

	static uint32_t timer=getClockmSecs()+2*60*SECS;

	if(TIMEOUT( timer ) ) {

		RESOURCE_MAP::iterator next( this->resources.begin() );
		for( ; next!=resources.end(); ) {
			RESOURCE_MAP::iterator iter( next ); next++;
			if( this->checkRes( &iter->second ) ) {
				this->resources.erase( iter );
			}
		}
		timer=getClockmSecs()+2*60*SECS;
	}
}

void cResources::deleteBlock( sLocation location ) 
{
	uint64_t p = this->getBlocks( location );
	
	RESOURCE_MAP::iterator iter( this->resources.find( p ) );
	if( iter!=this->resources.end() )
		this->resources.erase( iter );
}

static bool canMine( pClient client, pItem weapon )
{
	pChar pc = client->currChar();
	if ( ! pc ) return false;
	
	if( !weapon )
		client->sysmessage("You can't mine with nothing in your hand !!");
	else
		switch( weapon->getId() )
		{
			case 0x0E85	:
			case 0x0E86	:
			case 0x0F39	:
			case 0x0F3A	:
				if (pc->getBody()->isMounted())
					client->sysmessage( "You cant mine while on a horse!");
				else
					if( !pc->IsGM() && (ores.stamina<0) && (abs( ores.stamina )>pc->stm) )
						client->sysmessage( "You are too tired to mine.");
					else
						return true;
				break;
			default :
				client->sysmessage( "You must have a pickaxe or shovel in hand in order to mine.");
		}

	return false;
}


void Skills::target_mine( pClient client, pTarget t )
{
	pChar pc = client->currChar();
	if ( ! pc ) return;

	pItem weapon = pc->GetItemOnLayer(1);
	
	if( !canMine( client, weapon ) )
		return;

	sLocation target = t->getPosition();

	pc->facexy( target.x, target.y );

	AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skMining,AMX_BEFORE);

	if ( pc->hidden )
		pc->unHide();

	bool floor = false;
	bool mountain= false;

	map_st map;

	pc->stm+=ores.stamina;
	if(pc->stm<0) 
		pc->stm=0;
	if(pc->stm>pc->dx) 
		pc->stm=pc->dx;
	pc->updateStamina();

	
	int cx = abs( (int) (pc->getPosition().x - target.x) );
	int cy = abs( (int) (pc->getPosition().y - target.y) );

	if( (cx>5) || (cy>5) )
	{
		client->sysmessage("You are to far away to reach that");
		return;
	}

	uint32_t id = t->getModel();

	if( SrvParms->minecheck > 0 && !id )
	{
		// mountains are "map0's" and no statics !!!
		data::seekMap( target.x, target.y, map );

		string name = tiledataLand->getName(map.id);
		if ( !strcmp(name.c_str(),"rock") || !(strcmp(name.c_str(), "mountain")) || !(strcmp(name.c_str(), "cave")))
			mountain = true;
	}

	data::seekMap( target.x, target.y, map );
	if( !id )
		id= map.id;

	switch( id )
	{
		case 0x0ED3:
		case 0x0EDF:
		case 0x0EE0:
		case 0x0EE1:
		case 0x0EE2:
		case 0x0EE8:
			Skills::GraveDig(client);
			return;
		default:
			break;
	}

	//
	// Caves (Walls & Floors)
	//
	if( (id >= 0x025C && id <= 0x0276) ||
		(id >= 0x027D && id <= 0x0280) ||
		(id >= 0x053B && id <= 0x0553) ||
		(id == 0x056A))
		floor = true;

	// sand (Anthalir)
	if( (id>=0x0017) && (id<=0x0019) )
		floor = true;


	// check if cave floor ENDYMION USE THIS BUT SEE VALUES IN OLD CODE
	/*if ( 
		( (targetData.getModel( s )>>8)==0x05 && ( ((targetData.getModel( s )%256)>=0x3b && (targetData.getModel( s )%256)<=0x4f ) || ((targetData.getModel( s )%256)>=0x51 && (targetData.getModel( s )%256)<=0x53) || (targetData.getModel( s )%256)==0x6a ))&&
		(!( ((targetData.getModel( s )>>8)==0x02)&&
        ( ( ((targetData.getModel( s )%256)>=0x5c) && ((targetData.getModel( s )%256)<=0x76))||(((targetData.getModel( s )%256)>=0x7d)&&((targetData.getModel( s )%256)<=0x80))))))
		
		 floor=1;*/
	

	if ((SrvParms->minecheck!=0)&&(!floor)&&(!mountain))//Mine only mountains & floors
	{
		client->sysmessage("You can't mine that!");
		return;
	}

	pResource res = ores.getResource( target );

	ores.checkResource( target, res );

	if( !ores.thereAreSomething( res ) )
	{
		client->sysmessage("There is no metal here to mine.");
		return;
	}

	if (pc->getBody()->isMounted())
		pc->playAction(0x1A);
	else
		pc->playAction(0x0B);

	pc->playSFX(0x0125);

	if(!pc->checkSkill(skMining, 0, 1000))
	{
		clien->sysmessage("You sifted thru the dirt and rocks, but found nothing useable.");
		if( rand()%2==1)
			return; //Randomly deplete resources even when they fail 1/2 chance you'll loose ore.
	}
	ores.decreaseResource( target, res );

	AmxFunction::g_prgOverride->CallFn( AmxFunction::g_prgOverride->getFnOrdinal(AMXskMining), s);
	AMXEXECSVTARGET(pc->getSerial(),AMXT_SKITARGS,skMining,AMX_AFTER);
}

