  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "common_libs.h"
#include "network.h"
#include "debug.h"
#include "cmdtable.h"
#include "sregions.h"
#include "sndpkg.h"
#include "commands.h"
#include "data.h"
#include "spawn.h"
#include "set.h"
#include "inlines.h"

namespace Commands
{
	int32_t cmd_offset;

	void MakeShop(int c)
	{
		pChar pc = MAKE_CHAR_REF( c );
		if ( ! pc ) return;

		pc->shopkeeper = true;

		pItem pi;
		if (pc->GetItemOnLayer(LAYER_TRADE_RESTOCK) == NULL)
		{
			pi = item::CreateFromScript( "$item_restock", pc );
			if( pi )//AntiChrist - to preview crashes
			{
				pi->setContainer(pc);
				pi->layer=LAYER_TRADE_RESTOCK;
				pi->type=ITYPE_CONTAINER;
				pi->setNewbie();
			}
		}

		if (pc->GetItemOnLayer(LAYER_TRADE_NORESTOCK) == NULL)
		{
			pi = item::CreateFromScript( "$item_restock", pc );
			if( pi )
			{
				pi->setContainer(pc);
				pi->layer=LAYER_TRADE_NORESTOCK;
				pi->type=ITYPE_CONTAINER;
				pi->setNewbie();
			}
		}

		if (pc->GetItemOnLayer(LAYER_TRADE_BOUGHT) == NULL)
		{
			pi = item::CreateFromScript( "$item_restock", pc );
			if( pi )
			{
				pi->setContainer(pc);
				pi->layer=LAYER_TRADE_BOUGHT;
				pi->type=ITYPE_CONTAINER;
				pi->setNewbie();
			}
		}
	}

	void KillSpawn(NXWSOCKET s, int r)  //courtesy of Revana
	{
		/*unsigned int i;
		int killed=0;

		r++; // synch with 1-indexed real storage, casue 0 is no region indicator, LB

		if (r<=0 || r>=255) return;

		sysmessage(s,"Killing spawn, this may cause lag...");

		for(i=0;i<charcount;i++)
		{
			pChar pc_i=MAKE_CHAR_REF(i);
			if(pc_i) {

				if(pc_i->spawnregion==r && !pc_i->free)
				{
					pc_i->boltFX()
					pc_i->playSFX( 0x0029);
					pc_i->deleteChar();
					killed++;
				}
			}
		}


		for(i=0;i<itemcount;i++)
		{
			pItem pi =MAKE_ITEM_REF(i);
			if(pi)
			{
				if(pi->spawnregion==r && !pi->free)
				{
					pi->deleteItem();
					killed++;
				}
			}
		}

		gcollect();
		sysmessage(s, "Done.");
		sysmessage(s, "%i of Spawn %i have been killed.",killed,r-1);*/
	}

	void RegSpawnMax (NXWSOCKET s, int r ) // rewrite LB
	{
		pChar pc=MAKE_CHAR_REF(currchar[s]);
		if ( ! pc ) return;

	//	unsigned int currenttime=uiCurrentTime;

		sysbroadcast("Spawn %d is spawning all items/NPCs, this will cause some lag.", r);

		Spawns->doSpawnAll( r );

		pc->sysmsg("[DONE] All NPCs/items spawned in spawn %d.", r );
	}

	void RegSpawnNum (NXWSOCKET s, int r, int n) // rewrite by LB
	{
	/*	int i, spawn=0;
		unsigned int currenttime=uiCurrentTime;
		char *temps;

		r++;
		if (r<=0 || r>=255) return;

		temps = new char[100];

			spawn = (spawnregion[r].max-spawnregion[r].current);
			if (n > spawn)
			{
				sprintf(temps, "%d too many for region %d, spawning %d to reach MAX:%d instead.",n, r-1, spawn, spawnregion[r].max);
				sysmessage(s, temps);
				n=spawn;
			}
			sprintf(temps, "Region %d is Spawning %d NPCs/items, this will cause some lag.", r-1, spawn);
			sysbroadcast(temps);

			for(i=1;i<spawn;i++)
			{
				doregionspawn(r);
			}//for

			spawnregion[r].nexttime=currenttime+(MY_CLOCKS_PER_SEC*60*RandomNum(spawnregion[r].mintime,spawnregion[r].maxtime));

			sprintf(temps, "[DONE] %d total NPCs/items spawned in Spawnregion %d.",spawn,r-1);
			sysmessage(s, temps);
		if (temps!=NULL) delete [] temps; // fixing memory leak, LB
	*/
	}//regspawnnum

