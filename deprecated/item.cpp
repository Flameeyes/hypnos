  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "item.h"
#include "data.h"
#include "scp_parser.h"
#include "archive.h"
#include "map.h"


#include "inlines.h"
#include "scripts.h"


namespace item
{
	/*!
	\author Anthalir
	*/
	pItem CreateFromScript( char *itemname, cObject* cont, int amount )
	{
		int scid= xss::getIntFromDefine(itemname);
		if( scid==0 )
		{
			LogError("item '%s' is not defined in scripts", itemname);
			return NULL;
		}
		return item::CreateFromScript( scid, cont, amount );
	}

	/*!
	\author Xanathar
	\brief adds an item read from items.xss/scp
	\return the created item
	\param itemnum scriptid of the item
	\param cont container to add the item to
	*/
	pItem CreateFromScript( SCRIPTID itemnum, cObject* cont, int amount )
	{
		char 		sect[512];
		std::string 	lha;
		std::string 	rha;

		cScpIterator* 	iter = NULL;
		int 		tmp;

		bool 	finished = false;

		sprintf(sect, "SECTION ITEM %i", itemnum);

		if((iter = Scripts::Items->getNewIterator(sect)) == NULL)
			return NULL;

		pItem pi = new cItem(cItem::nextSerial());

		pi->animSetId(0);
		pi->setId( 0x0915 );
		amxVS.setServerMode();

		do
		{
			iter->parseLine( lha, rha );
			switch ( lha[0])
			{
				case '{':
					break;
				case '}':
					finished = true;
					break;
				case '@':
					if	( !LoadItemEventsFromScript( pi, const_cast<char*>(lha.c_str()), const_cast<char*>(rha.c_str()) ) )
						WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'A':
					if	( lha == "AMOUNT" )
						pi->amount = str2num(rha);
					else if ( lha == "ATT" )
						pi->att = getRangedValue(const_cast<char*>(rha.c_str()));
					//
					// Old style user variables
					//
					else if ( lha == "AMXFLAG0" )
						amxVS.insertVariable( pi->getSerial(), 0, str2num( rha ) );
					else if ( lha == "AMXFLAG1" )
						amxVS.insertVariable( pi->getSerial(), 1, str2num( rha ) );
					else if ( lha == "AMXFLAG2" )
						amxVS.insertVariable( pi->getSerial(), 2, str2num( rha ) );
					else if ( lha == "AMXFLAG3" )
						amxVS.insertVariable( pi->getSerial(), 3, str2num( rha ) );
					else if ( lha == "AMXFLAG4" )
						amxVS.insertVariable( pi->getSerial(), 4, str2num( rha ) );
					else if ( lha == "AMXFLAG5" )
						amxVS.insertVariable( pi->getSerial(), 5, str2num( rha ) );
					else if ( lha == "AMXFLAG6" )
						amxVS.insertVariable( pi->getSerial(), 6, str2num( rha ) );
					else if ( lha == "AMXFLAG7" )
						amxVS.insertVariable( pi->getSerial(), 7, str2num( rha ) );
					//
					// New style user variables
					//
					else if (  lha ==  "AMXINT" )
					{
						std::string rha1;
						std::string rha2;
						splitLine( rha, rha1, rha2 );
						amxVS.insertVariable( pi->getSerial(), str2num( rha1 ), str2num( rha2 ) );
					}
					else if ( lha == "AMXSTR"  )
					{
						std::string rha1;
						std::string rha2;
						splitLine( rha, rha1, rha2 );
						amxVS.insertVariable( pi->getSerial(), str2num( rha1 ), rha2 );
					}
					else if ( lha == "ANIMID" )
						pi->animSetId(hex2num(rha));
					else if ( lha == "AUXDAMAGE" )
						pi->auxdamage = str2num(rha);
					else if ( lha == "AUXDAMAGETYPE" )
						pi->auxdamagetype = static_cast<DamageType>(str2num(rha));
					else if ( lha == "AMMO" )
						pi->ammo = str2num(rha);
					else if ( lha == "AMMOFX" )
						pi->ammoFx = hex2num(rha);
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'C':
					if	( lha == "COLOR" )
					{
						tmp = hex2num(rha);
						pi->setColor(tmp);
					}
					else if ( lha == "CONTAINS" )
					{
						//
						// Sparhawk: 	for containers only, an easy way to fill a container with items
						//		Original idea by Pier Sotto Jox
						//
						switch( pi->type )
						{
							case ITYPE_CONTAINER		:
							case ITYPE_UNLOCKED_CONTAINER	:
							case ITYPE_LOCKED_CONTAINER	:
								{
									std::string	str_scriptId,
											str_itemAmount;

									splitLine( rha, str_scriptId, str_itemAmount );

									SCRIPTID	scriptId = str2num( str_scriptId );
									int32_t		itemAmount = str2num( str_itemAmount );

									if ( itemAmount < 1 )
										itemAmount = 1;

									pItem newpi = CreateFromScript( scriptId, pi, itemAmount );

									if( ! newpi )
										WarnOut( "CreateFromScript: invalid attribute %s %s", lha.c_str(), rha.c_str() );
								}
								break;
							default				:
								WarnOut( "CreateFromScript: %s attribute in non container item %i", lha.c_str(), itemnum );
						}
						break;
					}
					else if ( lha == "CREATOR" )
						pi->creator = rha;
					else if ( lha == "COLORLIST" )
					{
						std::string value(cObject::getRandomScriptValue("RANDOMCOLOR", rha ));
						pi->setColor( hex2num(value) );
					}
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'D':
					if	( lha == "DAMAGE" )
						pi->att = getRangedValue( const_cast<char*>(rha.c_str()) );
					else if ( lha == "DAMAGETYPE" )
						pi->damagetype = static_cast<DamageType>(str2num(rha));
					else if ( lha == "DEX" )
						pi->dx = str2num(rha);
					else if ( lha == "DISABLED" )
						pi->disabled = uiCurrentTime + (str2num(rha)*MY_CLOCKS_PER_SEC);// AntiChrist
					else if ( lha == "DISABLEMSG" ) {
						if( pi->disabledmsg )
							(*pi->disabledmsg) = rha;
						else
							pi->disabledmsg= new std::string( rha );
					}
					else if ( lha == "DISPELLABLE" )
						pi->setDispellable();
					else if ( lha == "DECAY" )
						pi->setDecay();
					else if ( lha == "DIR" )
						pi->dir = str2num(rha);
					else if ( lha == "DYE" )
						pi->dye = str2num(rha);
					else if ( lha == "DEXADD" )
						pi->dx2 = str2num(rha);
					else if ( lha == "DEF" )
						pi->def = str2num(rha);
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'F':
					if ( lha == "FIGHTSKILL" )
						pi->fightskill = static_cast<Skill>(str2num(rha)); // Added by Luxor
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'G':
					if ( lha == "GOOD" )
						pi->good = str2num(rha); // Added by Magius(CHE)
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'H':
					if ( lha == "HIDAMAGE" )
						pi->hidamage = str2num(rha);
					else if ( lha == "HP" )
						pi->hp = getRangedValue((char*)rha.c_str());
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'I':
					if ( lha == "ID" )
					{
						tmp = hex2num(rha);
						pi->setId( tmp );
					}
					else if ( lha == "ITEMLIST" )
					{
						pi->Delete();
						pi=item::CreateScriptRandomItem( const_cast<char*>(rha.c_str()), cont);
						/*
						if( pi )
							if( strcmp( script3, "") ) //ndEndy defined amount, 1 by default
								pi->amount=str2num( script3 );
						*/

					}
					else if ( lha == "INT" )
						pi->in = str2num(rha);
					else if ( lha == "INTADD" )
						pi->in2 = str2num(rha);
					else if ( lha == "ITEMHAND" )
						pi->itmhand = str2num(rha);
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'L':
					if ( lha == "LAYER" )
						pi->layer = pi->scriptlayer = str2num(rha);	//Luxor
					else if ( lha == "LODAMAGE" )
						pi->lodamage = str2num(rha);
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'M':
					if ( lha == "MORE" )
						pi->more = str2num(rha);
					else if ( lha == "MORE2" )
						pi->moreb = str2num(rha);
					else if ( lha == "MOVABLE" )
						pi->magic = str2num(rha);
					else if ( lha == "MAXHP" )
						pi->maxhp = str2num(rha); // by Magius(CHE)
					else if ( lha == "MOREX" )
						pi->morex = str2num(rha);
					else if ( lha == "MOREY" )
						pi->morey = str2num(rha);
					else if ( lha == "MOREZ" )
						pi->morez = str2num(rha);
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'N':
					if ( lha == "NEWBIE" )
						pi->setNewbie();
					else if ( lha == "NAME" )
						pi->setCurrentName(rha);
					else if ( lha == "NAME2" )
						pi->setSecondaryName(rha);
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'O':
					if ( lha == "OFFSPELL" )
						pi->offspell = str2num(rha);
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'P':
					if ( lha == "POISONED" )
						pi->poisoned = (PoisonType)str2num(rha);

					if ( lha == "PILE" )

						pi->pileable = str2num(rha);
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'R':
					if ( lha == "RANK" )
					{
						pi->rank = str2num(rha); // By Magius(CHE)
						if (pi->rank <= 0)
							pi->rank = 10;
					}
					else if ( lha == "REQSKILL" )
					{// Added by Luxor
						int params[2];
						fillIntArray( const_cast<char*>(rha.c_str()), params, 2, 0, 10);
						pi->reqskill[0] = params[0];
						pi->reqskill[1] = params[1];
					}
					else if ( lha == "RESISTS" )
					{// Added by Luxor
						int params[2];
						fillIntArray( const_cast<char*>(rha.c_str()), params, 2, 0, 10);
						if (params[0] < MAX_RESISTANCE_INDEX) {
							pi->resists[params[0]] = params[1];
						}
					}
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'S':
					if ( lha == "SK_MADE" )
						pi->madewith = str2num(rha); // by Magius(CHE)
					else if ( lha == "SMELT" )
						pi->smelt = str2num(rha);
					else if ( lha == "STR" )
						pi->st = str2num(rha);
					else if ( lha == "SPD" )
						pi->spd = str2num(rha);
					else if ( lha == "STRADD" )
						pi->st2 = str2num(rha);
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'T':
					if ( lha == "TYPE" )
						pi->type = str2num(rha);
					else if ( lha == "TRIGGER" )
						pi->trigger = str2num(rha);
					else if ( lha == "TRIGTYPE" )
						pi->trigtype = str2num(rha);
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'U':
					if ( lha == "USES" )
						pi->tuses = str2num(rha);
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'V':
					if ( lha == "VISIBLE" )
						pi->visible = str2num(rha);
					else if ( lha == "VALUE" )
						pi->value = str2num(rha);
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
				case 'W':
					if ( lha == "WEIGHT" )
					{
						pi->weight = str2num(rha);
					}
					else WarnOut("Unrecognised attribute : \"%s\", in item number %i\n", lha.c_str(), itemnum);
					break;
			}
		}
		while ( !finished );

		amxVS.setUserMode();
		pi->useAnimId=(pi->animid()!=0);

		tile_st tile;
		data::seekTile(pi->getId(), tile);

		if (tile.flags&TILEFLAG_STACKABLE)
			pi->pileable = 1;

		if (!pi->maxhp && pi->hp)
			pi->maxhp = pi->hp; // Magius(CHE)

		safedelete(iter);
		pi->setScriptID(itemnum);

		if( amount!=INVALID )
			pi->amount=amount;

		//Luxor: put it in the container
		if ( cont )
			if( isItemSerial( cont->getSerial() ) )
			{
				((pItem)cont)->AddItem(pi);
			}
			else
				pi->setContainer( cont );

		return pi;

	}

