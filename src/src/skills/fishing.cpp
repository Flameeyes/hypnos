/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Store all fishing related functions
\author Ripper
\date 31/01/2001
*/
#include "common_libs.h"
#include "sndpkg.h"
#include "debug.h"
#include "data.h"
#include "skills/fishing.h"
#include "basics.h"
#include "inlines.h"

TIMERVAL Fishing::basetime = FISHINGTIMEBASE;
TIMERVAL Fishing::randomtime = FISHINGTIMER;

pNPC SpawnFishingMonster(pChar pc, char* cScript, char* cList, char* cNpcID)
{
	/*This function gets the random monster number from
	the script and list specified.
	npcs::AddRespawnNPC passing the new number*/

	if (region[pc->region].priv&0x01 && SrvParms->guardsactive) //guarded
		return NULL;

	char sect[512];
	int i=0,item[256]={0};

    cScpIterator* iter = NULL;
    char script1[1024];
	
	sprintf(sect, "SECTION %s %s", cList, cNpcID);
    iter = Scripts::Fishing->getNewIterator(sect);
    if (iter==NULL) return NULL

	int loopexit=0;
 	do
	{
  		strcpy(script1, iter->getEntry()->getFullLine().c_str());
		if((script1[0]!='}')&&(script1[0]!='{'))
		{
			item[i]=str2num(script1);
			i++;
		}
	}
 	while(script1[0]!='}' && (++loopexit < MAXLOOPS) );
 	safedelete(iter);

 	if(i>0)
 	{
  		i=rand()%(i);
		if(item[i]!=-1)
		{
			return npcs::AddRespawnNPC(pc,item[i]);
		}
	}
	return NULL;
}

int SpawnFishingItem(pClient client,int nInPack, char* cScript, char* cList, char* cItemID)
{
 	/*This function gets the random item number from the list and recalls
 	  SpawnItemBackpack2 passing the new number*/
	
	pChar pc=cSerializable::findCharBySerial(currchar[s]);
	if ( ! pc ) return INVALID;
	
	char sect[512];
	int i=0,item[256]={0};
    
	cScpIterator* iter = NULL;
    char script1[1024];

	sprintf(sect, "SECTION %s %s", cList, cItemID);
    iter = Scripts::Fishing->getNewIterator(sect);
    if (iter==NULL) return -1;
	int loopexit=0;

 	do
 	{
  		strcpy(script1, iter->getEntry()->getFullLine().c_str());
  		if ((script1[0]!='}')&&(script1[0]!='{'))
  		{
			item[i]=str2num(script1);
   			i++;
		}
	}
	while( (script1[0]!='}') && (++loopexit < MAXLOOPS) );
	safedelete(iter);
 	if(i>0)
 	{
  		i=rand()%(i);
		if((item[i]!=INVALID) && (nInPack) ) {
			item::CreateFromScript( item[i], pc->getBackpack());
			return item[i];
		}
	}
	return INVALID;
}

inline bool isWaterTarget(pClient client)
{
	tile_st tile;
	map_st map;
	land_st land;

	if(buffer[s][0x11]==0x17 && buffer[s][0x12]==0x98 || buffer[s][0x11]==0x17 && buffer[s][0x12]==0x9B || buffer[s][0x11]==0x17 && buffer[s][0x12]==0x9C || buffer[s][0x11]==0x17 && buffer[s][0x12]==0x99 || buffer[s][0x11]==0x17 && buffer[s][0x12]==0x97 || buffer[s][0x11]==0x17 && buffer[s][0x12]==0x9A)
	{
		return true;
	} 
	if(buffer[s][0x1]!=0x01) return false;

	int x = ShortFromCharPtr(buffer[s] +0xB);
	int y = ShortFromCharPtr(buffer[s] +0xD);

	data::seekMap(x, y,map);
	switch(map.id)
	{
		//water tiles:
		case 0x00A8:
		case 0x00A9:
		case 0x00AA:
		case 0x00Ab:
		case 0x0136:
		case 0x0137:
		case 0x3FF0:
		case 0x3FF1:
		case 0x3FF2:
		case 0x2FF3:
			return true;
		default:
			break;
	}

	data::seekTile( LongFromCharPtr(buffer[s] +0x11), tile);
	if( !(strstr((char *) tile.name, "water") || strstr((char *) tile.name, "lava")) )
	{
		data::seekLand(map.id, land);
		if (!(land.flags&TILEFLAG_WET))//not a "wet" tile
		{
			return false;
		}
	}
	
	return true;
}


void Fishing::target_fish( pClient client, pTarget t )
{
	pChar pPlayer = client->currChar();
	if ( ! pPlayer ) return;

	Location charpos= pPlayer->getPosition();
	Location whereFish = t->getLocation();

	int px = whereFish.x;
	int py = whereFish.y;

	if( dist( charpos, whereFish )>6.0 )	// too far away from target
	{
		client->sysmessage("You are too far away to reach that");
		return;
	}

	if( !isWaterTarget(client) )	// target is not a water tile
	{
		client->sysmessage("You can only fish in water !!");
		return;
	}
	
	pPlayer->facexy(px, py);
	pPlayer->playAction(0x0B);
	if (randomtime!=0)
		pPlayer->fishingtimer=rand()%randomtime+basetime;
	else
		pPlayer->fishingtimer=basetime;
	pPlayer->playSFX(0x023F);
	pPlayer->unHide();
	pPlayer->fish();
//	Fish(client);
}