	void KillAll(NXWSOCKET s, int percent, char* sysmsg)
	{
	return;
	/*	int i;
		sysmessage(s,"Killing all characters, this may cause some lag...");
		sysbroadcast(sysmsg);
		for(i=0;i<charcount;i++)
		{
			pChar pj=MAKE_CHAR_REF(i);
			if(!pj)
				continue;

			if(!pj->IsGM())
			{
				if(rand()%100+1<=percent)
				{
					pj->boltFX();
					pj->playSFX(0x0029);
					pj->Kill();
				}
			}
		}
		sysmessage(s, "Done.");*/
	}

	//o---------------------------------------------------------------------------o
	//|   Function -  void cpage(NXWSOCKET s,char *reason)
	//|   Date     -  UnKnown
	//|   Programmer  -  UnKnown
	//o---------------------------------------------------------------------------o
	//|   Purpose     -
	//o---------------------------------------------------------------------------o
	void CPage(NXWSOCKET s, std::string reason) // Help button (Calls Counselor Call Menus up)
	{
		int i;
		uint32_t a;
		int x2=0;

		pChar pc_currchar = MAKE_CHAR_REF(currchar[s]);
		if ( !pc_currchar ) return;

		a = pc_currchar->getSerial();

		for(i=1;i<MAXPAGES;i++)
		{
			if(counspages[i].handled==1)
			{
				counspages[i].handled=0;
				strcpy(counspages[i].name,pc_currchar->getCurrentName().c_str());
				strcpy(counspages[i].reason,reason.c_str());
				counspages[i].serial.serial32 = a;
				time_t current_time = time(0);
				struct tm *local = localtime(&current_time);
				sprintf(counspages[i].timeofcall, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);
				pc_currchar->playercallnum=i;
				pc_currchar->pagegm=2;
				x2++;
				break;
			}
		}
		if(x2==0)
		{
			pc_currchar->sysmsg("The Counselor Queue is currently full. Contact the shard operator");
			pc_currchar->sysmsg("and ask them to increase the size of the queue.");
		}
		else
		{
			if(reason == "OTHER")
			{
				char *temp;
				pc_currchar->pagegm=0;
				asprintf(&temp, "Counselor Page from %s [ %08x ]: %s", pc_currchar->getCurrentName().c_str(), a, reason.c_str());
				bool found=false;

				NxwSocketWrapper sw;
				sw.fillOnline();
				for( sw.rewind(); !sw.isEmpty(); sw++ ) {

					NXWCLIENT ps_i=sw.getClient();
					if(ps_i==NULL)
						continue;
					pChar pj=ps_i->currChar();
					if ( pj && pj->IsCounselor() )
					{
						found=true;
						ps_i->sysmsg( temp);
					}

				}
				free(temp);
				
				if (found)
					pc_currchar->sysmsg("Available Counselors have been notified of your request.");
				else
					pc_currchar->sysmsg("There was no Counselor available to take your call.");
			}
			else pc_currchar->sysmsg("Please enter the reason for your Counselor request");
		}
	}