	int CreateRandomItem( char * sItemList )//NEW FUNCTION -- 24/6/99 -- AntiChrist merging codes
	{
		int i=0, loopexit=0, iList[256];  //-- no more than 256 items in a single item list
		char sect[512];
		cScpIterator* iter = NULL;
		char script1[1024];

		sprintf(sect, "SECTION ITEMLIST %s", sItemList);

		iter = Scripts::Items->getNewIterator(sect);
		if (iter==NULL) return INVALID;

		do  // -- count items storing item #'s in iList[]
		{
			strcpy(script1, iter->getEntry()->getFullLine().c_str());
			if ((script1[0]!='}')&&(script1[0]!='{'))
			{
				iList[i]=str2num(script1);
				i++;
			}
		}
		while ((script1[0]!='}')&&(++loopexit < MAXLOOPS));
		safedelete(iter);

		if (i==0) return iList[0]; else return(iList[rand()%i]);

	}

	pItem CreateScriptRandomItem( char * sItemList, cObject* cont )
	{
		int k=CreateRandomItem(sItemList);   // -- Get random Item #
		return (k>0)? item::CreateFromScript( k, cont ) : NULL;
	}

//not checked

	/*!
	\warning for internal use only
	\author Luxor
	*/
	static pItem spawnItemByIdInternal(int nAmount, const char* cName, short id, short color)
	{
		bool pile=false;

		tile_st tile;
		data::seekTile(id, tile);
		pile = (tile.flags&TILEFLAG_STACKABLE);

		pItem pi = item::CreateFromScript( "$item_hardcoded" );
		if ( ! pi ) return NULL;
		pi->setId( id );
		pi->setColor( color );
    		pi->pileable = pile;

		if(cName!=NULL)
			pi->setCurrentName(cName);

		pi->setDecay();
		return pi;
	}

