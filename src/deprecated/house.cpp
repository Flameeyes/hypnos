/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief House System Functions' Bodies
\todo a lot of functions need rewrite, now are commented out - Flameeyes
\todo like other parts of NoX, houses should be moved into "House" namespace, IMHO - Flameeyes
*/

#include "common_libs.h"
#include "network.h"
#include "sndpkg.h"
#include "house.h"
#include "npcai.h"
#include "data.h"
#include "boats.h"
#include "map.h"
#include "inlines.h"

bool CheckBuildSite(int x, int y, int z, int sx, int sy);


/*!
\author Zippy
\brief Build an house

Triggered by double clicking a deed-> the deed's morex is read
for the house section in house.cpp. Extra items can be added
using HOUSE ITEM, (this includes all doors!) and locked "LOCK"
Space around the house with SPACEX/Y and CHAR offset CHARX/Y/Z

\todo Remove temp variable
*/
void buildhouse( pClient client, pTarget t )
{
	int i = t->buffer[2];
	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	int loopexit=0;//where they click, and the house/key items
	uint32_t k, sx = 0, sy = 0, icount=0;
	uint16_t x, y, id_tile;
	int16_t z;
	int hitem[100];//extra "house items" (up to 100)
	char sect[512];                         //file reading
	char itemsdecay = 0;            // set to 1 to make stuff decay in houses
	static int looptimes=0;         //for targeting
	int cx=0,cy=0,cz=8;             //where the char is moved to when they place the house (Inside, on the steps.. etc...)(Offset)
	int boat=0;//Boats
	int hdeed=0;//deed id #
	int norealmulti=0,nokey=0,othername=0;
	char name[512];

	pChar pc = client->currChar();
	if ( ! pc ) return;

	sLocation charpos= pc->getPosition();

	int16_t id = INVALID; //house ID



	hitem[0]=0;//avoid problems if there are no HOUSE_ITEMs by initializing the first one as 0
	if (i)
	{
		cScpIterator* iter = NULL;
		char script1[1024];
		char script2[1024];
		sprintf(sect, "SECTION HOUSE %d", i);//and BTW, .find() adds SECTION on there for you....

		iter = Scripts::House->getNewIterator(sect);
		if (iter==NULL) return;

		do
		{
			iter->parseLine(script1, script2);
			if ((script1[0]!='}')&&(script1[0]!='{'))
			{
				if (!(strcmp(script1,"ID")))
				{
					id = hex2num(script2);
				}
				else if (!(strcmp(script1,"SPACEX")))
				{
					sx=str2num(script2)+1;
				}
				else if (!(strcmp(script1,"SPACEY")))
				{
					sy=str2num(script2)+1;
				}
				else if (!(strcmp(script1,"CHARX")))
				{
					cx=str2num(script2);
				}
				else if (!(strcmp(script1,"CHARY")))
				{
					cy=str2num(script2);
				}
				else if (!(strcmp(script1,"CHARZ")))
				{
					cz=str2num(script2);
				}
				else if( !(strcmp(script1, "ITEMSDECAY" )))
				{
					itemsdecay = str2num( script2 );
				}
				else if (!(strcmp(script1,"HOUSE_ITEM")))
				{
					hitem[icount]=str2num(script2);
					icount++;
				}
				else if (!(strcmp(script1, "HOUSE_DEED")))
				{
					hdeed=str2num(script2);
				}
				else if (!(strcmp(script1, "BOAT"))) boat=1;//Boats

				else if (!(strcmp(script1, "NOREALMULTI"))) norealmulti=1; // LB bugfix for pentas crashing client
				else if (!(strcmp(script1, "NOKEY"))) nokey=1;
				else if (!(strcmp(script1, "NAME")))
				{
					strcpy(name,script2);
					othername=1;
				}
			}
		}
		while ( (strcmp(script1,"}")) && (++loopexit < MAXLOOPS) );
		safedelete(iter);

		if (!id)
		{
			ErrOut("Bad house script # %i!\n",i);
			return;
		}
	}

	if(!looptimes)
	{
		if (i)
		{


			if (norealmulti) {
				pTarget targ = clientInfo[s]->newTarget( new cLocationTarget() );
				targ->code_callback=buildhouse;
				ShortToCharPtr(0x4064, t->buffer);
				targ->send( ps );
				ps->sysmsg( "Select a place for your structure: ");
			}
			else
				client->sysmessage("Select location for building.");
				nPackets::Sent::TargetMulti pk(0x00010000/*serial*/, id -0x4000/*model*/);
				client->sendPacket(&pk);
		}
		else
		{
			client->sysmessage("Select location for building.");
			nPackets::Sent::TargetMulti pk(0x00010000/*serial*/, ShortFromCharPtr(t->buffer) -0x4000/*model*/);
			client->sendPacket(&pk);
		}
		looptimes++;//for when we come back after they target something
		return;
	}
	if(looptimes)
	{
		looptimes=0;
		if(!pc->IsGM() && SrvParms->houseintown==0)
		{
			if ((region[pc->region].priv & rgnFlagGuarded) && itemById::IsHouse(id) ) // popy
			{
			    client->sysmessage(" You cannot build houses in town!");
			    return;
			}
		}
		x = ShortFromCharPtr(buffer[s] +11); //where they targeted	
		y = ShortFromCharPtr(buffer[s] +13);
		z = ShortFromCharPtr(buffer[s] +15);
		id_tile = ShortFromCharPtr(buffer[s] +17);
		z += tileHeight(id_tile);

		//XAN : House placing fix :)
		if ( (( x<XBORDER || y <YBORDER ) || ( x>(uint32_t)((map_width*8)-XBORDER) || y >(uint32_t)((map_height*8)-YBORDER) ))  )
		{
			client->sysmessage("You cannot build your structure there!");
			return;
		}


		/*
		if (ishouse(id1, id2)) // strict checking only for houses ! LB
		{
			if(!(CheckBuildSite(x,y,z,sx,sy)))
			{
				client->sysmessage("Can not build a house at that location (CBS)!");
				return;
			}
		}*/


		for (k=0;k<sx;k++)//check the SPACEX and SPACEY to make sure they are valid locations....
		{
			for (uint32_t l=0;l<sy;l++)
			{

				sLocation loc;

				loc.x=x+k;

				loc.y=y+l;

				loc.z=z;
				sLocation newpos = sLocation( x+k, y+l, z );
				if ( (isWalkable( newpos ) == illegal_z ) &&
					((charpos.x != x+k)&&(charpos.y != y+l)) )
					/*This will take the char making the house out of the space check, be careful
					you don't build a house on top of your self..... this had to be done So you
					could extra space around houses, (12+) and they would still be buildable.*/
				{
					client->sysmessage("You cannot build your stucture there.");
					return;
					//ConOut("Invalid %i,%i [%i,%i]\n",k,l,x+k,y+l);
				} //else ConOut("DEBUG: Valid at %i,%i [%i,%i]\n",k,l,x+k,y+l);

				if ( !norealmulti && cMulti::getAt(loc) )
				{
					client->sysmessage("You cant build structures inside structures");
					return;
				}
			}
		}

		if((id % 256)>=18)
			sprintf(temp,"%s's house",pc->getCurrentName().c_str());//This will make the little deed item you see when you have showhs on say the person's name, thought it might be helpful for GMs.
		else
			strcpy(temp, "a mast");
		if(norealmulti)
			strcpy(temp, name);
		//--^

		if (othername)
			strcpy(temp,name);

		if (id == INVALID)
			return;

		pItem pHouse = item::CreateFromScript( "$item_hardcoded" );
		if ( !pHouse ) return;
		pHouse->setId( id );
		pHouse->setCurrentName( temp );

		pc->making=0;

		pHouse->setPosition(x, y, z);
		pHouse->setDecay( false );
		pHouse->setNewbie( false );
		pHouse->setDispellable( false );
		pHouse->more4 = itemsdecay; // set to 1 to make items in houses decay
		pHouse->morex=hdeed; // crackerjack 8/9/99 - for converting back *into* deeds
		pHouse->setOwner(pc);
		if (pHouse->isInWorld())
		{
			mapRegions->add(pHouse);
		}
		if (!hitem[0] && !boat)
		{
			pc->teleport();
			return;//If there's no extra items, we don't really need a key, or anything else do we? ;-)
		}

		if(boat)
		{
			if(!Build(s,pHouse, id%256/*id2*/))
			{
				pHouse->Delete();
				return;
			}
		}

		if (i)
		{
			pItem pFx1 = MAKE_ITEM_REF( pc->fx1 );
			if ( pFx1 != 0 )
				pFx1->Delete(); // this will del the deed no matter where it is
		}

		pc->fx1=-1; //reset fx1 so it does not interfere
		// bugfix LB ... was too early reseted

		pItem pKey=NULL;
		pItem pKey2=NULL;

		pItem pBackPack = pc->getBackpack();

		//Key...
		//Altered key naming to include pc's name. Integrated backpack and bankbox handling (Sparhawk)
		if ((id%256 >=0x70) && (id%256 <=0x73))
		{
			sprintf(temp,"%s's tent key",pc->getCurrentName().c_str());
			pKey = item::CreateFromScript( "$item_iron_key", pBackPack ); //iron key for tents
			pKey2= item::CreateFromScript( "$item_iron_key", pBackPack );
		}
		else if(id%256 <=0x18)
		{
			sprintf(temp,"%s's ship key",pc->getCurrentName().c_str());
			pKey= item::CreateFromScript( "$item_bronze_key", pBackPack ); //Boats -Rusty Iron Key
			pKey2= item::CreateFromScript( "$item_bronze_key", pBackPack );
		}
		else
		{
			sprintf(temp,"%s's house key",pc->getCurrentName().c_str());
			pKey= item::CreateFromScript( "$item_gold_key", pBackPack ); //gold key for everything else;
			pKey2= item::CreateFromScript( "$item_gold_key", pBackPack );
		}

		if ( ! pKey || ! pKey2 ) return;

		pKey->Refresh();
		pKey2->Refresh();

		pHouse->st = pKey->getSerial();		// Create link from house to housekeys to allow easy renaming of
		pHouse->st2= pKey2->getSerial();	// house, housesign and housekeys without having to loop trough
														// all world items (Sparhawk)


		pKey->more = pHouse->getSerial();	//use the house's serial for the more on the key to keep it unique
		pKey->type=ITYPE_KEY;
		pKey->setNewbie();

		pKey2->more = pHouse->getSerial();	//use the house's serial for the more on the key to keep it unique
		pKey2->type=ITYPE_KEY;
		pKey2->setNewbie();

		pItem bankbox = pc->GetBankBox();
		if(bankbox!=NULL) // we sould add a key in bankbox only if the player has a bankbox =)
		{
			pItem p_key3=item::CreateFromScript( "$item_gold_key" );
			if ( ! p_key3 ) return;
			p_key3->setCurrentName( "a house key" );
			p_key3->more = pHouse->getSerial();
			p_key3->type=ITYPE_KEY;
			p_key3->setNewbie();
			bankbox->AddItem(p_key3);
		}
		if(nokey)
		{
			pKey->Delete(); // No key for .. nokey items
			pKey2->Delete(); // No key for .. nokey items
		}

		for (k=0;k<icount;k++)//Loop through the HOUSE_ITEMs
		{
			cScpIterator* iter = NULL;
			char script1[1024];
			char script2[1024];
			sprintf(sect,"SECTION HOUSE ITEM %i",hitem[k]);
			iter = Scripts::House->getNewIterator(sect);

			if (iter!=NULL)
			{
				pItem pi_l=NULL;
				loopexit=0;
				do
				{
					iter->parseLine(script1, script2);
					if (script1[0]!='}')
					{
						if (!(strcmp(script1,"ITEM")))
						{
							pi_l=item::CreateScriptItem(s,str2num(script2),0);//This opens the item script... so we gotta keep track of where we are with the other script.

							if(pi_l)
							{


							pi_l->magic=2;//Non-Movebale by default
							pi_l->setDecay( false ); //since even things in houses decay, no-decay by default
							pi_l->setNewbie( false );
							pi_l->setDispellable( false );
							pi_l->setPosition(x, y, z);
							pi_l->setOwner(pc);
							// SPARHAWK 2001-01-28 Added House sign naming
							if (pi_l->IsSign())
								if ((id%256 >=0x70) && (id%256<=0x73))
									pi_l->setCurrentName("%s's tent",pc->getCurrentName().c_str());
								else if (id%256<=0x18)
									pi_l->setCurrentName("%s's ship",pc->getCurrentName().c_str());
								else
									pi_l->setCurrentName("%s's house",pc->getCurrentName().c_str());

							}
						}
						if (!(strcmp(script1,"DECAY")))
						{
							if (pi_l) pi_l->setDecay();
						}
						if (!(strcmp(script1,"NODECAY")))
						{
							if (pi_l) pi_l->setDecay( false );
						}
						if (!(strcmp(script1,"PACK")))//put the item in the Builder's Backpack
						{
							if (pi_l) pi_l->setContainer(pc->getBackpack());
							if (pi_l) pi_l->setPosition("x", rand()%90+31);
							if (pi_l) pi_l->setPosition("y", rand()%90+31);
							if (pi_l) pi_l->setPosition("z", 9);
						}
						if (!(strcmp(script1,"MOVEABLE")))
						{
							if (pi_l) pi_l->magic=1;
						}
						if (!(strcmp(script1,"LOCK")))//lock it with the house key
						{
							if (pi_l) pi_l->more = pHouse->getSerial();
						}
						if (!(strcmp(script1,"X")))//offset + or - from the center of the house:
						{
							if (pi_l) pi_l->setPosition("x", x+str2num(script2));
						}
						if (!(strcmp(script1,"Y")))
						{
							if (pi_l) pi_l->setPosition("y", y+str2num(script2));
						}
						if (!(strcmp(script1,"Z")))
						{
							if (pi_l) pi_l->setPosition("z", z+str2num(script2));
						}
					}
				}
				while ( (strcmp(script1,"}")) && (++loopexit < MAXLOOPS) );

				if (pi_l)
					if (pi_l->isInWorld())
					{
						mapRegions->add(pi_l);
					}
				safedelete(iter);
			}
		}

        NxwSocketWrapper sw;
		sw.fillOnline( pc, false );
        for( sw.rewind(); !sw.isEmpty(); sw++ ) {
			pClient ps_i = sw.getClient();
			if(ps_i==NULL)
				continue;
			pChar pc_i=ps_i->currChar();
			if(pc_i)
				pc_i->teleport();
		}
                //</Luxor>
		if (!(norealmulti))
		{
			charpos.x= x+cx; //move char inside house
			charpos.y= y+cy;
			charpos.dispz= charpos.z= z+cz;

			pc->setPosition( charpos );
			//ConOut("Z: %i Offset: %i Char: %i Total: %i\n",z,cz,chars[currchar[s]].z,z+cz);
			pc->teleport();
		}
	}
}