	void DyeItem(NXWSOCKET s) // Rehue an item
	{
		uint16_t color, body;

		pChar Me = MAKE_CHAR_REF(currchar[s]);
		if ( ! Me ) return;

		pItem pi = cSerializable::findItemBySerial(LongCharFromPtr(buffer[s] +1));
		if( pi )
		{

			if( !Me->IsGMorCounselor() ) {

				if( !pi->isDyeable() )
					return;
				
				pItem outmost = pi->getOutMostCont();
				
				//!\todo should be rewrote using dynamic_cast
				
				uint32_t cont = outmost->getContSerial();
				if( isCharSerial( cont ) ) {
					if( cont!=Me->getSerial() )
						return;
				}
				else if( isItemSerial( cont ) ) {
					pItem backpack = Me->getBackpack();
					if( backpack && ( cont!=backpack->getSerial() ) )
						return;
				}
				else { //on ground
					if( pi->magic==4 )
						return;
				}
			}

			color = ShortFromCharPtr(buffer[s] +7);


			if (( color<0x0002) || (color>0x03E9))
			{
				color = 0x03E9;
			}


			if (! ((color & 0x4000) || (color & 0x8000)) )
			{
				pi->setColor( color );
			}

			if (color == 0x4631)
			{
				pi->setColor( color );
			}

			pi->Refresh();

			Me->playSFX(0x023E);
			return;
		}

		pChar pc = cSerializable::findCharBySerial(LongCharFromPtr(buffer[s] +1));
		if( pc && Me->IsGMorCounselor() )
		{
			color = ShortFromCharPtr(buffer[s] +7);


			body = pc->getId();

			if(  color < 0x8000  && body >= BODY_MALE && body <= BODY_DEADFEMALE ) color |= 0x8000; // why 0x8000 ?! ^^;

			if ((color & 0x4000) && (body >= BODY_MALE && body<= 0x03E1)) color = 0xF000; // but assigning the only "transparent" value that works, namly semi-trasnparency.

			if (color != 0x8000)
			{
				pc->setColor(color);
				pc->setOldColor(color);
				pc->teleport( TELEFLAG_NONE );

				Me->playSFX(0x023E);
			}
		}
	}


	void AddHere(NXWSOCKET s, uint16_t id, char z)
	{
		if ( s < 0 || s >= now )
			return;
		bool pileable=false;
		tile_st tile;

		data::seekTile( id, tile);
		if (tile.flags&TILEFLAG_STACKABLE) pileable=true;

		pItem pi = item::CreateFromScript( "$item_hardcoded" );
		if ( !pi ) return;
		pi->setId( id );
		pi->pileable = pileable;

		if(pi)//AntiChrist - to preview crashes
		{
			pChar pc_currchar = MAKE_CHAR_REF(currchar[s]);
			Location charpos= pc_currchar->getPosition();

			pi->MoveTo( charpos.x, charpos.y, z );
			pi->doordir=0;
			pi->setDecay( false );
			pi->setNewbie( false );
			pi->setDispellable( false );
			pi->Refresh();//AntiChrist
		}

	}

	void DupeItem(pClient client, pItem pi_from, uint16_t amount)
	{
		pChar pc = client->currChar();
		if ( ! pc ) return;
		pItem pack = pc->getBackpack();
		if ( ! pack || ! pi_from )
			return;

		if ( !pi_from->corpse )
		{
			pItem pi_to = new cItem(cItem::nextSerial());
			(*pi_to)=(*pi_from);

			pack->AddItem(pi_to);

			pi_to->setAmount(amount);

			pi_to->Refresh();
		}
	}

	// new wipe function, basically it prints output on the console when someone wipes so that
	// if a malicious GM wipes the world you know who to blame

	/*!
	\brief wipes every wipeable object
	\author Luxor
	*/
	void Wipe(NXWSOCKET s)
	{
		if (s < 0 || s >= now)
			return;

		pChar pc = cSerializable::findCharBySerial(currchar[s]);
		if ( ! pc ) return;
		InfoOut( "%s has initiated an item wipe\n", pc->getCurrentName().c_str() );
		cAllObjectsIter objs;
		pItem pi = NULL;
		for( objs.rewind(); !objs.IsEmpty(); objs++ ) {
			pi = cSerializable::findItemBySerial( objs.getSerial() );
			if ( pi && pi->isInWorld() && pi->wipe == 0 )
				pi->Delete();
		}

		sysbroadcast("All items have been wiped");
	}


	//New Luxor's possess command :) Enjoy!
	/*!
	\brief possess command for GMs
	\author Luxor
	\param s the socket of the GM
	*/
	void Possess(NXWSOCKET s)
	{
		pChar pc = MAKE_CHAR_REF(currchar[s]);
		if ( ! pc ) return;
		pChar pcPos = cSerializable::findCharBySerial(LongCharFromPtr(buffer[s]+7));
		if ( ! pcPos ) return;

		pc->possess(pcPos);

	}
};
