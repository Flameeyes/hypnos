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
\todo a lot of functions need rewrite, now are commented out - Akron
\todo like other parts of NoX, houses should be moved into "House" namespace, IMHO - Akron
*/

#include "common_libs.h"
#include "network.h"
#include "sndpkg.h"
#include "debug.h"
#include "itemid.h"
#include "set.h"
#include "house.h"
#include "npcai.h"
#include "data.h"
#include "boats.h"
#include "map.h"
#include "inlines.h"

std::map< uint32_t, pChar > houses;

bool CheckBuildSite(int x, int y, int z, int sx, int sy);

/*!
\todo take a look to initialization, we could initialize the vector with the variables
*/
void mtarget(int s, int a1, int a2, int a3, int a4, char b1, char b2, char *txt)
{
	uint8_t multitarcrs[26]= { 0x99, 0x01, 0x40, 0x01, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	multitarcrs[2]=a1;
	multitarcrs[3]=a2;
	multitarcrs[4]=a3;
	multitarcrs[5]=a4;
	multitarcrs[18]=b1;
	multitarcrs[19]=b2;
	sysmessage(s, txt);
	Xsend(s, multitarcrs, 26);
//AoS/	Network->FlushBuffer(s);
}

/*!
\author Zippy
\brief Build an house

Triggered by double clicking a deed-> the deed's morex is read
for the house section in house.cpp. Extra items can be added
using HOUSE ITEM, (this includes all doors!) and locked "LOCK"
Space around the house with SPACEX/Y and CHAR offset CHARX/Y/Z

\todo Remove temp variable
*/
void buildhouse( NXWCLIENT ps, pTarget t )
{
	NXWSOCKET s = ps->toInt();
	int i = t->buffer[2];
	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	int loopexit=0;//where they click, and the house/key items
	uint32_t x, y, k, sx = 0, sy = 0, icount=0;
	signed char z;
	int hitem[100];//extra "house items" (up to 100)
	char sect[512];                         //file reading
	char itemsdecay = 0;            // set to 1 to make stuff decay in houses
	static int looptimes=0;         //for targeting
	int cx=0,cy=0,cz=8;             //where the char is moved to when they place the house (Inside, on the steps.. etc...)(Offset)
	int boat=0;//Boats
	int hdeed=0;//deed id #
	int norealmulti=0,nokey=0,othername=0;
	char name[512];
	pChar pc=ps->currChar();
	if ( ! pc ) return;
	Location charpos= pc->getPosition();

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
				targ->buffer[0]=0x40;
				targ->buffer[1]=100;
				//targ->buffer[2]; never setted
				targ->send( ps );
				ps->sysmsg( "Select a place for your structure: ");
			}
			else
				mtarget(s, 0, 1, 0, 0, (id>>8) -0x40, (id%256), "Select location for building.");

		}
		else
		{
			mtarget(s, 0, 1, 0, 0, t->buffer[0]-0x40, t->buffer[1], "Select location for building.");
		}
		looptimes++;//for when we come back after they target something
		return;
	}
	if(looptimes)
	{
		looptimes=0;
		if(!pc->IsGM() && SrvParms->houseintown==0)
		{
			if ((region[pc->region].priv & RGNPRIV_GUARDED) && itemById::IsHouse(id) ) // popy
			{
			    sysmessage(s," You cannot build houses in town!");
			    return;
			}
		}

		x = ShortFromCharPtr(buffer[s] +11); //where they targeted
		y = ShortFromCharPtr(buffer[s] +13);
		z = buffer[s][16] + tileHeight(ShortFromCharPtr(buffer[s] +17));

#define XBORDER 200
#define YBORDER 200

		//XAN : House placing fix :)
		if ( (( x<XBORDER || y <YBORDER ) || ( x>(uint32_t)((map_width*8)-XBORDER) || y >(uint32_t)((map_height*8)-YBORDER) ))  )
		{
			sysmessage(s, "You cannot build your structure there!");
			return;
		}


		/*
		if (ishouse(id1, id2)) // strict checking only for houses ! LB
		{
			if(!(CheckBuildSite(x,y,z,sx,sy)))
			{
				sysmessage(s,"Can not build a house at that location (CBS)!");
				return;
			}
		}*/


		for (k=0;k<sx;k++)//check the SPACEX and SPACEY to make sure they are valid locations....
		{
			for (uint32_t l=0;l<sy;l++)
			{

				Location loc;

				loc.x=x+k;

				loc.y=y+l;

				loc.z=z;
				Location newpos = Location( x+k, y+l, z );
				if ( (isWalkable( newpos ) == illegal_z ) &&
					((charpos.x != x+k)&&(charpos.y != y+l)) )
					/*This will take the char making the house out of the space check, be careful
					you don't build a house on top of your self..... this had to be done So you
					could extra space around houses, (12+) and they would still be buildable.*/
				{
					sysmessage(s, "You cannot build your stucture there.");
					return;
					//ConOut("Invalid %i,%i [%i,%i]\n",k,l,x+k,y+l);
				} //else ConOut("DEBUG: Valid at %i,%i [%i,%i]\n",k,l,x+k,y+l);

				pItem pi_ii=findmulti(loc);
				if ( pi_ii && !(norealmulti))
				{
					sysmessage(s,"You cant build structures inside structures");
					return;
				}
			}
		}

		//Boats ->
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
		pHouse->setOwnerSerial32(pc->getSerial());
		if (pHouse->isInWorld())
		{
			mapRegions->add(pHouse);
		}
		if (!hitem[0] && !boat)
		{
			pc->teleport();
			return;//If there's no extra items, we don't really need a key, or anything else do we? ;-)
		}

		if(boat) //Boats
		{
			if(!Boats->Build(s,pHouse, id%256/*id2*/))
			{
				pHouse->Delete();
				return;
			}
		}

		if (i)//Boats->.. Moved from up there ^
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


		pKey->more1=pHouse->getSerial().ser1;//use the house's serial for the more on the key to keep it unique
		pKey->more2=pHouse->getSerial().ser2;
		pKey->more3=pHouse->getSerial().ser3;
		pKey->more4=pHouse->getSerial().ser4;
		pKey->type=ITYPE_KEY;
		pKey->setNewbie();
		pKey2->more1=pHouse->getSerial().ser1;//use the house's serial for the more on the key to keep it unique
		pKey2->more2=pHouse->getSerial().ser2;
		pKey2->more3=pHouse->getSerial().ser3;
		pKey2->more4=pHouse->getSerial().ser4;
		pKey2->type=ITYPE_KEY;
		pKey2->setNewbie();

		pItem bankbox = pc->GetBankBox();
		if(bankbox!=NULL) // we sould add a key in bankbox only if the player has a bankbox =)
		{

			pItem p_key3=item::CreateFromScript( "$item_gold_key" );
			if ( ! p_key3 ) return;
			p_key3->setCurrentName( "a house key" );
			p_key3->more1=pHouse->getSerial().ser1;
			p_key3->more2=pHouse->getSerial().ser2;
			p_key3->more3=pHouse->getSerial().ser3;
			p_key3->more4=pHouse->getSerial().ser4;
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
							pi_l->setOwnerSerial32(pc->getSerial());
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
							if (pi_l) {
								pi_l->more1=pHouse->getSerial().ser1;
								pi_l->more2=pHouse->getSerial().ser2;
								pi_l->more3=pHouse->getSerial().ser3;
								pi_l->more4=pHouse->getSerial().ser4;
							}
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
			NXWCLIENT ps_i = sw.getClient();
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

/*!
\brief Turn a house into a deed if posible
\author CrackerJack
\param s socket of player
\param pi pointer to the house item
*/
void deedhouse(NXWSOCKET s, pItem pi)
{
	uint32_t x1, y1, x2, y2;
	if ( ! pi ) return;
	pChar pc = cSerializable::findCharBySerial(currchar[s]);
	if ( ! pc ) return;
	Location charpos= pc->getPosition();


	if(pi->getOwnerSerial32() == pc->getSerial() || pc->IsGM()) // bugfix LB, was =
	{
		getMultiCorners(pi, x1,y1,x2,y2);

		pItem pi_ii=item::CreateFromScript( pi->morex, pc->getBackpack() ); // need to make before delete
		if ( ! pi_ii ) return;

		sysmessage( s, "Demolishing House %s", pi->getCurrentName().c_str());
		pi->Delete();
		sysmessage(s, "Converted into a %s.", pi_ii->getCurrentName().c_str());
		// door/sign delete

		NxwCharWrapper sc;
		sc.fillCharsNearXYZ( charpos, BUILDRANGE, true, false );
		for( sc.rewind(); !sc.isEmpty(); sc++ ) {
			pChar p_index=sc.getChar();
			if( p_index ) {

				Location charpos2= p_index->getPosition();
				if( (charpos2.x >= (uint32_t)x1) && (charpos2.y >= (uint32_t)y1) && (charpos2.x <= (uint32_t)x2) && (charpos2.y <= (uint32_t)y2) )
				{

					if( p_index->npcaitype == NPCAI_PLAYERVENDOR )
					{
						char *temp;
						asprintf( &temp, "A vendor deed for %s", p_index->getCurrentName().c_str() );
						pItem pDeed = item::CreateFromScript( "$item_employment_deed", pc->getBackpack() );
						if ( ! pDeed ) return;
						free(temp);

						pDeed->Refresh();
						p_index->Delete();
						sysmessage(s, "Packed up vendor %s.", p_index->getCurrentName().c_str());
					}
				}
			}
		}

		NxwItemWrapper si;
		si.fillItemsNearXYZ( charpos, BUILDRANGE, false );
		for( si.rewind(); !si.isEmpty(); si++ ) {
		{
			pItem p_item=si.getItem();
			if( p_item ) {
				if( (p_item->getPosition().x >= (uint32_t)x1) &&
					(p_item->getPosition().x <= (uint32_t)x2) &&
					(p_item->getPosition().y >= (uint32_t)y1) &&
					(p_item->getPosition().y <= (uint32_t)y2) )
					{
							p_item->Delete();
					}
				}
			}
		}

		killkeys( pi->getSerial() );
		sysmessage(s,"All house items and keys removed.");
		/*
		charpos.z= charpos.dispz= Map->MapElevation(charpos.x, charpos.y);
		pc->setPosition( charpos );
		*/
		pc->setPosition("z", mapElevation(charpos.x, charpos.y));
		pc->setPosition("dz", mapElevation(charpos.x, charpos.y));
		pc->teleport();
		return;
	}
}

// removes houses - without regions. slow but necassairy for house decay
// LB 19-September 2000
/*!
\brief Remove Houses, without regions, slow but necessariry for house decay
\todo Need rewrite, now commented out
*/
void killhouse(ITEM i)
{
/*	pChar pc;
	pItem pi;
	int x1, y1, x2, y2;


	pi = MAKE_ITEM_REF(i);

	Map->MultiArea(pi, &x1, &y1, &x2, &y2);
	uint32_t serial = pi->getSerial();

	int a;
	for (a = 0; a < charcount; a++) // deleting npc-vendors attched to the decying house
	{
		pc = cSerializable::findCharBySerial(a); /// !!!!!!!!!!!!!!
		Location charpos= pc->getPosition();

		if ((charpos.x >= x1) && (charpos.y >= y1) && (charpos.x <= x2) && (charpos.y <= y2) && !pc->free)
		{
			if (pc->npcaitype == NPCAI_PLAYERVENDOR) // player vendor in right place, delete !
			{
				pc->deleteChar();
			}
		}
	}

	for (a = 0; a < itemcount; a++) // deleting itmes inside house
	{
		pi = MAKE_ITEM_REF(a);
		if ((pi->getPosition().x >= x1) &&
			(pi->getPosition().y >= y1) &&
			(pi->getPosition().x <= x2) &&
			(pi->getPosition().y <= y2) &&
			(!pi->free))
		{
			if (pi->type != ITYPE_GUILDSTONE) // dont delete guild stones !
			{
				pi->deleteItem();
			}
		}
	}

	// deleting house keys
	killkeys(serial);
	*/
}

/*!
\brief does all the work for house decay

checks all items if they are houses. if so, check its time stamp. if its too old remove it
\todo need rewrite, now is commented out...
*/
int check_house_decay()
{
/*	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	pItem pi;
	bool is_house;
	int houses=0;
	int decayed_houses=0;
	unsigned long int timediff;
	unsigned long int ct=getclock();

	//CWatch *Watch=new CWatch();

	for (int i=0; i<itemcount; i++)
	{
		pi=MAKE_ITEM_REF(i); // there shouldnt be an error here !
		is_house=pi->IsHouse();
		if (is_house && !pi->free)
		{
			// its a house -> check its unused time
			//ConOut("id2: %x time_unused: %i max: %i\n",pi->id2,pi->time_unused,server_data.housedecay_secs);

			if (pi->time_unused>SrvParms->housedecay_secs) // not used longer than max_unused time ? delete the house
			{
				decayed_houses++;
				sprintf(temp,"%s decayed! not refreshed for > %i seconds!\n",pi->getCurrentName().c_str(),SrvParms->housedecay_secs);
				LogMessage(temp);
				killhouse(i);
			}
			else // house ok -> update unused-time-attribute
			{
				timediff=(ct-pi->timeused_last)/MY_CLOCKS_PER_SEC;
				pi->time_unused+=timediff; // might be over limit now, but it will be cought next check anyway
				pi->timeused_last=ct;	// if we don't do that and housedecay is checked every 11 minutes,
									// it would add 11,22,33,... minutes. So now timeused_last should in fact
									// be called timeCHECKED_last. but as there is a new timer system coming up
									// that will make things like this much easier, I'm too lazy now to rename
									// it (Duke, 16.2.2001)
			}
			houses++;
		}
	}
	//delete Watch;
	return decayed_houses;
*/
	return 0;
}


/*!
\author Luxor
\note This function use a BAD method based on cAllObjects, will be substituted with a map system.
*/
void killkeys(uint32_t serial) // Crackerjack 8/11/99
{
	if ( serial <= INVALID )
		return;

	cAllObjectsIter objs;
	pItem pi = NULL;
	for( objs.rewind(); !objs.IsEmpty(); objs++ ) {
		if ( !isItemSerial( objs.getSerial() ) )
			continue;

		if ( (pi=static_cast<pItem>(objs.getObject())) ) {
			if ( pi->type == ITYPE_KEY && calcserial(pi->more1, pi->more2, pi->more3, pi->more4) == serial )
				pi->Delete();
		}
	}
}

/*!
\brief Checks if somebody is on the house list
\param pi pointer to the house item
\param s1 serial (byte1)
\param s2 serial (byte2)
\param s3 serial (byte3)
\param s4 serial (byte4)
\param li pointer to variable to put items[] index of list item in or NULL
\return 0 if character is not on house list, else the type of list
\todo change the 4 chars to a single serial32
*/
int on_hlist(pItem pi, uint32_t serial, int *li)
{
	if ( ! pi || ! pi->isInWorld() ) return 0;

	pItem p_ci=NULL;

	NxwItemWrapper si;
	si.fillItemsNearXYZ( pi->getPosition(), BUILDRANGE, false );
	for( si.rewind(); !si.isEmpty(); si++ ) {

		p_ci=si.getItem();
		if( p_ci ) {

			if((p_ci->morey== (uint32_t)pi->getSerial())&&
			   (calcserial( p_ci->more1i, p_ci->more2, p_ci->more3, p_ci->more4) == serial) )
				{
				    if(li!=NULL) *li=DEREF_pItem(p_ci);
						return p_ci->morex;
				}
		}
	}

	return 0;
}

/*!
\brief Adds somebody to a house list
\param pc someone character
\param pi_h someonelse's house
\param t list tyle
\return 1 if successful addition, 2 if the char was alredy on a house list, 3 if the character is not on property
*/
int add_hlist(pChar pc, pItem pi_h, int t)
{
	uint32_t sx, sy, ex, ey;

	if ( ! pc || ! pi_h ) return 3;

	if(on_hlist(pi_h, pc->getSerial(), NULL))
		return 2;

	getMultiCorners(pi_h, sx,sy,ex,ey);
	// Make an object with the character's serial & the list type
	// and put it "inside" the house item.
	Location charpos= pc->getPosition();

	if((charpos.x >= (uint32_t)sx) && (charpos.y >= (uint32_t)sy) && (charpos.x <= (uint32_t)ex) && (charpos.y <= (uint32_t)ey))
	{
		pItem pi = new cItem(cItem::nextSerial());

		pi->morex= t;
		pi->more1= pc->getSerial().ser1;
		pi->more2= pc->getSerial().ser2;
		pi->more3= pc->getSerial().ser3;
		pi->more4= pc->getSerial().ser4;
		pi->morey= pi_h->getSerial();

		pi->setDecay( false );
		pi->setNewbie( false );
		pi->setDispellable( false );
		pi->visible= 0;
		pi->setCurrentName("friend of house");

		pi->setPosition( pi_h->getPosition() );
#ifdef SPAR_I_LOCATION_MAP
		pointers::addToLocationMap(pi);
#else
		mapRegions->add(pi);
#endif
		return 1;
	}
	return 3;
}

/*!
\brief Remove somebody from a house list
\param pc someone
\param pi house
\return 0 if the player was not on a list, else the list which the player was in.
*/
int del_hlist(pChar pc, pItem pi)
{
	int hl, li;

	if ( ! pc || ! pi ) return 0;

	hl=on_hlist(pi, pc->getSerial(), &li);
	if(hl) {
		pItem pli=MAKE_ITEM_REF(li);
		if( pli ) {
#ifdef SPAR_I_LOCATION_MAP
			//
			// Hmmm....this is handled by pointers::delItem()....must remove it later
			//
			pointers::delItemFromLocationMap(pli);
#else
			mapRegions->remove(pli);
#endif
			pli->Delete();
		}
	}
	return(hl);
}

bool house_speech( pChar pc, NXWSOCKET socket, std::string &talk)
{
	//
	// NOTE: Socket and pc checking allready done in talking()
	//
	int  fr;
	pItem pi = findmulti( pc->getPosition() );
	
	if( !pi )
		return false;

	//
	// if pc is not a friend or owner, we don't care what he says
	//
	fr=on_hlist(pi, pc->getSerial(), NULL);
	if( fr != H_FRIEND && pi->getOwnerSerial32() != pc->getSerial() )
		return false;
	//
	// house ban
	//
	if( talk.find("I BAN THEE") != std::string::npos )
	{
		pTarget targ = clientInfo[socket]->newTarget( new cCharTarget() );
		targ->code_callback=target_houseBan;
		targ->buffer[0]=pi->getSerial();
		targ->send( getClientFromSocket( socket) );
		sysmessage( socket, "Select person to ban from house.");
		return true;
	}
	//
	// kick out of house
	//
	if( talk.find("REMOVE THYSELF") != std::string::npos )
	{
		pTarget targ = clientInfo[socket]->newTarget( new cCharTarget() );
		targ->code_callback=target_houseEject;
		targ->buffer[0]=pi->getSerial();
		targ->send( getClientFromSocket( socket) );
		sysmessage( socket, "Select person to eject from house.");
		return true;
	}
	//
	// Lock down item
	//
	if ( talk.find("I WISH TO LOCK THIS DOWN") != std::string::npos )
	{
		pTarget targ = clientInfo[socket]->newTarget( new cItemTarget() );
		targ->code_callback=target_houseLockdown;
		targ->buffer[0]=pi->getSerial();
		targ->send( getClientFromSocket( socket) );
		sysmessage( socket, "Select item to lock down");
		return true;
	}
	//
	// Unlock down item
	//
	if ( talk.find("I WISH TO RELEASE THIS") != std::string::npos )
	{
		pTarget targ = clientInfo[socket]->newTarget( new cItemTarget() );
		targ->code_callback=target_houseRelease;
		targ->buffer[0]=pi->getSerial();
		targ->send( getClientFromSocket( socket) );
		sysmessage( socket, "Select item to release");
		return true;
	}
	//
	// Secure item
	//
	if ( talk.find("I WISH TO SECURE THIS") != std::string::npos )
	{
		pTarget targ = clientInfo[socket]->newTarget( new cItemTarget() );
		targ->code_callback=target_houseSecureDown;
		targ->buffer[0]=pi->getSerial();
		targ->send( getClientFromSocket( socket) );
		sysmessage( socket, "Select item to secure");
		return true;
	}
	return false;
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
void target_houseOwner( NXWCLIENT ps, pTarget t )
{
	pChar curr=ps->currChar();
	if ( ! curr ) return;

	pChar pc = cSerializable::findCharBySerial( t->getClicked() );
	if ( ! pc ) return;

	pItem pSign=cSerializable::findItemBySerial( t->buffer[0] );
	if ( ! pSign ) return;

	pItem pHouse=cSerializable::findItemBySerial(calcserial(pSign->more1, pSign->more2, pSign->more3, pSign->more4));
	if ( ! pHouse ) return;

	NXWSOCKET s = ps->toInt();
	if(pc->getSerial() == curr->getSerial())
	{
		sysmessage(s, "you already own this house!");
		return;
	}

	pSign->setOwnerSerial32(pc->getSerial());

	pHouse->setOwnerSerial32(pc->getSerial());

	killkeys( pHouse->getSerial() );


	NXWCLIENT osc=pc->getClient();
	NXWSOCKET os= (osc!=NULL)? osc->toInt() : INVALID;

	pItem pi3=item::CreateFromScript( "$item_gold_key" ); //gold key for everything else
	if ( ! pi3 ) return;
	
	pi3->setCurrentName( "a house key" );
	if(os!=INVALID)
	{
		pi3->setContainer( pc->getBackpack() );
	}
	else
	{
		pi3->MoveTo( pc->getPosition() );
	}
	pi3->Refresh();
	pi3->more1= pHouse->getSerial().ser1;
	pi3->more2= pHouse->getSerial().ser2;
	pi3->more3= pHouse->getSerial().ser3;
	pi3->more4= pHouse->getSerial().ser4;
	pi3->type=7;

	sysmessage(s, "You have transferred your house to %s.", pc->getCurrentName().c_str());
	char temp[520];
	sprintf(temp, "%s has transferred a house to %s.", curr->getCurrentName().c_str(), pc->getCurrentName().c_str());

	NxwSocketWrapper sw;
	sw.fillOnline( pc, false );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		NXWSOCKET k=sw.getSocket();
		if(k!=INVALID)
			sysmessage(k, temp);
	}
}

// buffer[0] house
void target_houseEject( NXWCLIENT ps, pTarget t )
{
	pChar pc = cSerializable::findCharBySerial(t->getClicked());
	if ( ! pc ) return;
	pItem pi_h=MAKE_ITEM_REF(t->buffer[0]);
	if ( ! pi_h ) return;

	NXWSOCKET s=ps->toInt();

	Location pcpos= pc->getPosition();

	uint32_t sx, sy, ex, ey;
	getMultiCorners(pi_h, sx,sy,ex,ey);
	if((pcpos.x>=(uint32_t)sx) && (pcpos.y>=(uint32_t)sy) && (pcpos.x<=(uint32_t)ex) && (pcpos.y<=(uint32_t)ey))
	{
		pc->MoveTo( ex, ey, pcpos.z );
		pc->teleport();
		sysmessage(s, "Player ejected.");
	} else
		sysmessage(s, "That is not inside the house.");

}

//buffer[0] house
void target_houseBan( NXWCLIENT ps, pTarget t )
{
	target_houseEject(ps, t);	// first, eject the player

	pChar pc = cSerializable::findCharBySerial( t->getClicked() );
	if ( ! pc ) return;

	pChar curr=ps->currChar();
	if ( ! curr ) return;

	NXWSOCKET s = ps->toInt();

	pItem pi=cSerializable::findItemBySerial( t->buffer[0] );
	if(pi)
	{
		if(pc->getSerial() == curr->getSerial())
			return;
		int r=add_hlist(pc, pi, H_BAN);
		if(r==1)
		{
			sysmessage(s, "%s has been banned from this house.", pc->getCurrentName().c_str());
		}
		else if(r==2)
		{
			sysmessage(s, "That player is already on a house register.");
		}
		else
			sysmessage(s, "That player is not on the property.");
	}
}

// buffer[0] the house
void target_houseFriend( NXWCLIENT ps, pTarget t )
{
	pChar Friend = cSerializable::findCharBySerial( t->getClicked() );

	pChar curr=ps->currChar();
	if ( ! curr ) return;

	NXWSOCKET s = ps->toInt();

	pItem pi=cSerializable::findItemBySerial( t->buffer[0] );

	if( Friend && pi)
	{
		if(Friend->getSerial() == curr->getSerial())
		{
			sysmessage(s,"You cant do that!");
			return;
		}
		int r=add_hlist(Friend, pi, H_FRIEND);
		if(r==1)
		{
			sysmessage(s, "%s has been made a friend of the house.", Friend->getCurrentName().c_str());
		}
		else if(r==2)
		{
			sysmessage(s, "That player is already on a house register.");
		}
		else
			sysmessage(s, "That player is not on the property.");
	}
}

// bugffer[0] the hose
void target_houseUnlist( NXWCLIENT ps, pTarget t )
{
	pChar pc = cSerializable::findCharBySerial( t->getClicked() );
	pItem pi= cSerializable::findItemBySerial( t->buffer[0] );
	NXWSOCKET s = ps->toInt();
    
	if(pc && pi)
	{
        	if ( del_hlist(pc, pi) >0)
			sysmessage(s, "%s has been removed from the house registry.", pc->getCurrentName().c_str());
		else 
			sysmessage(s, "That player is not on the house registry.");
	}
}

void target_houseLockdown( NXWCLIENT ps, pTarget t )
// PRE:     S is the socket of a valid owner/coowner and is in a valid house
// POST:    either locks down the item, or puts a message to the owner saying he's a moron
// CODER:   Abaddon
// DATE:    17th December, 1999
{

	pChar pc=ps->currChar();
	if ( ! pc ) return;
	NXWSOCKET s = ps->toInt();

    pItem pi=cSerializable::findItemBySerial( t->getClicked() );
    if(pi)
    {

        // time to lock it down!

        if( pi->isFieldSpellItem() )
        {
            sysmessage(s,"you cannot lock this down!");
            return;
        }
        if (pi->type==12 || pi->type==13 || pi->type==203)
        {
            sysmessage(s, "You cant lockdown doors or signs!");
            return;
        }
        if ( pi->IsAnvil() )
        {
            sysmessage(s, "You cant lockdown anvils!");
            return;
        }
        if ( pi->IsForge() )
        {
            sysmessage(s, "You cant lockdown forges!");
            return;
        }

        pItem multi = findmulti( pi->getPosition() );
        if( multi )
        {
            if(pi->magic==4)
            {
                sysmessage(s,"That item is already locked down, release it first!");
                return;
            }
            pi->magic = 4;  // LOCKED DOWN!
            clientInfo[s]->dragging=false;
            pi->setOwnerSerial32Only(pc->getSerial());
            pi->Refresh();
            return;
        }
        else
        {
            // not in a multi!
            sysmessage( s, "That item is not in your house!" );
            return;
        }
    }
    else
    {
        sysmessage( s, "Invalid item!" );
        return;
    }
}

void target_houseSecureDown( NXWCLIENT ps, pTarget t )
// For locked down and secure chests
{
	pChar pc=ps->currChar();
	if ( ! pc ) return;
	NXWSOCKET s = ps->toInt();

    pItem pi=cSerializable::findItemBySerial( t->getClicked() );
    if(pi)
    {
        // time to lock it down!

        if( pi->isFieldSpellItem() )
        {
            sysmessage(s,"you cannot lock this down!");
            return;
        }
        if (pi->type==12 || pi->type==13 || pi->type==203)
        {
            sysmessage(s, "You cant lockdown doors or signs!");
            return;
        }
        if(pi->magic==4)
        {
            sysmessage(s,"That item is already locked down, release it first!");
            return;
        }

        pItem multi = findmulti( pi->getPosition() );
        if( multi && pi->type==1)
        {
            pi->magic = 4;  // LOCKED DOWN!
            pi->secureIt = 1;
            clientInfo[s]->dragging=false;
            pi->setOwnerSerial32Only(pc->getSerial());
            pi->Refresh();
            return;
        }
        if(pi->type!=1)
        {
            sysmessage(s,"You can only secure chests!");
            return;
        }
        else
        {
            // not in a multi!
            sysmessage( s, "That item is not in your house!" );
            return;
        }
    }
    else
    {
        sysmessage( s, "Invalid item!" );
        return;
    }
}

void target_houseRelease( NXWCLIENT ps, pTarget t )
// PRE:     S is the socket of a valid owner/coowner and is in a valid house, the item is locked down
// POST:    either releases the item from lockdown, or puts a message to the owner saying he's a moron
// CODER:   Abaddon
// DATE:    17th December, 1999
// update: 5-8-00
{
	pChar pc=ps->currChar();
	if ( ! pc ) return;
	NXWSOCKET s = ps->toInt();

    pItem pi=cSerializable::findItemBySerial( t->getClicked() );
    if(pi)
    {
        if(pi->getOwnerSerial32() != pc->getSerial())
        {
            sysmessage(s,"This is not your item!");
            return;
        }
        if( pi->isFieldSpellItem() )
        {
            sysmessage(s,"you cannot release this!");
            return;
        }
        if (pi->type==12 || pi->type==13 || pi->type==203)
        {
            sysmessage(s, "You cant release doors or signs!");
            return;
        }
        // time to lock it down!
        pItem multi = findmulti( pi->getPosition() );
        if( multi && pi->magic==4 || pi->type==1)
        {
            pi->magic = 1;  // Default as stored by the client, perhaps we should keep a backup?
            pi->secureIt = 0;
            pi->Refresh();
            return;
        }
        else if( !multi )
        {
            // not in a multi!
            sysmessage( s, "That item is not in your house!" );
            return;
        }
    }
    else
    {
        sysmessage( s, "Invalid item!" );
        return;
    }
}