bool CheckBuildSite(int x, int y, int z, int sx, int sy)
{
	signed int checkz;
	//char statc;
	int checkx;
	int checky;
	int ycount=0;
	checkx=x-(sx/2);
	for (;checkx<(x+(sx/2));checkx++)
	{
		checky=y-(sy/2);
		for (;checky<(y+(sy/2));checky++)
		{
			checkz=mapElevation(checkx,checky);
			if ((checkz>(z-2))&&(checkz<(z+2)))
			{
				ycount++;
			}
			//	statc=Map->StaHeight(checkx,checky,checkz);
			//	if (statc>0)
			//		statb=true;
		}
	}
	if (ycount==(sx*sy)) //&& (statb==false))
		return true;
	else
		return false;
}

// buffer 0 the sign
void target_houseOwner( pClient ps, pTarget t )
{
	pChar curr=ps->currChar();
	if ( ! curr ) return;

	pChar pc = dynamic_cast<pChar>( t->getClicked() );
	if ( ! pc ) return;

	pItem pSign=cSerializable::findItemBySerial( t->buffer );
	if ( ! pSign ) return;

	pHouse ph=cSerializable::findItemBySerial( pSign->more );
	if ( ! ph ) return;

	if(pc == curr)
	{
		client->sysmessage("you already own this house!");
		return;
	}

	pSign->setOwner(pc);

	ph->setOwner(pc);
	ph->killKeys()


	pClient osc=pc->getClient();

	pItem pi3=item::CreateFromScript( "$item_gold_key" ); //gold key for everything else
	if ( ! pi3 ) return;
	
	pi3->setCurrentName( "a house key" );
	if(osc)
		pi3->setContainer( pc->getBackpack() );
	else
		pi3->MoveTo( pc->getPosition() );
	pi3->Refresh();
	pi3->more = ph->getSerial();
	pi3->type=7;

	client->sysmessage("You have transferred your house to %s.", pc->getCurrentName().c_str());
	char *temp;
	asprintf(&temp, "%s has transferred a house to %s.", curr->getCurrentName().c_str(), pc->getCurrentName().c_str());

	NxwSocketWrapper sw;
	sw.fillOnline( pc, false );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient k = sw.getClient();
		if ( k )
			k->sysmessage(temp);
	}
	
	free(temp);
}