	bool moreItemMemoryRequested = false;

	/*!
	\brief adds an item from spawner or gm 'ADDITEM command
	\return the pointer to the item added
	\param s socket of the gm or -1
	\param itemnum spawner item or -1
	\param nSpawned is spawned?
	\param cont container to add the item to
	*/
	pItem CreateScriptItem(NXWSOCKET s, int32_t itemnum, bool nSpawned, cObject* cont )
	{
		pItem pi= NULL;
		pi = item::CreateFromScript( itemnum,cont);
		if (!pi)
		{
			LogWarning("ITEM <%i> not found in the scripts",itemnum);
			return NULL;
		}

		if ((s!=INVALID) && (!nSpawned))
		{
			short xx,yy,zz;
			xx=(buffer[s][11]<<8)+buffer[s][12];
			yy=(buffer[s][13]<<8)+buffer[s][14];
			zz=buffer[s][16]+tileHeight((buffer[s][17]<<8)+buffer[s][18]);
			pi->MoveTo(xx,yy,(int8_t)zz);
		}
		else
		{
			if( cont ) {
				pi->setContainer( cont );
				if( isItemSerial( cont->getSerial() ) )
					cont->AddItem(pi);
				pi->Refresh();
			}
			else
				if (pi->isInWorld())
				{
#ifdef SPAR_I_LOCATION_MAP
					pointers::addToLocationMap( pi );
#else
					mapRegions->add(pi);
#endif
				}
		}

		return pi;

	}