// LB: added fish stacking !!
void Fishing::Fish(pClient client)
{
//	const int max_fish_piles = 1;		// attention: thats per fish *type*, so the efffective limit of piles is *3
//	const int max_fish_stacksize = 15;	// attention: rela max size = value+1
	pChar pc = client->currChar();
	if (!pc) return;

	pItem pc_bp = pc->getBackpack();

	int ii;
	int idnum;
	int16_t color;
        
	Location charpos= pc->getPosition();
	if(pc->stm<=2) //Luxor bug fix
	{
		pc->stm=0;
		client->sysmessage("You are too tired to fish, you need to rest!");
		return;
	}

	pc->stm-=2; // lose 2 stamina each cast.
		
	if(!pc->checkSkill( FISHING, 0, 1000))
	{
		client->sysmessage("You fish for a while, but fail to catch anything.");
		return;
	}


	// New Random fishing up treasures and monsters...Ripper
	unsigned short skill=pc->skill[FISHING];
	int fishup=(RandomNum(0,100));
	switch (fishup)
		{
		case 0:
            if(skill>=200) 
			{ 
				SpawnFishingItem( s, 1, "fishing.scp", "FISHLIST", "5" ); // random boots
				client->sysmessage("You fished up an old pair of boots!");
			} 
            break;
		case 1:
            if(skill>=970) 
			{ 
				SpawnFishingItem( s, 1, "fishing.scp", "FISHLIST", "1" ); // random paintings 
				client->sysmessage("You fished up an ancient painting!"); 
			} 
            break;
		case 2:
            if(skill>=950) 
			{ 
				SpawnFishingItem( s, 1, "fishing.scp", "FISHLIST", "2" ); // random weapons 
				client->sysmessage("You fished up an ancient weapon!");
			} 
            break;
		case 3:
            if(skill>=950) 
			{ 
				SpawnFishingItem( s, 1, "fishing.scp", "FISHLIST", "3" ); // random armor 
				client->sysmessage("You fished up an ancient armor!");
			} 
            break;
		case 4:
            if(skill>=700) 
			{ 
				SpawnFishingItem( s, 1, "fishing.scp", "FISHLIST", "4" ); // random treasure
				client->sysmessage("You fished up some treasure!");
			} 
            break;
		case 5:
            if(skill>=400) 
			{ 
				if (SpawnFishingMonster( pc, "fishing.scp", "MONSTERLIST", "7" ) != -1) // random monsters 
					client->sysmessage("You fished up a hughe fish!");
				else
					client->sysmessage("You wait for a while, but nothing happens");
			} 
            break;
		case 6:
            if(skill>=800) 
			{
				SpawnFishingItem( s, 1, "fishing.scp", "FISHLIST", "6" ); // random chests
				client->sysmessage("You fished up an old chest!");
			} 
            break;
		case 7:
            if(skill>=700) 
			{
				SpawnFishingItem( s, 1, "fishing.scp", "FISHLIST", "8" ); // random seashells
				client->sysmessage("You fished up a seashell!");
			} 
            break;
		case 8:
            if(skill>=700) 
			{
				SpawnFishingItem( s, 1, "fishing.scp", "FISHLIST", "9" ); // random skulls
				client->sysmessage("You fished up a skull!");
			} 
            break;
		case 9:
            if(skill>=900) 
			{
				SpawnFishingItem( s, 1, "fishing.scp", "FISHLIST", "10" ); // random nets
				client->sysmessage("You fished up a net!");
			} 
            break;
		case 10:
            if(skill>=900) 
			{
				SpawnFishingItem( s, 1, "fishing.scp", "FISHLIST", "11" ); // random gold
				client->sysmessage("You fished up some gold!");
			} 
            break;
		case 11:
            if(skill>=400) 
			{
				SpawnFishingItem( s, 1, "fishing.scp", "FISHLIST", "12" ); // random bones
				client->sysmessage("You fished up some bones!");
			} 
            break;
		default: {

			ii=rand()%3;
			idnum=0xCC+ii;
	
			///**** exotic fish stuff *****//
			double mv=-0.087087087*(float)pc->skill[FISHING]+100.087087087086; // gm fish -> 1/13 % probability on new spawn(!) to have exotic color, neophyte: 1/92 % probability, linear interpolation in between
			int no_idea_for_variable_name = (int) mv;
			if (no_idea_for_variable_name<=0) no_idea_for_variable_name=1; // prevent modulo crashes	
			if (rand()%no_idea_for_variable_name==0) 
			{ 		
				color=(charpos.x + charpos.y);
				color+= rand()%10;
				color= color%0x03E9; 
				if((color <0x0002) || (color > 0x03E9)) color = 0x03E9;

				if( color & 0xC000 )
					color = (0x0010 | (rand()%255));
			} else color=0;
		
			/**** end of exotic fish stuff stuff */
		
			//fishes_around_player=item::Find_items_around_player(i, 0x09, idnum, 2, 2, max_fish_piles, fish_sers); // lets search for fish in a 2*2 rectangle around the player
			
			//pItem pFish;
			//if (fishes_around_player<=0) // no fish around -> spawn a new one
		
				//Luxor - Now fishes are spawned into backpack
			//{
				//Luxor: fishes should be read from items.xss
				//pFish=item::SpawnItem(i,1,"#",1,0x0900+idnum,(c1<<8)+c2,0);
			pItem fish = item::CreateFromScript( "$item_fish" );
			if ( ! fish ) return;
		
			fish->setColor(color);
			fish->setId( fish->getId() | idnum );
		
			if (pc_bp)
				pc_bp->AddItem( fish );
			else {
				fish->MoveTo( charpos ); //Luxor bug fix
				fish->Refresh();
			}
		}
	}

	if(color)
		client->sysmessage("You pull out an exotic fish!");
	else
		client->sysmessage("You pull out a fish!");
}