// buffer[0] house
void target_houseEject( pClient client, pTarget t )
{
	pChar pc = dynamic_cast<pChar>( t->getClicked() );
	if ( ! pc ) return;

	pHouse ph = dynamic_cast<pHouse>( cSerializable::findBySerial(t->buffer) );
	if ( ! ph ) return;

	if ( ! pi_h->getArea().isInside(pc->getPosition()) )
	{
		client->sysmessage("That is not inside the house.");
		return;
	}
	
	pc->MoveTo( ex+1, ey+1, pcpos.z );
	pc->teleport();
	client->sysmessage("Player ejected.");
}

//buffer[0] house
void target_houseBan( pClient client, pTarget t )
{
	pChar pc = dynamic_cast<pChar>( t->getClicked() );
	if ( ! pc ) return;

	pChar curr = client->currChar();
	if ( ! curr ) return;

	if(pc == curr)
		return;
	
	pHouse ph = dynamic_cast<pHouse>(cSerializable::findBySerial( t->buffer[0] ));
	if ( ! ph ) return;
	
	if ( ! ph->getArea().isInside(pc->getBody()->getPosition()) )
	{
		client->sysmessage("That player is not on the property.");
		return;
	}
	
	// Eject the player only when the target is acknowledged
	target_houseEject(client, t);

	if ( ! ph->setStatus(pc, cHouse::psBanned) )
		client->sysmessage("That player can't be banned.");
	else
		client->sysmessage("%s has been banned from this house.", pc->getCurrentName().c_str());
}