	/*
	\author Duke
	\brief Create an item for a socket
	\note I took the idea (to overload) from the 'official' source. So thanks to fur!
	\param nSocket the socket to add the item for
	\param nAmount amount of item to spawn
	\param cName name of the item
	\param nStackable true if item is stackable
	\param cItemId item's ID
	\param cColorId item's color
	\param nPack if true the item is spawned in socket's backpack
	\param nSend if true the item is sent to all clients

	OK, here's the scoop, in a few key places we were calling add item when there was
	flat out no way to get a socket, say if a region spawner made an item, there is no
	associated socket.  This was causing the socket to be -1, and it was indexing into
	curchar[-1] and blowing up.  So I made a new version that takes a socket and a
	a character (the following function) if you want to call that one with socket = -1
	thats fine, because you have to pass the character in as well.  If you call this
	function (which was the original) you are not allowed to pass in -1 for the socket
	because it will crash. I've put the assert() in its place, so at least when it does
	crash you'll know why. If this assert() ever happens to you, it means you need to
	fix the CALLER of this function to pass the character in and call the other function
	instead. Taking the assert() out won't help, its just liable to crash on currchar[-1]
	anyways.  - fur

	what fur said about the assert only partially applies to this version. Duke

	*/
	/*pItem SpawnItem(NXWSOCKET  nSocket, int nAmount, char* cName, bool nStackable,
						int16_t cItemId, int16_t cColorId,
						bool nPack, bool nSend)
	{
		if (nSocket < 0)
		{
			ErrOut("SpawnItem called with bad socket\n");
			return NULL;
		}
		else
		{
			return item::SpawnItem(nSocket, currchar[nSocket], nAmount, cName, nStackable, cItemId, cColorId, nPack, nSend);
		}
	}*/

/*
	\param nSocket the socket to add the item for
	\param ch the character to add the item for
	\param nAmount amount of item to spawn
	\param cName name of the item
	\param nStackable true if item is stackable
	\param cItemId item's ID
	\param cColorId item's color
	\param nPack if true the item is spawned in socket's backpack
	\param nSend if true the item is sent to all clients
*/
	/*pItem SpawnItem(NXWSOCKET  nSocket, SERIAL ch,
						int32_t nAmount, char* cName, bool nStackable,
						int16_t cItemId, int16_t cColorId,
						bool nPack, bool nSend)
	{
		pItem pi= item::SpawnItem(ch, nAmount, cName, nStackable, cItemId, cColorId, nPack);
		if (pi==NULL) return NULL;
		if (nSend && nSocket!=INVALID) {
			pChar pc=MAKE_CHAR_REF(currchar[nSocket]);
			if ( ! pc ) return NULL;
			statwindow(pc,MAKE_CHAR_REF(ch));
		}
		return pi;
	}*/