// buffer[0] the house
void target_houseFriend( pClient client, pTarget t )
{
	pChar pc = dynamic_cast<pChar>( t->getClicked() );
	if ( ! pc ) return;

	pChar curr = client->currChar();
	if ( ! curr ) return;

	if(pc == curr)
		return;
	
	pHouse ph = dynamic_cast<pHouse>(cSerializable::findBySerial( t->buffer[0] ));
	if ( ! ph ) return;
	
	if ( ! ph->getArea().isInside(pc->getBody()->getPosition()) )
	{
		client->sysmessage("That player is not on the property.");
		return;
	}
	
	if ( ! ph->setStatus(pc, cHouse::psFriend) )
		client->sysmessage("That player can't be made friend.");
	else
		client->sysmessage("%s has been made a friend of the house.", pc->getCurrentName().c_str());
}

// bugffer[0] the hose
void target_houseUnlist( pClient client, pTarget t )
{
	pChar pc = dynamic_cast<pChar>( t->getClicked() );
	if ( ! pc ) return;

	pChar curr = client->currChar();
	if ( ! curr ) return;

	if(pc == curr)
		return;
	
	pHouse ph = dynamic_cast<pHouse>(cSerializable::findBySerial( t->buffer[0] ));
	if ( ! ph ) return;
	
	if ( ! ph->getArea().isInside(pc->getBody()->getPosition()) )
	{
		client->sysmessage("That player is not on the property.");
		return;
	}
	
	if ( ! ph->setStatus(pc, cHouse::psFriend) )
		client->sysmessage("That player can't be removed from the house registry.");
	else
		client->sysmessage("%s has been removed from the house registry.", pc->getCurrentName().c_str());
}