	/*!
	\author Magius(CHE), bugfixed by AntiChrist
	*/
	void GetScriptItemSetting(pItem pi)
	{
		//ConOut("GetScriptItemSetting....");//debug


		int loopexit=0;
		cScpIterator* iter = NULL;
		char script1[1024];
		char script2[1024];
		int tmp;
		bool sectfound=false;
		char buff[512];


		if(pi == NULL) return;

		sprintf(buff,"x%x",pi->getId());


		pi->animSetId(0);


		safedelete(iter);
		iter = Scripts::HardItems->getNewIteratorInStr(buff);

		if (iter==NULL) return;


		do
		{
			iter->parseLine(script1, script2);
			if ((script1[0]!='}')&&(script1[0]!='{'))
			{
				if (script1[0] != 'S')
					continue;
				if (!(strcmp("SECTION",script1)))
					if(strstr(script2,buff)) sectfound = true;//AntiChrist bugfix

				if(sectfound)//AntiChrist bugfix
				{
					switch (script1[0])
					{
					case 'A':
					case 'a':
						if (!(strcmp("AMOUNT",script1))) pi->amount=str2num(script2); // -Fraz- moved from Case C
						else if (!strcmp("ANIMID", script1)) pi->animSetId(hex2num(script2));
						break;
					case 'C':
					case 'c':
						if (!(strcmp("CREATOR", script1))) pi->creator = script2; // by Magius(CHE)
						else if (!(strcmp("COLOR",script1)))
						{
							tmp=hex2num(script2);
							pi->setColor( tmp );
						}
					break;

					case 'D':
					case 'd':
						if (!(strcmp("DEX", script1))) pi->dx=str2num(script2);
						else if (!(strcmp("DEXADD", script1))) pi->dx2=str2num(script2);
						else if ((!(strcmp("DAMAGE",script1)))||(!(strcmp("ATT",script1))))
							pi->att = getRangedValue(script2);
						else if (!(strcmp("DEF",script1))) pi->def=str2num(script2);
						else if (!(strcmp("DYE",script1))) pi->dye=str2num(script2);
						else if (!(strcmp("DIR",script1))) pi->dir=str2num(script2);
						else if (!(strcmp("DECAY",script1))) pi->setDecay();
						else if (!(strcmp("DISPELLABLE",script1))) pi->setDispellable();
						else if (!(strcmp("DISABLEMSG",script1))) {
							if( pi->disabledmsg!=NULL )
								(*pi->disabledmsg) = script2;
							else
								pi->disabledmsg = new std::string( script2 );
						}
						else if (!(strcmp("DISABLED",script1))) pi->disabled=uiCurrentTime+(str2num(script2)*MY_CLOCKS_PER_SEC);//AntiChrist
					break;

					case 'G':
					case 'g':
					case 'H':
					case 'h':
						if (!(strcmp("GOOD",script1))) pi->good=str2num(script2); // Added by Magius(CHE)
						else if (!(strcmp("HP", script1))) pi->hp=str2num(script2);
						else if (!(strcmp("HIDAMAGE", script1))) pi->hidamage=str2num(script2);
					break;

					case 'I':
					case 'i':
					case 'L':
					case 'l':
						if (!(strcmp("INT", script1))) pi->in=str2num(script2);
						else if (!(strcmp("INTADD", script1))) pi->in2=str2num(script2);
						else if (!(strcmp("ITEMHAND",script1))) pi->itmhand=str2num(script2);
						else if (!(strcmp("LAYER",script1))) pi->layer=str2num(script2);
						else if (!(strcmp("LODAMAGE", script1))) pi->lodamage=str2num(script2);
					break;

					case 'M':
					case 'm':
						if (!(strcmp("MAXHP", script1))) pi->maxhp=str2num(script2); // by Magius(CHE)
						else if (!(strcmp("MOVABLE",script1))) pi->magic=str2num(script2);
						else if (!(strcmp("MORE", script1))) pi->more = str2num(script2);
						else if (!(strcmp("MORE2", script1))) pi->moreb = str2num(script2);
						else if (!(strcmp("MOREX",script1))) pi->morex=str2num(script2);
						else if (!(strcmp("MOREY",script1))) pi->morey=str2num(script2);
						else if (!(strcmp("MOREZ",script1))) pi->morez=str2num(script2);
					break;

					case 'N':
					case 'n':
					case 'O':
					case 'o':
						if (!(strcmp("NAME",script1))) pi->setCurrentName(script2);
						else if (!(strcmp("NAME2",script1))) pi->setSecondaryName(script2);
						else if (!(strcmp("NEWBIE",script1))) pi->setNewbie();
						else if (!(strcmp("OFFSPELL",script1))) pi->offspell=str2num(script2);

					break;

					case 'P':
					case 'p':
					case 'R':
					case 'r':
						if (!strcmp("POISONED",script1)) pi->poisoned=(PoisonType)str2num(script2);
						else if (!strcmp("RANK",script1))
						{
							pi->rank=str2num(script2); // By Magius(CHE)
							if (pi->rank==0) pi->rank=10;
						}
					break;

					case 'S':
					case 's':
						if (!(strcmp("SPD",script1))) pi->spd=str2num(script2);
						else if (!(strcmp("SK_MADE", script1))) pi->madewith=str2num(script2); // by Magius(CHE)
						else if (!(strcmp("STR", script1))) pi->st=str2num(script2);
						else if (!(strcmp("STRADD", script1))) pi->st2=str2num(script2);
						else if (!(strcmp("SMELT", script1))) pi->smelt=str2num(script2);
					break;

					case 'T':
					case 't':
					case 'U':
					case 'u':
						if (!(strcmp("TRIGGER",script1))) pi->trigger=str2num(script2);
						else if (!(strcmp("TRIGTYPE",script1))) pi->trigtype=str2num(script2);
						else if (!(strcmp("TYPE",script1))) pi->type=str2num(script2);
						else if (!(strcmp("USES",script1))) pi->tuses=str2num(script2);
					break;

					default:
						if (!(strcmp("VISIBLE",script1))) pi->visible=str2num(script2);
						else if (!(strcmp("VALUE",script1))) pi->value=str2num(script2);

						else if (!(strcmp("WEIGHT",script1)))
						{
							int anum=3;
							//anum=4;
							anum=str2num(script2); // Ison 2-20-99
							pi->weight=anum;
							//ConOut("SETTING WEIGTH TO %i\n",pi->weight);
						}
						//Added by Krozy on 7-Sep-98
					break;
					}
				}
			}
			if(script1[0]=='}') break;
		}
		while ((script1[0]!='}') && (++loopexit < MAXLOOPS) );

		safedelete(iter);
	}

	pItem SpawnRandomItem(NXWSOCKET s, char* cList, char* cItemID)
	{

		pChar pc=MAKE_CHAR_REF( currchar[s] );
		if ( ! pc ) return NULL;

		char sect[512];
		char script1[1024];
		cScpIterator* iter = NULL;
		int i=0,item[256]={0};


		sprintf(sect, "SECTION %s %s", cList, cItemID);

		iter = Scripts::Necro->getNewIterator(sect);
		if (iter==NULL) return NULL;

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
			if(item[i]!=INVALID) {
				return item::CreateFromScript( item[i], pc->getBackpack());
			}
		}
		return NULL;

	}
};