void target_houseLockdown( pClient client, pTarget t )
// PRE:     S is the socket of a valid owner/coowner and is in a valid house
// POST:    either locks down the item, or puts a message to the owner saying he's a moron
// CODER:   Abaddon
// DATE:    17th December, 1999
{
	pChar pc = client->currChar();
	if ( ! pc ) return;

	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	
	if ( ! pi )
	{
		client->sysmessage("Invalid item!");
		return;
	}
	
	// time to lock it down!

	if( pi->isFieldSpellItem() )
	{
		client->sysmessage("you cannot lock this down!");
		return;
	}
	//!\todo Change these types with mnemonics
	if (pi->type==12 || pi->type==13 || pi->type==203)
	{
		client->sysmessage("You cant lockdown doors or signs!");
		return;
	}
	if ( pi->isAnvil() )
	{
		client->sysmessage("You cant lockdown anvils!");
		return;
	}
	if ( pi->isForge() )
	{
		client->sysmessage("You cant lockdown forges!");
		return;
	}

	//!\todo Take a look if this check do all the stuff needed
	if( ! cMulti::getAt(pi->getPosition()) )
	{
		// not in a multi!
		client->sysmessage("That item is not in your house!" );
		return;
	}
	
	{
		
	if(pi->magic==4)
	{
		client->sysmessage("That item is already locked down, release it first!");
		return;
	}
	pi->magic = 4;  // LOCKED DOWN!
	clientInfo[s]->dragging=false;
	pi->setOwner(pc);
	pi->Refresh();
	return;
}

void target_houseSecureDown( pClient client, pTarget t )
// For locked down and secure chests
{
	pChar pc = clean->currChar();
	if ( ! pc ) return;

	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if ( ! pi )
	{
		client->sysmessage("Invalid item!" );
		return;
	}
        
	// time to lock it down!

        if( pi->isFieldSpellItem() )
        {
		client->sysmessage("you cannot lock this down!");
		return;
        }
        if (pi->type==12 || pi->type==13 || pi->type==203)
        {
		client->sysmessage("You cant lockdown doors or signs!");
		return;
        }
        if(pi->magic==4)
        {
		client->sysmessage("That item is already locked down, release it first!");
		return;
        }

        pMulti multi = cMulti::getAt( pi->getPosition() );
        if( multi && pi->type==1)
        {
		pi->magic = 4;  // LOCKED DOWN!
		pi->secureIt = 1;
		clientInfo[s]->dragging=false;
		pi->setOwner(pc);
		pi->Refresh();
		return;
        }
        if(pi->type!=1)
        {
		client->sysmessage("You can only secure chests!");
		return;
        }
        else
        {
		// not in a multi!
		client->sysmessage("That item is not in your house!" );
		return;
        }
}

void target_houseRelease( pClient client, pTarget t )
// PRE:     S is the socket of a valid owner/coowner and is in a valid house, the item is locked down
// POST:    either releases the item from lockdown, or puts a message to the owner saying he's a moron
// CODER:   Abaddon
// DATE:    17th December, 1999
// update: 5-8-00
{
	pChar pc = client->currChar();
	if ( ! pc ) return;

    
	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if(pi)
	{
		if(pi->getOwner() != pc)
		{
		    client->sysmessage("This is not your item!");
		    return;
		}

		if( pi->isFieldSpellItem() )
		{
		    client->sysmessage("you cannot release this!");
		    return;
		}

		if (pi->type==12 || pi->type==13 || pi->type==203)
		{
		    client->sysmessage("You cant release doors or signs!");
		    return;
		}

		// time to lock it down!
		pMulti multi = cMulti::getAt( pi->getPosition() );
		if( multi && pi->magic==4 || pi->type==1)
		{
		    pi->magic = 1;  // Default as stored by the client, perhaps we should keep a backup?
		    pi->secureIt = 0;
		    pi->Refresh();
		    return;
		}

		if( !multi ) {
			client->sysmessage("That item is not in your house!" );
			return;
		}
	} else {
        
		client->sysmessage("Invalid item!" );
		return;
	}
}
