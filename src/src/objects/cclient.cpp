/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cClient class
*/

#include "cclient.h"
#include "settings.h"

static ClientList cClient::clients;
static ClientList cClient::cGMs;

cClient::cClient(int32_t sd, struct sockaddr_in* addr)
{
	sock = new cSocket(sd, addr);
	pc = NULL;
	acc = NULL:
        visualRange = VISRANGE;
        clients.push_back(this);
}

cClient::~cClient()
{
	if ( pc )
		pc->setClient(NULL);
	if ( acc )
		acc->setClient(NULL);

	delete sock;
        clients.erase(find(clients.begin(), clients.end(), this));
}

/*!
\brief Compress packet buffer
\param out_buffer buffer to send
\param out_len size of buffer to send
*/
void cClient::compress(uint8_t *&out_4buffer, uint32_t& out_len)
{
uint8_t *new_buffer = new uint8_t[out_len];
uint32_t new_len=0, tmp_len=out_len;

	if(out_len <= 0)
		return;

	for(i = 0; i < out_len; i++ ) {

		n_bits = bit_table[out_buffer[i]][0];
		value = bit_table[out_buffer[i]][1];

		while(n_bits--) {
//			new_buffer[new_len] = (new_buffer[new_len] << 1) | (uint8_t)((value >> n_bits) & 1);
			new_buffer[new_len] <<= 1;
			new_buffer[new_len] |= (uint8_t)((value >> n_bits) & 1);

			bit_4_byte++;
			if(bit_4_byte / 8) {
				new_len++;
				bit_4_byte %= 8;
			}
		}
	}

	n_bits = bit_table[256][0];
	value = bit_table[256][1];

	if(n_bits <= 0) {
		out_len = 0;
		return;
	}

	while(n_bits--) {
//		new_buffer[new_len] = (new_buffer[new_len] << 1) | (uint8_t)((value >> n_bits) & 1);
		new_buffer[new_len] <<= 1;
		new_buffer[new_len] |= (uint8_t)((value >> n_bits) & 1);

		bit_4_byte++;
		if(bit_4_byte / 8) {
			new_len++;
			bit_4_byte %= 8;
		}
	}

	if(bit_4_byte) {
		while(bit_4_byte < 8) {
			new_buffer[new_len] <<= 1;
			bit_4_byte++;
		}
		new_len++;
	}

	delete out_buffer;
	out_buffer = new_buffer;
	out_len = new_len;
}

/*!
\brief Show a container to player
\author Kheru - rewrote by Flameeyes
\param cont the container
*/
void cClient::showContainer(pCont cont)
{
	if ( ! cont )
		return;

	NxwItemWrapper si;
	si.fillItemsInContainer( cont, false, false );
	int32_t count=si.size();

	cPacketSendDrawContainer pk(cont->getSerial(), cont->getGump());
	sendPacket(&pk);

	cPacketSendContainerItem pk2;

	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi=si.getItem();
		if ( ! pi )
			continue;

		//fix location of items if they mess up. (needs tweaked for container types)
		//! \todo The position of the items should be bound to client-specific protocol
		if (pi->getPosition().x > 150) pi->setPosition("x", 150);
		if (pi->getPosition().y > 140) pi->setPosition("y", 140);

		pk2.addItem(pi);
	}

	sendPacket(&pk2);
}

/*!
\brief Show an item into a container
\author Flameeyes
\param item item to show
*/
void cClient::showItemInContainer(pItem item)
{
	if ( ! item || pc->distFrom(pi) > VISRANGE )
		return;

	cPacketSendShowItemInContainer pk( item );

	sendPacket(&pk);

	pc->getBody()->calcWeight();
}


/*!
\brief Set the light level
\param level base light level
*/
void cClient::light(uint8_t level)
{
	if ( ! pc ) return;

	if (worldfixedlevel != 0xFF)
		level = worldfixedlevel;
	else if (pc->getFixedLight() != 0xFF)
		level = pc->getFixedLight();
	else if (pc->inDungeon())
		level = dungeonlightlevel;

	cPacketSendOverallLight pk(level);

	sendPacket(&pk);
}


void cClient::updatePaperdoll()
{
	cPacketSendPaperdollClothingUpdated pk();
	sendPacket(&pk);
}


void cClient::showBankBox(pPC dest)
{
	if ( ! dest || (dest != pc && acc->getPrivLevel() < privSeer ) )
		return;

	pItem bank = dest->getBankBox();
	if ( ! bank )
		return;

	showClient(bank);
}


/*!
\brief region specific bankbox
\author Endymion
\param pc character owner of bank

If activated, you can only put golds into normal banks
and there are special banks (for now we still use normal bankers,
but u have to say the SPECIALBANKTRIGGER word to open it)
where u can put all the items: one notice: the special bank
is caracteristic of regions....so in Britain you don't find
the items you leaved in Minoc!
All this for increasing pk-work and commerce! :)
(and surely the Mercenary work, so now have to pay strong
warriors to escort u during your travels!)
*/
void cClient::showSpecialBankBox(pPC dest)
{
	if ( ! dest || (dest != pc && acc->getPrivLevel() < privSeer ) )
		return;

	pItem bank = dest->getSpecialBankBox();
	if ( ! bank )
		return;

	showClient(bank);
}

/*!
\brief send status window of target to client
\param target character whose status window has to be reported to client
\param extended if extended info needed
*/

void cClient::statusWindow(pChar target, bool extended) //, bool canrename)  will be calculated from client data
{

	VALIDATEPC(target);
        cChar pc = currChar();  // pc who will get the status window

        bool canrename;

	if (pc->IsGM() || ((target->getOwnerSerial32()==pc->getSerial()) && (target!=pc))) canrename = true;
	else canrename = false;

	if ((pc->getBody()->getId() == BODY_DEADMALE) || (pc->getBody()->getId() == BODY_DEADFEMALE)) canrename = false;
       	uint8_t ext;
        if (extended)
        {
                ext = 0x01;
	        if ( flags & flagClientIsUO3D )
		        ext = 0x03;
        	if ( flags & flagClientIsAoS )
	        	ext = 0x04;
        }
        else ext = 0x0;

	cPacketSendStatus pk(sorg, ext, canrename);
	sendPacket(&pk);
}

/*!
\brief updates status window of client if drag & drop has modified player in any way, weight included
\param item item moved in drag & drop
*/

void cClient::updateStatusWindow(pItem item)
{
	pChar pc = currchar();
	pItem pack = currchar()->getBackpack();
	if ( ! item || ! pack )
		return;

	if( item->getContainer() != pack || item->getContainer() == pc )     //!< if item was in pack and has been moved out or has been equipped/deequipped update char
		statusWindow( pc, true );
}



/*!
\brief brings up the skill window in client
*/
void cClient::skillWindow() // Opens the skills list, updated for client 1.26.2b by LB
{

	pChar pc = currChar();
	if ( ! pc ) return;

        //!  \todo: port this into cPackets since it is a raw packet

	uint8_t skillstart[4]={ 0x3A, 0x00, };
	uint8_t skillmid[7]={ 0x00, };
	uint8_t skillend[2]={ 0x00, };
	uint16_t len;
	char x;

	len = 0x015D;					// Hardcoded -_-;  // hack for that 3 new skills+1.26.2 client, LB 4'th dec 1999
	ShortToCharPtr(len, skillstart +1);
	skillstart[3] = 0x00;				// Type:
							// 0x00 = full list, 0xFF = single skill update,
							// 0x02 = full list with skillcap, 0xDF = single skill update with cap

	Xsend(s, skillstart, 4);
	for (int i=0;i<skTrueSkills;i++)
	{
		Skills::updateSkillLevel(pc,i);
		ShortToCharPtr(i+1, skillmid +0);
		ShortToCharPtr(pc->skill[i], skillmid +2);
		ShortToCharPtr(pc->baseskill[i], skillmid +4);

		x=pc->lockSkill[i];
		if (x!=0 && x!=1 && x!=2) x=0;
		skillmid[6]=x; // leave it unlocked, regardless
		Xsend(s, skillmid, 7);
	}
	Xsend(s, skillend, 2);
//AoS/	Network->FlushBuffer(s);
}


void sendMidi(char num1, char num2)
{
	UI16 music_id = (num1<<8)|(num2%256);
	cPacketSendPlayMidi pk(music_id);
	sendPacket(&pk);
}

void senditem(pItem pi) // Shows items to client (on the ground or inside containers)
{
	if ( ! pi ) return;
	pChar pc= currChar();
	if ( ! pc ) return;

       	if (!pc->hasInRange(pi, visualRange) ) return; //we must check on client's selected visual range for items to send (see packet 0xc8)
	if ( pi->visible==2 && !pc->isGM()) return;
        if ( pi->visible==1 && pc->getSerial()!=pi->getOwnerSerial32() && !pc->isGM()) return; //On visible set to 1, only owners or GMs see the item
	// meaning of the item's attribute visible
	// Visible 0 -> visible to everyone
	// Visible 1 -> only visible to owner and gm's (for owners normal for gm's grayish/hidden color)
	// visible 2 -> only visble to gm's (greyish/hidden color)

	if (!pi->isInWorld())
	{
         	// If flow arrives here, item is not in world. Now we must check if its container is a true container
		pContainer container = dynamic_cast<pContainer> pi->getContainer();
                if (container && pi->getId()<0x4000)
		{
                        // LB client crashfix, dont show multis in BP
			// we should better move it out of pack, but thats
			// only a first bannaid
			showItemInContainer(pi);
		}
                return;
	}

	cPacketSendObjectInformation pk(pi, pc);
        sendPacket(&pk);
	if (pi->isCorpse()) backpack2(pi);
}

// sends item in differnt color and position than it actually is
// used for LSd potions now, LB 5'th nov 1999
void senditem_lsd(pItem pi, uint16_t color, Location position)
{
	if ( ! pi ) return;
	pChar pc= currChar();
	if ( ! pc ) return;

	if ( pi->visible>=1 && !(pc->IsGM()) ) return; // workaround for missing gm-check client side for visibity since client 1.26.2
	// for lsd we dont need extra work for type 1 as in senditem

       	cPacketSendLSDObject pk(pi, pc, color, position);
        sendPacket(&pk);
}

/*------------------------------------------------------------------------------
                        DRAG & DROP METHODS
------------------------------------------------------------------------------*/

/*!
\brief Get an item
\author Unknow, revamped by Endymion, then Chronodt (3/2/2004)
\param  pi item to get
\param amount amount of *pi to get
*/
void cClient::get_item( pItem pi, uint16_t amount ) // Client grabs an item
{
	pChar pc_currchar = currChar();
	VALIDATEPC( pc_currchar );

	//Luxor: not-movable items
	/*if (pi->magic == 2 || (isCharSerial(pi->getContSerial()) && pi->getContSerial() != pc_currchar->getSerial()) ) {
		if (isCharSerial(pi->getContSerial())) {
			pChar pc_i = cSerializable::findCharBySerial(pi->getContSerial());
			if ( pc_i )
				pc_i->sysmsg("Warning, backpack bug located!");
		}
		if (client->isDragging()) {
        		client->resetDragging();
			updateStatusWindow(pi);
        	}
		pi->setContainer( pi->getOldContainer() );
		pi->setPosition( pi->getOldPosition() );
		pi->layer = pi->oldlayer;
		pi->Refresh();
		return;
	}*/

	pc_currchar->disturbMed(); // Meditation

	tile_st item;
 	data::seekTile( pi->getId(), item );

	// Check if item is equiped
 	if( pi->getContainer == pc_currchar && pi->layer == item.quality )
 	{
 		if( pc_currchar->UnEquip( pi, 1 ) == 1 )	// bypass called
 		{
 			if( isDragging() )
 			{
 				resetDragging();
				updatePaperdoll();
 			}
 			return;
 		}
 	}


	pChar owner=NULL;
	pItem container=NULL;
	if ( !pi->isInWorld() ) { // Find character owning item

		if ( isCharSerial(pi->getContainer()->getSerial()) )  // or: isChar(pi->getContainer()) -.^?
		{
			owner = (pChar) pi->getContainer();
		}
		else  // its an item
		{
			//Endymion Bugfix:
			//before check the container.. but if this cont is a subcont?
			//so get the outmostcont and check it else:
			//can loot without lose karma in subcont
			//can steal in trade ecc
			//not very good :P
			container = pi->getOutMostCont();
			if( isCharSerial( container->getContainer()->getSerial() ) ) //see above ...
				owner = (pChar) container->getContainer();
		}

		if ( owner && (owner != pc_currchar) )
		{
			if ( !pc_currchar->IsGM() && owner->getOwnerSerial32() != pc_currchar->getSerial() )
			{// Own serial stuff by Zippy -^ Pack aniamls and vendors.
                        //! \todo the sendpacket stuff here
				uint8_t bounce[2]= { 0x27, 0x00 };
				Xsend(s, bounce, 2);
//AoS/				Network->FlushBuffer(s);
				if (isDragging())
				{
					resetDragging();
					pi->setOldContainer(pi->getContainer());
					item_bounce3(pi);
				}
				return;
			}
		}
	}

	if ( container )
	{

		if ( container->layer == 0 && container->getId() == 0x1E5E)
		{
			// Trade window???
			uint32_t serial = calcserial( pi->moreb1, pi->moreb2, pi->moreb3, pi->moreb4);
			if ( serial == INVALID )
				return;

			pItem piz = cSerializable::findItemBySerial(serial );
			if ( piz )
				if ( piz->morez || container->morez )
				{
					piz->morez = 0;
					container->morez = 0;
					sendtradestatus( piz, container );
				}


			//<Luxor>
			if (pi->amxevents[EVENT_ITAKEFROMCONTAINER]!=NULL)
			{
				g_bByPass = false;
				pi->amxevents[EVENT_ITAKEFROMCONTAINER]->Call( pi->getSerial(), pi->getContSerial(), pc_currchar->getSerial() );
				if (g_bByPass)
				{
                                //! \todo the sendpacket stuff here
					Sndbounce5(s);
					if (isDragging())
					{
						resetDragging();
						updateStatusWindow(pi);
					}
					pi->setContainer( pi->getOldContainer() );
					pi->setPosition( pi->getOldPosition() );
					pi->layer = pi->oldlayer;
					pi->Refresh();
					return;
                		}
			}
			//</Luxor>

			/*
			//<Luxor>
			g_bByPass = false;
			pi->runAmxEvent( EVENT_ITAKEFROMCONTAINER, pi->getSerial(), pi->getContSerial(), s );
			if (g_bByPass)
			{
				Sndbounce5(s);
				if (client->isDragging())
				{
					client->resetDragging();
					updateStatusWindow(pi);
				}
				pi->setContainer( pi->getOldContainer() );
				pi->setPosition( pi->getOldPosition() );
				pi->layer = pi->oldlayer;
				pi->Refresh();
				return;
			}
			//</Luxor>
			*/

			if ( container->corpse )
			{
				if ( container->getOwnerSerial32() != pc_currchar->getSerial())
				{ //Looter :P

					pc_currchar->unHide();
					bool bCanLoot = false;
					if( pc_currchar->party!=INVALID ) {
						pChar dead = cSerializable::findCharBySerial( container->getOwnerSerial32() ) ;
						if( dead && dead->party==pc_currchar->party ) {
							P_PARTY party = Partys.getParty( pc_currchar->party );
							if( party!=NULL ) {
								P_PARTY_MEMBER member = party->getMember( pc_currchar->getSerial() );
								if( member!=NULL )
									bCanLoot = member->canLoot;
							}
						}
					}
					if ( !bCanLoot && container->more2==1 )
					{
						pc_currchar->IncreaseKarma(-5);
						//!\todo should be investigated
						pc_currchar->setCrimGrey(ServerScp::g_nLootingWillCriminal);
						pc_currchar->sysmsg("You are loosing karma!");
					}
				}
			} // corpse stuff

			container->SetMultiSerial(INVALID);

			//at end reset decay of container
			container->setDecayTime();

		} // end cont valid
	}

	if ( !pi->corpse )
	{
		updateStatusWindow(pi);

		tile_st tile;
		data::seekTile( pi->getId(), tile);

		if (!pc_currchar->IsGM() && (( pi->magic == 2 || ((tile.weight == 255) && ( pi->magic != 1))) && !pc_currchar->canAllMove() )  ||
			(( pi->magic == 3|| pi->magic == 4) && !pc_currchar->isOwnerOf( pi )))
		{
                        //! \todo the sendpacket stuff here
			uint8_t bounce[2]={ 0x27, 0x00 };
			Xsend(s, bounce, 2);
//AoS/			Network->FlushBuffer(s);
			if (isDragging()) // only restore item if it got draggged before !!!
			{
				resetDragging();
				item_bounce4( pi );
			}
		} // end of can't get
		else
		{
			// AntiChrist bugfix for the bad bouncing bug ( disappearing items when bouncing )
			setDragging();
			pi->setOldPosition( pi->getPosition() ); // first let's save the position

			pi->oldlayer = pi->layer;	// then the layer
			pi->layer = 0;

			if (!pi->isInWorld())
				pc_currchar->playSFX(0x0057);

			if (pi->getAmount()>1)
			{
				if (amount > pi->getAmount())
					amount = pi->getAmount();
				else if (amount < pi->getAmount())
				{ //get not all but a part of item ( piled? ), but anyway make a new one item

					pItem pin = new cItem(cItem::nextSerial());
					(*pin)=(*pi);

					pin->setAmount(pi->getAmount() - amount);

					pin->setContainer(pi->getContainer());	//Luxor
					pin->setPosition( pi->getPosition() );

					/*if( !pin->isInWorld() && isItemSerial( pin->getContainer()->getSerial() ) )
						pin->SetRandPosInCont( (pItem)pin->getContainer() );*/

					statusWindow(pc_currchar, true);
					pin->Refresh();//AntiChrist
				}

				if ( pi->getId() == ITEMID_GOLD)
				{
					pItem pack= pc_currchar->getBackpack();
					if ( pack )
						if ( pi->getContainer() == pack )
							statusWindow(pc_currchar, true);
				}

				pi->setAmount(amount);

			} // end if corpse
#ifdef SPAR_I_LOCATION_MAP
			pointers::delFromLocationMap( pi );
#else
			mapRegions->remove( pi );
#endif
			pi->setPosition( 0, 0, 0 );
			pi->setContainer(0);
		}
	}

	pc_currchar->weight += pi->getWeightActual();
	statusWindow(pc_currchar, true);
}


/*!
\brief Drop an item
\author Unknown, moved here by Chronodt (3/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
*/

void cClient::drop_item(pItem pi, Location &loc, pItem cont) // Item is dropped
{

    //#define debug_dragg

	if (clientDimension==3)
	{
	  // UO:3D clients send SOMETIMES two dragg packets for a single dragg action.
	  // sometimes we HAVE to swallow it, sometimes it has to be interpreted
	  // if UO:3D specific item loss problems are reported, this is probably the code to blame :)
	  // LB

	  #ifdef debug_dragg
	    if ( pi ) { sprintf(temp, "%04x %02x %02x %01x %04x i-name: %s EVILDRAG-old: %i\n",pi->getSerial(), loc->x, loc->y, loc->z, cont->getSerial(), pi->name, evilDrag); ConOut(temp); }
		else { sprintf(temp, "blocked: %04x %02x %02x %01x %04x i-name: invalid item EVILDRAG-old: %i\n",pi->getSerial(), loc->x, loc->y, loc->z, cont->getSerial(), evilDrag); ConOut(temp); }
	  #endif

	  if  ( (loc->x==-1) && (loc->y==-1) && (loc->z==0)  && (evilDrag) )
	  {
		  evilDrag=false;
          #ifdef debug_dragg
		    ConOut("Swallow only\n");
          #endif
		  return;
	  }	 // swallow! note: previous evildrag !

	  else if ( (loc->x==-1) && (loc->y==-1) && (loc->z==0)  && (!evilDrag) )
	  {
          #ifdef debug_dragg
		    ConOut("Bounce & Swallow\n");
          #endif
		  item_bounce6( pi);
		  return;
	  }
	  else if ( ( (loc->x!=-1) && (loc->y!=-1) && ( cont->getSerial()!=-1)) || ( (pi->getSerial()>=0x40000000) && (cont->getSerial()>=0x40000000) ) )
		  evilDrag=true; // calc new evildrag value
	  else evilDrag=false;
	}

	#ifdef debug_dragg
	  else
	  {
	     if ( pi ) { sprintf(temp, "blocked: %04x %02x %02x %01x %04x i-name: %s EVILDRAG-old: %i\n",pi->getSerial(), loc->x, loc->y, loc->z, cont->getSerial(), pi->name, evilDrag); ConOut(temp); }
	  }
	#endif


	if ( isItemSerial(cont->getSerial()) && (cont->getSerial() != INVALID)  ) // Invalid target => invalid container => put inWorld !!!
		pack_item(pi, loc, cont);
	else
		dump_item(pi, loc, cont);
}

/*!
\brief put item into a container
\author Unknown, moved here by Chronodt (3/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
*/


void cClient::pack_item(pItem pi, Location &loc, pItem cont) // Item is put into container
{

	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	char temp2[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	int serial/*, serhash*/;
	tile_st tile;
//	bool abort=false;

	pChar pc= currChar();
	if ( ! pc ) return;

	Location charpos = pc->getPosition();

	pItem pack;

	if (pi->getId() >= 0x4000)
	{
//		abort=true; // LB crashfix that prevents moving multi objcts in BP's
		sysmsg("Hey, putting houses in your pack crashes your back and client!");
	}

	//ndEndy recurse only a time
	pItem contOutMost = cont->getOutMostCont();
//	pChar contOwner = ( !contOutMost->isInWorld() )? cSerializable::findCharBySerial( contOutMost->getContainer()->getSerial() ) : NULL;
	pChar contOwner = (pChar) contOutMost->getContainer();

	if( contOwner ) {
		//if ((contOwner->npcaitype==NPCAI_PLAYERVENDOR) && (contOwner->npc) && (contOwner->getOwnerSerial32()!=pc->getSerial()) )
		if ( contOwner->getSerial() != pc->getSerial() && contOwner->getOwnerSerial32() != pc->getSerial() && !pc->IsGM() ) { // Luxor
			sysmsg("This aint your backpack!");
                        //! \todo the sendpacket stuff here
			Sndbounce5(s);
			if (isDragging()) {
				resetDragging();
				item_bounce3(pi);
				if (cont->getId() >= 0x4000)
					senditem(s, cont);
			}
			return;
		}
	}

	if (cont->amxevents[EVENT_IONPUTITEM]!=NULL) {
		g_bByPass = false;
		cont->amxevents[EVENT_IONPUTITEM]->Call( cont->getSerial(), pi->getSerial(), pc->getSerial() );
		if (g_bByPass)
		{
			item_bounce6(pi);
			return;
		}
	}
	if (cont->layer==0 && cont->getId() == 0x1E5E &&
		cont->getContainer() == pc)
	{
		// Trade window???
		serial=calcserial(cont->moreb1, cont->moreb2, cont->moreb3, cont->moreb4);
		if(serial==-1) return;

		pItem pi_z = cSerializable::findItemBySerial(serial);

		if ( pi_z )
			if ((pi_z->morez || cont->morez))
			{
				pi_z->morez=0;
				cont->morez=0;
				sendtradestatus( pi_z, cont );
			}
	}

	if(SrvParms->usespecialbank)//only if special bank is activated
	{
		if(cont->morey==MOREY_GOLDONLYBANK && cont->morex==MOREX_BANK && cont->type==ITYPE_CONTAINER)
		{
			if ( pi->getId() == ITEMID_GOLD )
			{//if they're gold ok
				pc->playSFX( goldsfx(2) );
			} else
			{//if they're not gold..bounce on ground
				sysmsg("You can only put golds in this bank box!");

				pi->setContainer(0);
				pi->MoveTo( charpos );
				pi->Refresh();
				pc->playSFX( itemsfx(pi->getId()) );
				return;
			}
		}
	}

	// Xanathars's Bank Limit Code
	if ( nSettings::Server::getBankMaxItems() )
	{
		if(  contOutMost && contOutMost->morex==MOREX_BANK ) {

			int n = contOutMost->CountItems( INVALID, INVALID, false);
			n -= contOutMost->CountItems( ITEMID_GOLD, INVALID, false);
			if( pi->type == ITYPE_CONTAINER )
				n += pi->CountItems( INVALID, INVALID, false);
			else
				++n;
			if( n > nSettings::Server::getBankMaxItems() ) {
				sysmsg("You exceeded the number of maximimum items in bank of %d", nSettings::Server::getBankMaxItems());
				item_bounce6(pi);
				return;
			}

		}
	}


	//ndEndy this not needed because when is dragging cont serial is INVALID
	//testing UOP Blocking Tauriel 1-12-99
	if (!pi->isInWorld())
	{
		item_bounce6(pi);
		return;
	}

	data::seekTile(pi->getId(), tile);
	if ((((pi->magic==2)||((tile.weight==255)&&(pi->magic!=1)))&&!pc->canAllMove) ||
				( (pi->magic==3|| pi->magic==4) && !(pi->getOwnerSerial32()==pc->getSerial())))
	{
        //! \todo the sendpacket stuff here
		Sndbounce5(s);
		if (isDragging())
		{
			resetDragging();
			item_bounce3(pi);
			if (cont->getId() >= 0x4000)
				senditem(s, cont);
		}
		return;
	}
	// - Trash container
	if( cont->type==ITYPE_TRASH)
	{
		pi->Delete();
		sysmsg("As you let go of the item it disappears.");
		return;
	}
	// - Spell Book
	if (cont->type==ITYPE_SPELLBOOK)
	{
		if (!pi->IsSpellScroll72())
		{
			sysmsg("You can only place spell scrolls in a spellbook!");
                        //! \todo the sendpacket stuff here
			Sndbounce5(s);
			if (isDragging())
			{
				resetDragging();
				item_bounce3(pi);
			}
			if (cont->getId() >= 0x4000)
				senditem(s, cont);
			return;
		}
		pack= pc->getBackpack();
		if( pack )
		{
			if ((cont->getContainer() != pc ) && (cont->getContainer()!=pack) && (!pc->CanSnoop()))
			{
				sysmsg("You cannot place spells in other peoples spellbooks.");
				item_bounce6(pi);
				return;
			}

			if( strncmp(pi->getCurrentName().c_str(), "#", 1) )
				pi->getName(temp2);
			else
				strcpy(temp2,pi->getCurrentName().c_str());

			NxwItemWrapper sii;
			sii.fillItemsInContainer( cont, false );
			for( sii.rewind(); !sii.isEmpty(); sii++ ) {

				pItem pi_ci=sii.getItem();

					if ( pi_ci )
					{
						if( strncmp(pi_ci->getCurrentName().c_str(), "#", 1) )

							pi_ci->getName(temp);
						else
							strcpy(temp,pi_ci->getCurrentName().c_str());

						if(!(strcmp(temp,temp2)) || !(strcmp(temp,"All-Spell Scroll")))
						{
							sysmsg("You already have that spell!");
							item_bounce6(pi);
							return;
						}
					}
				// Juliunus, to prevent ppl from wasting scrolls.
				if (pi->amount > 1)
				{
					sysmsg("You can't put more than one scroll at a time in your book.");
					item_bounce6(pi);
					return;
				}
			}
		}
		cont->AddItem( pi );
		sendSpellBook(cont);
		return;
	}

	if (cont->type == ITYPE_CONTAINER) {

		if ( contOwner )
		{
			if ( (contOwner->npcaitype==NPCAI_PLAYERVENDOR) && (contOwner->npc) && (contOwner->getOwnerSerial32()==pc->getSerial()) )
			{
				pc->fx1= DEREF_pItem(pi);
				pc->fx2=17;
				pc->sysmsg("Set a price for this item.");
			}
		}

		short xx=loc->x;
		short yy=loc->y;

		cont->AddItem(pi,xx,yy);

		pc->playSFX( itemsfx(pi->getId()) );
		statusWindow(pc, true);
	}
	// end of player run vendors

	else
		// - Unlocked item spawner or unlockable item spawner
		if (cont->type==ITYPE_UNLOCKED_CONTAINER || cont->type==ITYPE_NODECAY_ITEM_SPAWNER || cont->type==ITYPE_DECAYING_ITEM_SPAWNER)
		{
			cont->AddItem(pi, loc->x, loc->y); //Luxor
			pc->playSFX( itemsfx(pi->getId()) );

		}
		else  // - Pileable
			if (cont->pileable && pi->pileable)
			{
				if ( !cont->PileItem( pi ) )
				{
					item_bounce6(pi);
					return;
				}
			}
			else
			{
				if( !pi->getOldContainer() ) //current cont serial is invalid because is dragging
				{
					NxwSocketWrapper sw;
					sw.fillOnline( pi->getPosition() );
                                        //! \todo the sendpacket stuff here
					for( sw.rewind(); !sw.isEmpty(); sw++ )
						SendDeleteObjectPkt(sw.getSocket(), pi->getSerial() );
					mapRegions->remove(pi);
				}

				pi->setPosition( loc );
				pi->setContainer( cont->getContainer() );

				pi->Refresh();
			}


}


/*!
\brief drop dragged item on the ground or a character
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
*/

void cClient::dump_item(pItem pi, Location &loc, pItem cont) // Item is dropped on ground or a character
{
	if ( ! pi ) return;

	tile_st tile;

	pChar pc=currChar();
	if ( ! pc ) return;

	if ( isCharSerial(pi->getContainer()->getSerial()) && pi->getContainer() != pc ) {
		pChar pc_i = (pChar) pi->getContainer();
		if ( pc_i )
			pc_i->sysmsg("Warning, backpack disappearing bug located!");

		if (isDragging()) {
                        resetDragging();
                        updateStatusWindow(pi);
                }
		pi->setContainer( pi->getOldContainer() );
                pi->setPosition( pi->getOldPosition() );
                pi->layer = pi->oldlayer;
                pi->Refresh();
	}

	if (pi->magic == 2) { //Luxor -- not movable objects
		if (isDragging()) {
                        resetDragging();
                        updateStatusWindow(pi);
                }
		pi->setContainer( pi->getOldContainer() );
		pi->MoveTo( pi->getOldPosition() );
		pi->layer = pi->oldlayer;
		pi->Refresh();
		return;
	}

	pc->getBody()->calcWeight();
	statusWindow(pc, this);
	
	//Ripper...so order/chaos shields disappear when on ground.
	if( pi->getId()==0x1BC3 || pi->getId()==0x1BC4 )
	{
		pc->playSFX( 0x01FE);
		pc->staticFX(0x372A, 9, 6);
		pi->Delete();
		return;
	}


	//test UOP blocking Tauriel 1-12-99
	if (!pi->isInWorld())
	{
		item_bounce6(pi);
		return;
	}




	data::seekTile(pi->getId(), tile);
	if (!pc->IsGM() && ((pi->magic==2 || (tile.weight==255 && pi->magic!=1))&&!pc->canAllMove()) ||
		( (pi->magic==3 || pi->magic==4) && !(pi->getOwnerSerial32()==pc->getSerial())))
	{
		item_bounce6(pi);
		return;
	}

	if (loc->x != 0xffff)  // WAS buffer[s][5] != unsigned char '0xff'
        {
               if (pi->amxevents[EVENT_IDROPINLAND]!=NULL) {
	       	        g_bByPass = false;
        	        pi->amxevents[EVENT_IDROPINLAND]->Call( pi->getSerial(), pc->getSerial() );
			        if (g_bByPass) {
				        pi->Refresh();
				        return;
			        }
		        }
                NxwSocketWrapper sw;
	        sw.fillOnline( pi );
        	for( sw.rewind(); !sw.isEmpty(); sw++ )
        	{
                        //! \todo the sendpacket stuff here
        		SendDeleteObjectPkt( sw.getSocket(), pi->getSerial() );
        	}

        	pi->MoveTo(Loc);
                pi->setContainer(0);

                pItem p_boat = Boats->GetBoat(pi->getPosition());

        	if( p_boat )
        	{
        		pi->SetMultiSerial(p_boat->getSerial());
        	}


        	pi->Refresh();
	}
	else
	{
		if ( !droppedOnChar(pi, loc, cont) ) {
			//<Luxor>: Line of sight check
			//This part avoids the circle of transparency walls bug

			//-----
			if ( !lineOfSight( pc->getPosition(), loc ) ) {
		                ps->sysmsg("You cannot place an item there!");
                                //! \todo the sendpacket stuff here
        	        	Sndbounce5(s);
	                	if (isDragging()) {
	                        	resetDragging();
                        		updateStatusWindow(pi);
                		}
                		pi->setContainer( pi->getOldContainer() );
                		pi->setPosition( pi->getOldPosition() );
                		pi->layer = pi->oldlayer;
                		pi->Refresh();
                		return;
        		}
        		//</Luxor>

	        	//<Luxor> Items count check
	        	if (!pc->IsGM()) {
				NxwItemWrapper si;
				si.fillItemsAtXY( loc->x, loc->y );
				if (si.size() >= 2) { //Only 2 items permitted
					ps->sysmsg("There is not enough space there!");
                                        //! \todo the sendpacket stuff here
					Sndbounce5(s);
					if (isDragging()) {
						resetDragging();
						updateStatusWindow(pi);
					}
					if ( pc->getBackpack() ) {
						pc->getBackpack()->AddItem(pi);
					} else {
						pi->setContainer( pi->getOldContainer() );
						pi->setPosition( pi->getOldPosition() );
					}
					pi->layer = pi->oldlayer;
					pi->Refresh();
					return;
				}
			}
        		//</Luxor>
		}

		pc->getBody()->calcWeight();
		statusWindow(pc, true);
		pc->playSFX( itemsfx(pi->getId()) );

		//Boats !
		if (pc->getMultiSerial32() > 0) //How can they put an item in a multi if they aren't in one themselves Cut lag by not checking everytime something is put down
		{
			pItem multi = cSerializable::findItemBySerial( pc->getMultiSerial32() );
			if ( multi )
			{
				multi=findmulti( pi->getPosition() );
				if ( multi )
					//setserial(DEREF_pItem(pi),DEREF_pItem(multi),7);
					pi->SetMultiSerial(multi->getSerial());
			}
		}
		//End Boats
	}
}

/*!
\brief verifies if item has been dropped on a char and if so executes necessary code
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
\return bool
*/

bool cClient::droppedOnChar(pItem pi, Location &loc, pItem cont)
{
	VALIDATEPIR(pi, false);


	pChar pTC = cSerializable::findCharBySerial(cont->getSerial());	// the targeted character
	VALIDATEPCR(pTC, false);
	pChar pc_currchar = currChar();
	VALIDATEPCR(pc_currchar, false);
	Location charpos = pc_currchar->getPosition();

	if (!pTC) return true;

	if (pc_currchar->getSerial() != pTC->getSerial() /*DEREF_pChar(pTC)!=cc*/)
	{
		if (pTC->npc)
		{
			if(!pTC->HasHumanBody())
			{
				droppedOnPet( ps, pp, pi);
			}
			else	// Item dropped on a Human character
			{
				// Item dropped on a Guard (possible bounty quest)
				if( ( pTC->npc == 1 ) && ( pTC->npcaitype == NPCAI_TELEPORTGUARD ) )
				{
					droppedOnGuard(pi, loc, cont);
				}
				if ( pTC->npcaitype == NPCAI_BEGGAR )
				{
					droppedOnBeggar(pi, loc, cont);
				}

				//This crazy training stuff done by Anthracks (fred1117@tiac.net)
				if(pc_currchar->trainer != pTC->getSerial())

				{
					pTC->talk(this, "Thank thee kindly, but I have done nothing to warrant a gift.", false);
                                        //! \todo the sendpacket stuff here
					Sndbounce5(s);
					if (isDragging())
					{
						resetDragging();
						item_bounce5(pi);
					}
					return true;
				}
				else // The player is training from this NPC
				{
					droppedOnTrainer( pi, loc, cont);
				}
			}//if human or not
		}
		else // dropped on another player
		{
			// By Polygon: Avoid starting the trade if GM drops item on logged on char (crash fix)
			if ((pc_currchar->IsGM()) && !pTC->IsOnline())
			{
				// Drop the item in the players pack instead
				// Get the pack
				pItem pack = pTC->getBackpack();
				if (pack != NULL)	// Valid pack?
				{
					pack->AddItem(pi);	// Add it
					pTC->getBody()->calcWeight();
				}
				else	// No pack, give it back to the GM
				{
					pack = pc_currchar->getBackpack();
					if (pack != NULL)	// Valid pack?
					{
						pack->AddItem(pi);	// Add it
						pc_currchar->getBody()->calcWeight();
					}
					else	// Even GM has no pack?
					{
						// Drop it to it's feet
						pi->MoveTo( charpos );
						pi->Refresh();
					}
				}
			}
			else
			{
                                //<Luxor>: secure trade
                //! \todo the sendpacket stuff here
                 pItem tradeCont = tradestart(pc_currchar, pTC);
                 if ( tradeCont ) {
                    tradeCont->AddItem( pi, 30, 30 );
                 } else {
                    Sndbounce5(s);
                    if (isDragging()) {
                 		resetDragging();
                 		updateStatusWindow(pi);
                   	}
                 }
                 //</Luxor>
		        }
	        }
	}
	else // dumping stuff to his own backpack !
	{
		droppedOnSelf( pi, loc, cont);
	}
	return true;

}

/*!
\brief item has been dropped on a pet and verifies if correct item given
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
\return bool
*/


bool cClient::droppedOnPet(pItem pi, Location &loc, pItem cont)
{
	pChar pet = cSerializable::findCharBySerial(cont->getSerial());
	VALIDATEPCR(pet, false);
	pChar pc = currChar();
	VALIDATEPCR(pc, false);

	if((pet->hunger<6) && (pi->type==ITYPE_FOOD))//AntiChrist new hunger code for npcs
	{
		pc->playSFX( 0x3A+(rand()%3) );	//0x3A - 0x3C three different sounds

		if(pi->poisoned)
		{
			pet->applyPoison(PoisonType(pi->poisoned));
		}

		std::string itmname;
		if( pi->getCurrentName() == "#" )
		{
			char temp2[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
			pi->getName(temp2);
			itmname = temp2;
		}
		else itmname = pi->getCurrentName();

		pet->emotecolor = 0x0026;
		pet->emoteall("* You see %s eating %s *", true, pet->getCurrentName().c_str(), itmname.c_str() );
		pet->hunger++;
	} else
	{
		ps->sysmsg("It doesn't appear to want the item");
                //! \todo the sendpacket stuff here
		Sndbounce5(s);
		if (isDragging())
		{
			resetDragging();
			item_bounce5(pi);

		}
	}
	return true;
}

/*!
\brief item has been dropped on a guard and verifies if correct item given
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
\return bool
*/


bool cClient::droppedOnGuard(pItem pi, Location &loc, pItem cont)
{
	VALIDATEPIR(pi, false);
	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	pChar pc = currChar();
	VALIDATEPCR(pc,false);
	pChar pc_t=cSerializable::findCharBySerial(cont->getSerial()); //the guard
	VALIDATEPCR(pc_t,false);
	// Search for the key word "the head of"
        //! \todo change check for text to check for id
	if( strstr( pi->getCurrentName().c_str(), "the head of" ) ) //!!! Wrong! it must check the ItemID, not the name :(
//	if( pi->getId() == 0x1DA0 ) //!!! RIGHT! uhm... siam sicuri che questa sia la versione corretta ?! -.^" [targeting.cpp:148]
	{
		// This is a head of someone, see if the owner has a bounty on them
		pChar own=cSerializable::findCharBySerial(pi->getOwnerSerial32());
		VALIDATEPCR(own,false);

		if( own->questBountyReward > 0 )
		{
			// Give the person the bounty assuming that they are not the
			// same person as the reward is for
			if( pc->getSerial() != own->getSerial() )
			{
				// give them the gold for bringing the villan to justice
				pc->addGold(own->questBountyReward);
				pc->playSFX( goldsfx( own->questBountyReward ) );

				// Now thank them for their hard work
				sprintf( temp, "Excellent work! You have brought us the head of %s. Here is your reward of %d gold coins.", own->getCurrentName().c_str(), own->questBountyReward );
				pc_t->talk( this, temp, 0);

				// Delete the Bounty from the bulletin board
				BountyDelete(own );

				// xan : increment fame & karma :)
				pc->IncreaseKarma( nSettings::Actions::getBountyKarmaGain() );
				pc->modifyFame( nSettings::Actions::getBountyFameGain() );
			}
			else
				pc_t->talk(this, "You can not claim that prize scoundrel. You are lucky I don't strike you down where you stand!",0);

			// Delete the item
			pi->Delete();
		}
	}
	return true;
}

/*!
\brief item has been dropped on a beggar and verifies if correct item given
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
\return bool
*/

bool cClient::droppedOnBeggar(pItem pi, Location &loc, pItem cont)
{
	pChar pc_t=cSerializable::findCharBySerial(cont->getSerial()); //beggar
	if ( ! pi || ! currChar() || ! pc_t )
		return false;

	if(pi->getId()!=ITEMID_GOLD)
	{
		pc_t->talk(this, "Sorry %s i can only use gold", false, pc->getCurrentName().c_str());
                //! \todo the sendpacket stuff here
		Sndbounce5(s);
		if (isDragging())
		{
		        resetDragging();
			item_bounce5(pi);
			return true;
		}
	}
	else
	{
		pc_t->talk(this, "Thank you %s for the %i gold!", false, pc->getCurrentName().c_str(), pi->amount);
		if(pi->amount<=100)
		{
			pc->IncreaseKarma(10);
			ps->sysmsg("You have gain a little karma!");
		}
		else if(pi->amount>100)
		{
			pc->IncreaseKarma(50);
			ps->sysmsg("You have gain some karma!");
		}
		pi->Delete();
		return true;
	}
	return true;
}

/*!
\brief item has been dropped on a trainer and verifies if correct item given
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
\return bool
*/


bool cClient::droppedOnTrainer(pItem pi, Location &loc, pItem cont)
{
	VALIDATEPIR(pi, false);

	pChar pc = currChar();
	VALIDATEPCR(pc,false);
	pChar pc_t = cSerializable::findCharBySerial(cont->getSerial());
	VALIDATEPCR(pc_t,false);

	if( pi->getId() == ITEMID_GOLD )
	{ // They gave the NPC gold
		uint8_t sk=pc_t->trainingplayerin;
		pc_t->talk(this, "I thank thee for thy payment. That should give thee a good start on thy way. Farewell!", false);

		int sum = pc->getSkillSum();
		int delta = pc_t->getTeachingDelta(pc, sk, sum);

		if(pi->amount>delta) // Paid too much
		{
			pi->amount-=delta;
                        //! \todo the sendpacket stuff here
			Sndbounce5(s);
			if (isDragging())
			{
			        resetDragging();
				item_bounce5(pi);
			}
		}
		else
		{
			if(pi->amount < delta)		// Gave less gold
				delta = pi->amount;		// so adjust skillgain
			pi->Delete();
		}
		pc->baseskill[sk]+=delta;
		Skills::updateSkillLevel(pc, sk);
		pc->updateSkill(sk);

		pc->trainer=-1;
		pc_t->trainingplayerin=0xFF;
		pc->playSFX( itemsfx(pi->getId()) );
	}
	else // Did not give gold
	{
		pc_t->talk(this, "I am sorry, but I can only accept gold.", false);
                //! \todo the sendpacket stuff here
		Sndbounce5(s);
		if (isDragging())
		{
			resetDragging();
			item_bounce5(pi);
		}
	}//if items[i]=gold
	return true;
}

/*!
\brief item has been dropped on self and verifies if it was a correct item
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
\return bool
*/


bool cClient::droppedOnSelf(pItem pi, Location &loc, pItem cont)
{

	VALIDATEPIR(pi, false);
	pChar pc = currChar();
	VALIDATEPCR(pc, false);

	Location charpos = pc->getPosition();

	if (pi->getId() >= 0x4000 ) // crashfix , prevents putting multi-objects ni your backback
	{
		sysmsg("Hey, putting houses in your pack crashes your back and client !");
		pi->MoveTo( charpos );
		pi->Refresh();//AntiChrist
		return true;
	}

//	if (pi->glow>0) // glowing items
//	{
//		pc->addHalo(pi);
//		pc->glowHalo(pi);
//	}

	pItem pack = pc->getBackpack();
	if (pack==NULL) // if player has no pack, put it at its feet
	{
		pi->MoveTo( charpos );
		pi->Refresh();
	}
	else
	{
		pack->AddItem(pi); // player has a pack, put it in there

		pc->getBody()->calcWeight();
		statusWindow(pc,true);
		pc->playSFX( itemsfx(pi->getId()) );
	}
	return true;
}


/*!
\brief holds some statements that were COPIED some 50 times
\param pi item to be bounced back (already in dragging mode)
*/

void cClient::item_bounce3(const pItem pi)
{
	VALIDATEPI( pi );
	pi->setContainer( pi->getOldContainer() );
	pi->setPosition( pi->getOldPosition() );
	pi->layer=pi->oldlayer;

	pChar pc = (pChar) pi->getOldContainer();
	if(pc)
		return ;

	if ( pi->layer > 0 )
	{
		// Xanathar -- add BONUS STATS given by equipped special items
		pc->setStrength( pc->getStrength() + pi->st2, true );
		//pc->st += pi->st2;
		pc->setDexterity(pc->getDexterity() + pi->dx2, true);
		pc->setIntelligence(pc->getIntelligence() + pi->in2, true);
		// Xanathar -- for poisoned items
		if (pi->poisoned)
		{
			pc->poison += pi->poisoned;
			if ( pc->poison < 0)
				pc->poison = 0;
		}
	}
}

/*!
\brief Wears item dragged on paperdoll
\param pck char to "dressup" :)
\param pi item to be put on pc
*/
void cClient::wear_item(pChar pck, pItem pi) // Item is dropped on paperdoll
{

	pChar pc = currChar();
	if ( ! pc ) return;
	if( pck->dead )  //Exploit fix: Dead ppl can't equip anything.
		return;

	bool resetDragging = false;

	if( (pi->getId()>>8) >= 0x40)  // LB, client crashfix if multi-objects are moved to PD
		resetDragging = true;

	tile_st tile;
	int serial/*, letsbounce=0*/; // AntiChrist (5) - new ITEMHAND system

	data::seekTile(pi->getId(), tile);

	if( ( clientDimension == 3 ) &&  (tile.quality == 0) )
	{
		sysmsg("You can't wear that");
		resetDragging = true;
	}
	else {
		pItem outmost = pi->getOutMostCont();
		pChar vendor = (pChar) outmost->getContainer();
		if( vendor && ( vendor->getOwnerSerial32() != pc->getSerial() ) )
		{
			resetDragging = true;
		}
	}

	if( resetDragging ) {
                //! \todo the sendpacket stuff here
		Sndbounce5();
		if (isDragging())
		{
			resetDragging();
			item_bounce4(pi);
			updateStatusWindow(pi);
		}
		return;
	}

	if ( pck == pc || pc->IsGM() )
	{

		if ( !pc->IsGM() && pi->st > pck->getStrength() && !pi->isNewbie() ) // now you can equip anything if it's newbie
		{
			sysmsg("You are not strong enough to use that.");
			resetDragging = true;
		}
		else if ( !pc->IsGM() && !pi->checkItemUsability(pc, ITEM_USE_WEAR) )
		{
			resetDragging = true;
		}
		else if ( (pc->getId() == BODY_MALE) && ( pi->getId()==0x1c00 || pi->getId()==0x1c02 || pi->getId()==0x1c04 || pi->getId()==0x1c06 || pi->getId()==0x1c08 || pi->getId()==0x1c0a || pi->getId()==0x1c0c ) ) // Ripper...so males cant wear female armor
		{
			sysmsg("You cant wear female armor!");
			resetDragging = true;
		}
		else if ((((pi->magic==2)||((tile.weight==255)&&(pi->magic!=1))) && !pc->canAllMove()) ||
				( (pi->magic==3|| pi->magic==4) && !(pi->getOwnerSerial32() == pc->getSerial)) )
		{
			resetDragging = true;
		}

		if( resetDragging ) {
                        //! \todo the sendpacket stuff here
			Sndbounce5(s);
			if (isDragging())
			{
				resetDragging();
				item_bounce4(pi);
				updateStatusWindow(pi);
			}
			return;
		}



		// - AntiChrist (4) - checks for new ITEMHAND system
		// - now you can't equip 2 hnd weapons with 1hnd weapons nor shields!!
		serial= pck->getSerial(); //xan -> k not cc :)

		pItem pj = NULL;
 		pChar pc_currchar= pck;
// 		pItem pack= pc_currchar->getBackpack();
                //<Luxor>

		pItem pW = pc_currchar->getWeapon();
		if (tile.quality == 1 || tile.quality == 2)
		{ //weapons layers
			if ( (pi->layer == LAYER_2HANDWEAPON && pc_currchar->getShield()) )
			{
				sysmsg("You cannot wear two weapons.");
                                //! \todo the sendpacket stuff here
				Sndbounce5(s);
				if (isDragging())
				{
        			        resetDragging();
					updateStatusWindow(pi);
	        	}
				pi->setContainer( pi->getOldContainer() );
				pi->setPosition( pi->getOldPosition() );
				pi->layer = pi->oldlayer;
				pi->Refresh();
				return;
			}
			if ( pW )
			{
				if (pi->itmhand != 3 && pi->lodamage != 0 && pi->itmhand == pW->itmhand)
				{
					sysmsg("You cannot wear two weapons.");
                                        //! \todo the sendpacket stuff here
					Sndbounce5(s);
					if (isDragging())
					{
						resetDragging();
						updateStatusWindow(pi);
					}
					pi->setContainer( pi->getOldContainer() );
					pi->setPosition( pi->getOldPosition() );
					pi->layer = pi->oldlayer;
					pi->Refresh();
					return;
				}
			}
		}
		//</Luxor>

		if ( ServerScp::g_nUnequipOnReequip )
		{
			pItem drop[2]= {NULL, NULL};	                // list of items to drop
									// there no reason for it to be larger
			int curindex= 0;

			NxwItemWrapper si;
			si.fillItemWeared( pc_currchar, false, true, true );
			for( si.rewind(); !si.isEmpty(); si++ )
			{
				// we CANNOT directly bounce the item, or the containersearch() function will not work
				// so we store the item ID in letsbounce, and at the end we bounce the item

				pj=si.getItem();
				if( ! pj )
					continue;

				if ((tile.quality == 1) || (tile.quality == 2))// weapons
				{
					if (pi->itmhand == 2) // two handed weapons or shield
					{
						if (pj->itmhand == 2)
							drop[curindex++]= pj;

						if ( (pj->itmhand == 1) || (pj->itmhand == 3) )
							drop[curindex++]= pj;
					}

					if (pi->itmhand == 3)
					{
						if ((pj->itmhand == 2) || pj->itmhand == 3)
							drop[curindex++]= pj;
					}

					if ((pi->itmhand == 1) && ((pj->itmhand == 2) || (pj->itmhand == 1)))
						drop[curindex++]= pj;
				}
				else	// not a weapon
				{
					if (pj->layer == tile.quality)
						drop[curindex++]= pj;
				}
			}

			if (ServerScp::g_nUnequipOnReequip)
			{
				if (drop[0] != NULL)	// there is at least one item to drop
				{
					for (int i= 0; i< 2; i++)
					{
						if (drop[i] != NULL)
						{
							if( drop[i] ) pc_currchar->unEquip( drop[i], 1);
						}
					}
				}
				pc->playSFX( itemsfx(pi->getId()) );
				pc_currchar->equip(pi, 1);
			}
			else
			{
				if (drop[0] == NULL)
				{
					pc->playSFX( itemsfx(pi->getId()) );
					pc_currchar->equip(pi, 1);
				}
			}
		}

		if (!(pc->IsGM())) //Ripper..players cant equip items on other players or npc`s paperdolls.
		{
			if ((pck->getSerial() != pc->getSerial())/*&&(chars[s].npc!=k)*/) //-> really don't understand this! :|, xan
			{
				sysmsg("You can't put items on other people!");
				item_bounce6(pi);
				return;
			}
		}

		NxwSocketWrapper sws;
		sws.fillOnline( pi );
		for( sws.rewind(); !sws.isEmpty(); sws++ )
                        //! \todo the sendpacket stuff here
			SendDeleteObjectPkt( sws.getSocket(), pi->getSerial() );

//! \todo verify if layer behaves as Flameeyes told me :D (but it seems to me it does NOT! :P)

//		pi->layer=buffer[5];  //Chronodt: layer from packet should be ignored in favour of normal layer positioning for item
		pi->setContSerial(pck);

		if (g_nShowLayers) InfoOut("Item equipped on layer %i.\n",pi->layer);

		wearIt(pi);

		NxwSocketWrapper sw;
		sw.fillOnline( pck, false );
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			cClient j = sw.getClient();
			if( j!=NULL )
				j->wornitems(pck );
		}

		pc->playSFX( itemsfx(pi->getId()) );
		pc->getBody()->calcWeight();
		statusWindow(pc, true);

//		if (pi->glow>0)
//		{
//			pc->removeHalo(pi); // if gm equips on differnt player it needs to be deleted out of the hashteble
//			pck->addHalo(pi);
//			pck->glowHalo(pi);
//		}

		if ( pck->equip(pi, 1) == 2)	// bypass called
		{
			pItem pack = pck->getBackpack();
			pc->playSFX( itemsfx(pi->getId()) );
			pi->layer= 0;
			pi->setContainer( pack );
			showItemInContainer(pi);
			return;
		}

	}
}


/*!
\brief holds some statements that were COPIED some 50 times
\param pi item to be bounced back (already in dragging mode)
*/

void cClient::item_bounce4(const pItem pi)
{
	if ( ! pi ) return;
	item_bounce3(pi);
	if( (pi->getId() >>8) < 0x40)
		senditem( pi );
}

/*!
\brief holds some statements that were COPIED some 50 times
\param pi item to be bounced back (already in dragging mode)
*/

void cClient::item_bounce5( const pItem pi)
{
	if ( ! pi ) return;
	item_bounce3(pi);
	senditem(pi);
}

/*!
\brief holds some statements that were COPIED some 50 times
\param pi item to be bounced back (already in dragging mode)
*/

void cClient::item_bounce6(const pItem pi)
{
	if ( ! pi ) return;
	Sndbounce5();
	if ( isDragging() )
	{
		resetDragging();
		item_bounce4( pi );
	}
}

/*------------------------------------------------------------------------------
                             TRADING METHODS
------------------------------------------------------------------------------*/

/*!
\brief concludes buying of items
\author Unknown, updated to pyuo Chronodt (24/2/04)
\param npc vendor whose goods player is buying
\param allitemsbought list of items selected from player (layer, pItem and amount for each item)
*/
void cClient::buyaction(pNpc npc, std::list< boughtitem > &allitemsbought)
{

	int i, j;

	int playergoldtotal;

	int tmpvalue=0; // Fixed for adv trade system -- Magius(CHE) §

	pChar pc = currChar();
	if ( ! pc ) return;

	pItem pack = pc->getBackpack();
	VALIDATEPI(pack);

	int itemtotal=allitemsbought.size();
	if (itemtotal>256)
		return; //LB

	int clear=0;
	int goldtotal=0;
	int soldout=0;

        std::list<boughtitem>::iterator iter( allitemsbought.begin()), end( allitemsbought.end() );
	for (; iter!=end; iter++)
	{
		iter.item->rank=10;     //Just to be on the safe side... :)
		// Fixed for adv trade system -- Magius(CHE) §
		tmpvalue = iter.item->value;
		tmpvalue = iter.item->calcValue(tmpvalue);
		if (SrvParms->trade_system==1)
			tmpvalue=calcGoodValue(this,iter.item,tmpvalue,0);
		goldtotal+=iter.amount*tmpvalue;
		// End Fix for adv trade system -- Magius(CHE) §
                if (iter->item->amount < iter->amount) soldout=1;
	}

	bool useBank;
	useBank = (goldtotal >= SrvParms->CheckBank );

	if( useBank )
		playergoldtotal = pc->countBankGold();//GetBankCount(pc_currchar, 0x0EED );
	else
		playergoldtotal = pc->CountGold();

	if ((playergoldtotal<goldtotal)&&(!pc->IsGM()))
	{
		npc->talkAll("Alas, thou dost not possess sufficient gold for this purchase!", false);
	}
	else {
                if (soldout)
		{
			npc->talk(this, "Alas, I no longer have all those goods in stock. Let me know if there is something else thou wouldst buy.", false);
			clear=1;
		}
		else
		{
			if (pc->IsGM())
			{
				npc->talkAll("Here you are, %s. Someone as special as thee will receive my wares for free of course.", false, pc->getCurrentName().c_str());
			} else {
				if(useBank)
				{
					npc->talkAll("Here you are, %s. %d gold coin%s will be deducted from your bank account.  I thank thee for thy business.", false, pc->getCurrentName().c_str(), goldtotal, (goldtotal==1) ? "" : "s");
				} else {
					npc->talkAll("Here you are, %s.  That will be %d gold coin%s.  I thank thee for thy business.", false, pc->getCurrentName().c_str(), goldtotal, (goldtotal==1) ? "" : "s");
				}
			    pc->playSFX( goldsfx(goldtotal) );
			}
			npc->playAction(0x20);	// bow (Duke, 17/03/2001)

			clear=1;
			if( !(pc->IsGM() ) )
			{
				if( useBank )
				{
					pItem bank= pc->GetBankBox();
					bank->DeleteAmount(goldtotal, ITEMID_GOLD, 0);
				}
				else
				{
					pack->DeleteAmount( goldtotal, ITEMID_GOLD);
				}
			}

			for (iter = allitemsbought.begin(); iter!=end; iter++) {
				if (iter->item->amount > iter->amount)
				{
					if (iter->item->pileable)
					{
						item::CreateFromScript(  iter->item->getScriptID(), pack, iter->amount );
					}
					else
					{
						for (j=0;j<iter->amount;j++)
							item::CreateFromScript( iter->item->getScriptID(), pack );
					}
					iter->item->amount-=iter->amount;
					iter->item->restock+=iter->amount;
				}
				else
				{
					switch(iter->layer)
					{
						case LAYER_TRADE_RESTOCK:
							if (iter->item->pileable)
							{

								item::CreateFromScript( iter->item->getScriptID(), pack, iter->amount );

							}
							else
							{
								for (j=0;j<iter->amount;j++)
									item::CreateFromScript( iter->item->getScriptID(), pack );

							}
							iter->item->amount-=iter->amount;
							iter->item->restock+=iter->amount;
							break;
						case LAYER_TRADE_NORESTOCK:
							if (iter->item->pileable)
								iter->item->setContainer( pack );
							else
							{
								for (j=0;j<iter->amount;j++)
									item::CreateFromScript( iter->item->getScriptID(), pack );

								iter->item->setContainer( pack );
								iter->item->amount=1;
							}
							iter->item->Refresh();
							break;
						default:
							ErrOut("Switch fallout. trade.cpp, buyaction()\n"); //Morrolan
					}
				}
			}
		}
	}

	if (clear)
	{
                cPacketSendClearBuyWindow pk(npc);
	        sendPacket(&pk);
	}
	pc->getBody()->calcWeight();
	statusWindow(pc,true);  //!< \todo check second argument
}


void cClient::sellaction(pNpc npc, std::list< boughtitem > &allitemssold)
{
	pChar pc=currChar();
	VALIDATEPC(pc);

	pItem np_a=NULL, np_b=NULL, np_c=NULL;
	int i, amt, value=0, totgold=0;

	VALIDATEPC(npc);

	NxwItemWrapper si;
	si.fillItemWeared( npc, true, true, false );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi=si.getItem();
		if(ISVALIDPI(pi))
                {
			if (pi->layer == LAYER_TRADE_RESTOCK) np_a=pi;	// Buy Restock container
			if (pi->layer == LAYER_TRADE_NORESTOCK) np_b=pi;	// Buy no restock container
			if (pi->layer == LAYER_TRADE_BOUGHT) np_c=pi;	// Sell container
		}
        }

	// Pre Calculate Total Amount of selling items to STOPS if the items if greater than SELLMAXITEM - Magius(CHE)

	pItem join=NULL;
	uint32_t maxsell=0;
	int nitems=allitemssold.size();
	if (nitems>256) return;
        std::list< boughtitem >::iterator it = allitemssold.begin();
	for (;it!=allitemssold.end();it++) maxsell+=it->amount;

	if (maxsell>SrvParms->sellmaxitem)
	{
		npc->talkAll("Sorry %s but i can buy only %i items at time!", false, currChar()->getCurrentName().c_str(), SrvParms->sellmaxitem);
		return;
	}

	for (it = allitemssold.begin();it!=allitemssold.end();it++)
	{
		pItem pSell=it->item;	// the item to sell
		if (!ISVALIDPI(pSell)) continue;
		amt=it->amount;

		// player may have taken items out of his bp while the sell menu was up ;-)
		if (pSell->amount<amt)
		{
			npc->talkAll("Cheating scum! Leave now, before I call the guards!",0);
			return;
                }

		// Search the buy restock Container
		if( ISVALIDPI(np_a) )
                {
			NxwItemWrapper si2;
			si2.fillItemsInContainer( np_a, false );
			for( si2.rewind(); !si2.isEmpty(); si2++ )
			{
				P_ITEM pi=si2.getItem();
				if( ISVALIDPI(pi) && items_match(pi,pSell)) join=pi;
			}
		}

		// Search the sell Container to determine the price
		if( ISVALIDPI(np_c) )
                {
			NxwItemWrapper si2;
			si2.fillItemsInContainer( np_c, false );
			for( si2.rewind(); !si2.isEmpty(); si2++ )
			{
				pItem pi=si2.getItem();
				if( ISVALIDPI(pi) && items_match(pi,pSell))
				{
					value = pi->value;
					value = pSell->calcValue(value);
					if (SrvParms->trade_system==1)
						value=calcGoodValue(this,pSell,value,1); // Fixed for adv trade --- by Magius(CHE) §
					break;	// let's take the first match
				}
                        }
                }
		totgold+=(amt*value);	// add to the bill

		if (join!=NULL)	// The item goes to the container with restockable items
		{

			join->amount+=amt;
			join->restock-=amt;
			pSell->ReduceAmount(amt);
		}
		else
		{
			if(ISVALIDPI(np_b))
                        {
				NxwSocketWrapper sw;
				sw.fillOnline( pSell );

				for( sw.rewind(); !sw.isEmpty(); sw++ )
				{
					SendDeleteObjectPkt( sw.getSocket(), pSell->getSerial() );
				}

				pSell->setContainer( np_b );
				if (pSell->amount!=amt)
					Commands::DupeItem(s, DEREF_P_ITEM(pSell), pSell->amount-amt);
                        }
                }
        }
	currChar()->addGold(totgold);
	playSFX( goldsfx(totgold) );



        

	uint8_t clearmsg[8] = { 0x3B, 0x00, };
	ShortToCharPtr(0x08, clearmsg +1); 				// Packet len
	LongToCharPtr( LongFromCharPtr(buffer[s] +3), clearmsg +3);	// vendorID
	clearmsg[7]=0x00;						// Flag:  0 => no more items  0x02 items following ...
	Xsend(s, clearmsg, 8);
//AoS/	Network->FlushBuffer(s);
}


void sendtradestatus(pContainer cont1, pContainer cont2)  //takes clients from containers' owners
{
	VALIDATEPI(cont1);
	VALIDATEPI(cont2);

	pChar p1, p2;

	p1 = cSerializable::findCharBySerial(cont1->getContSerial());
	VALIDATEPC(p1);
	p2 = cSerializable::findCharBySerial(cont2->getContSerial());
	VALIDATEPC(p2);

	cPacketSendSecureTradingStatus pk1(0x02, cont1->getSerial(), (uint32_t) (cont1->morez%256), (uint32_t) (cont2->morez%256));
      	cPacketSendSecureTradingStatus pk2(0x02, cont2->getSerial(), (uint32_t) (cont2->morez%256), (uint32_t) (cont1->morez%256));
	p1->getClient()->sendPacket(&pk1);
	p2->getClient()->sendPacket(&pk2);
}

void dotrade(pContainer cont1, pContainer cont2)
{
        VALIDATEPI(cont1);
        VALIDATEPI(cont2);
        pPC pc1 = cSerializable::findCharBySerial(cont1->getContSerial());
        pPC pc2 = cSerializable::findCharBySerial(cont2->getContSerial());
        VALIDATEPC(pc1);
        VALIDATEPC(pc2);
        pContainer bp1 = pc1->getBackpack();
        pContainer bp2 = pc2->getBackpack();
        VALIDATEPI(bp1);
        VALIDATEPI(bp2);
        if (pc1->getClient() == NULL || pc2->getClient() == NULL) return;

        if (cont1->morez == 0 || cont2->morez == 0) {
                //If the trade is not accepted, then give items back to original owners
                pPC pc_dummy = NULL;
                pc_dummy = pc1;
                pc1 = pc2;
                pc2 = pc_dummy;
        }

        //Player1 items go to player2

	NxwItemWrapper si;
	si.fillItemsInContainer( cont1, false );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi = si.getItem();
		if( pi)
		{

			if (pi->amxevents[EVENT_IONTRANSFER]!=NULL) {
				g_bByPass = false;
				pi->amxevents[EVENT_IONTRANSFER]->Call(pi->getSerial(), pc1->getSerial(), pc2->getSerial());
				if (g_bByPass==true) continue; //skip item, I hope
			}
			pi->setContainer( bp2 );
			pi->setPosition( 50+(rand()%80), 50+(rand()%80), 9);
			pc2->getClient()->showItemInContainer(pi);
			pi->Refresh();
		}
	}


	si.clear();
	si.fillItemsInContainer( cont2, false );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi = si.getItem();
		if( pi)
		{

			if (pi->amxevents[EVENT_IONTRANSFER]!=NULL)
                        {
        			g_bByPass = false;
        			pi->amxevents[EVENT_IONTRANSFER]->Call(pi->getSerial(), pc2->getSerial(), pc1->getSerial());
        			if (g_bByPass==true) continue; //skip item, I hope
			}

			pi->setContainer( bp1 );
			pi->setPosition( 50+(rand()%80), 50+(rand()%80), 9);
			pc1->getClient()->showItemInContainer(pi);
			pi->Refresh();
		}
	}
}

void endtrade(uint32_t serial)
{
	pItem c1=cSerializable::findItemBySerial(serial);
	VALIDATEPI(c1);
	pItem c2=cSerializable::findItemBySerial(calcserial(c1->moreb1, c1->moreb2, c1->moreb3, c1->moreb4));
	VALIDATEPI(c2);

	pChar pc1=cSerializable::findCharBySerial(c1->getContSerial());
	VALIDATEPC(pc1);

	pChar pc2=cSerializable::findCharBySerial(c2->getContSerial());
	VALIDATEPC(pc2);


	pItem bp1= pc1->getBackpack();
	VALIDATEPI(bp1);
	pItem bp2= pc2->getBackpack();
	VALIDATEPI(bp2);

	pClient c1 = pc1->getClient();
	pClient c2 = pc2->getClient();

	if (c1 != NULL)	// player may have been disconnected (Duke)
        {
        	cPacketSendSecureTradingStatus pk1(0x01, cont1->getSerial(), 0, 0);
		c1->sendPacket(&pk1);
	}
	if (c2 != NULL)	// player may have been disconnected (Duke)
        {
              	cPacketSendSecureTradingStatus pk2(0x01, cont2->getSerial(), 0, 0);
              	c2->sendPacket(&pk2);
        }

	NxwItemWrapper si;
	si.fillItemsInContainer( c1, false );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pj=si.getItem(); //</Luxor>
		if (pj)
		{
			bp1->AddItem(pj);

			if (s1!=INVALID)
				pj->Refresh();
		}
	}

	NxwItemWrapper si2;
	si2.fillItemsInContainer( c2, false );
	for( si2.rewind(); !si2.isEmpty(); si2++ )
	{

		pItem pj=si2.getItem();
		if (pj)
		{
			bp2->AddItem(pj);

			if (s2!=INVALID)
				pj->Refresh();
		}
	}

	c1->Delete();
	c2->Delete();
}


/*------------------------------------------------------------------------------
                            AUDIO & MUSIC METHODS
------------------------------------------------------------------------------*/


/*!
\brief Play a random midi file adapt to the current status
\author Flameeyes
\todo Modify it after get working XML-Support
*/
void cClient::playMidi()
{

	pPC pc = currChar();
	if ( ! pc ) return;
	cScpIterator* iter = NULL;

    	char script1[1024];
    	char script2[1024];

	char sect[512];

	if (pc->inWarMode())
		strcpy(sect, "MIDILIST COMBAT");
	else
		sprintf(sect, "MIDILIST %i", region[pc->region].midilist);

	iter = Scripts::Regions->getNewIterator(sect);
	if (iter==NULL) return;

	char midiarray[50];
	int i=0;
	int loopexit=0;
	do
	{
		iter->parseLine(script1, script2);
		if ((script1[0]!='}')&&(script1[0]!='{'))
		{
			if (!(strcmp("MIDI",script1)))
			{
				midiarray[i]=str2num(script2);
				i++;
			}
		}
	}
	while ((script1[0]!='}') && (++loopexit < MAXLOOPS) );

	safedelete(iter);

	if (i!=0)
	{
		i=rand()%(i);
		sendMidi(0, midiarray[i]);
	}
}


/*!
\brief Play a sound effect
\author Flameeyes
\todo Fix the set support after get working new sets
*/
void cClient::playSFX(uint16_t sound, bool onlyMe)
{
	cPacketSendSoundFX pk(sound, pc->getPosition());

	if(onlyMe) {
		client->send(&pk);
		return;
	}

	//!\todo Missing way to find out
/*
	NxwSocketWrapper sw;
	sw.fillOnline( pc, false );

	for( sw.rewind(); !sw.isEmpty(); sw++ ) {
		NXWCLIENT ps=sw.getClient();
		if(ps!=NULL)
			ps->send(&pk);
	}
*/
}

/*------------------------------------------------------------------------------
                            TALK & MESSAGES METHODS
------------------------------------------------------------------------------*/


void cClient::talking(cSpeech &speech) // PC speech
{
	pChar pc = currChar();
	if ( ! pc ) return;


        //!\todo revise from here


	uint32_t i, j;
	int match;
	char sect[512];

	char name[30] = {0,};	// it **IS** important to 0 out the remaining gaps
	strcpy(name, pc->getCurrentName().c_str());

	// len+font+color+type = same postion for non unicode and unicode speech packets
	// but 8 ... x DIFFER a lot for unicode and non unicode packets !!!

	if ( buffer[socket][3] == '\x09' && pc->CanBroadcast() )
	{
		broadcast(socket);
		return;
	}

	//
	// Pass speech to small
	//
	g_bByPass = false;

	checkAmxSpeech( pc->getSerial(), speech);
	//
	// Allow for bypass set in checkAmxSpeech.This way certain speech like passwords will not be echoed to the screen
	//
	if (g_bByPass)
		return;

	//<Luxor>
	
	if ( pc->amxevents[EVENT_CHR_ONSPEECH] ) {
		g_bByPass = false;
		strcpy( script2, speech.c_str() );
		pc->amxevents[EVENT_CHR_ONSPEECH]->Call( pc->getSerial() );
		if( g_bByPass == true )
			return;
	}
	/*
        if ( pc->getAmxEvent( EVENT_CHR_ONSPEECH ) != NULL ) {
                strcpy( script2, speech.c_str() );
                pc->runAmxEvent( EVENT_CHR_ONSPEECH, pc->getSerial() );

                if( g_bByPass == true )
                        return;
        }
	*/

	//</Luxor>

	//
	// Process speech which should not be send/echoed to others
	//
	if( magic::checkMagicalSpeech( pc, speech) ) //Luxor
		return;
	if( pricePlayerVendorItem( pc, socket, speech ) )
		return;

	if( describePlayerVendorItem( pc, socket, speech ) )
		return;

	if( renameRune( pc, socket, speech ) )
		return;

	if( renameSelf( pc, socket, speech ) )
		return;

	if( renameKey( pc, socket, speech ) )
		return;

	if ( pc->pagegm == 1 ) // GM Page
	{
		new cGMPage(pc, speech, true);
		return;
	}
	
	if ( pc->pagegm == 2 ) // Counselor page
	{
		new cGMPage(pc, speech, false);
	}
	
	if ( pc->squelched )
	{
		pc->sysmsg("You have been squelched.");
		return;
	}

	if ( speech[0] == SrvParms->commandPrefix )
	{
		Commands::Command(socket, const_cast<char*>(speech.c_str()) );
		return;
	}

	/*
	Sparhawk	Entry and exit point for hidden GM to GM speech
	if ( pc->IsGM() && pc->hidden )
	{
		talkingGM2GM( s );
		return;
	}
	*/
	//
	// Unhide when talking
	//
	if (!pc->IsHiddenBySpell())
		pc->unHide();
	//
	// Collect all pcs in visual range
	//
	

	//
	// Echo speech to self and pcs in visual range
	//
	cPacketUnicodeSpeech talk;
	talk.obj=pc->getSerial();
	talk.model=pc->getId();
	talk.type= buffer[socket][3];
	talk.color= Duint8_t2WORD( buffer[socket][4], buffer[socket][5] );
	talk.font= Duint8_t2WORD( buffer[socket][6], buffer[socket][7] );
	talk.name+=pc->getCurrentName();

	wstring speechUni;
	string2wstring( speech, speechUni );

	wstring* speechGhostUni=NULL;

	int range;
	switch ( buffer[socket][3] ) {
		case 0x09 : //yell
			range=(int)(VISRANGE*1.5);
			break;
		case 0x08 : //wisper
			range=2;
			break;
		default:
			range=VISRANGE;
	}

	NxwSocketWrapper sw;
	sw.fillOnline( pc, false, range );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		NXWCLIENT ps=sw.getClient();
		if( ps==NULL )
			continue;
		pChar a_pc= ps->currChar();
		if(! a_pc )
			continue;

		if( a_pc->unicode )			// language
		{
			talk.language = LongFromCharPtr( buffer[socket]+8 );
		}
		else
		{
			talk.language = calcserial( 'E', 'N', 'U',  0 );
		}

		bool ghost;
		if( pc->dead && !a_pc->dead && !a_pc->IsGMorCounselor() && a_pc->spiritspeaktimer == 0 ) {
			if( speechGhostUni==NULL ) {
				speechGhostUni=new wstring();
				makeGhost( pc->getSpeechCurrent(), speechGhostUni );
			}
			//ndEndy not set speechGhostUni because want send true speech to event
			ghost=true;
		}
		else
			ghost=false;

		pc->setSpeechCurrent( &speechUni );

		if( a_pc->amxevents[EVENT_CHR_ONHEARPLAYER]!=NULL )
			a_pc->amxevents[EVENT_CHR_ONHEARPLAYER]->Call( a_pc->getSerial(), pc->getSerial(), ghost );
		//a_pc->runAmxEvent( EVENT_CHR_ONHEARPLAYER, a_pc->getSerial(), pc->getSerial(), ghost );

		bool modifiedInEvent = false;
		if( pc->getSpeechCurrent()==&speechUni ) { //so not was modified in event
			modifiedInEvent=false;
			if( ghost )
				pc->setSpeechCurrent( speechGhostUni );
		}
		else
			modifiedInEvent=true;


		talk.msg = pc->getSpeechCurrent();
		talk.send( a_pc->getClient() );

		if( modifiedInEvent )
			pc->deleteSpeechCurrent();
	}

	pc->resetSpeechCurrent();
	if( speechGhostUni!=NULL )
		safedelete(speechGhostUni);

	if ( buffer[socket][3] == 0 || buffer[socket][3] == 2) //speech type
	{
		pc->saycolor = ShortFromCharPtr(buffer[socket] +4);
	}
	//
	// Advanced speech logging by elcabesa
	//
	if (SrvParms->speech_log)
	{
		SpeechLogFile logfile(pc);
		logfile.Write("%s [%08x] [%i] said: %s\n", pc->getCurrentName().c_str(), pc->getSerial(), pc->account, speech.c_str());

		int n= 0;
		string namelist= "to: ";

		
		NxwSocketWrapper sw;
		sw.fillOnline( pc );

		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			
			NXWCLIENT ps=sw.getClient();
			if(ps==NULL)
				continue;
			pChar pc_new_char = ps->currChar();
			if( pc_new_char )
			{
				namelist+= "[" + string( pc_new_char->getCurrentName().c_str() ) + "] ";
				++n;
			}
		}
		if( n==0 )
			namelist+= "himself or NPC";

		logfile.Write(namelist + "\n");
	}
	//
	// Make npcs, houses, boats etc not respond to dead people
	//
	if (pc->dead)
		return;
	//
	//	Capitalize speech for easy handling
	//
	j = speech.size();
	for( i = 0; i < j; ++i)
		speech[i] = toupper( speech[i] );
	//
	// Process speech for guildstones, boat tillermen, houses and npcs
	//
	if( resignFromGuild( pc, socket, speech ) )
		return;

	if( callGuards( pc, socket, speech ) )
		return;

	if( Boats->Speech( pc, socket, speech ) )
		return;

	if( house_speech( pc, socket, speech ) )
		return;
	//
	// Collect all npcs in visual range
	//
	if( nearbyNpcs != NULL )
		safedelete(nearbyNpcs);
		
	nearbyNpcs = new NxwCharWrapper;
	nearbyNpcs->fillCharsNearXYZ(pc->getPosition(), range, true, false );
	nearbyNpcs->rewind(); //Luxor: this is important! Otherwise isEmpty will return true!
	if( nearbyNpcs->isEmpty() ) {
		safedelete(nearbyNpcs);
		nearbyNpcs = NULL; //Luxor
		return;
	}
	//
	// Handle stablemaster commands
	//
	if( Speech::Stablemaster::respond( pc, socket, speech ) )
		return;
	//
	// Handle guard commands
	//
	if( Speech::Guard::respond( pc, socket, speech ) )
		return;
	//
	//
	//
	if( response( socket ) )
		return;
	//
	// Process npc triggers
	//

	pChar pc_found = NULL;

	NxwCharWrapper sc;
	sc.fillCharsNearXYZ( pc->getPosition(), 2, true, false );
	//pCharVector	sc( pointers::getCharFromWorldMap( pc->getPosition().x, pc->getPosition().y, VISRANGE, pointers::NPC ));
	//pCharVectorIt	scIt( sc.begin() ), scEnd( sc.end() );

	//for( ; scIt != scEnd; ++scIt )
	//{
	//pChar pj = (*scIt);
	for( sc.rewind(); !sc.isEmpty(); sc++ ) {
		pChar pj=sc.getChar();
		if(pj) {
			if ((pc->getSerial() != pj->getSerial()) && (pj->npc) )
			{
				pc_found = pj;
				break;
			}
		}
	}

	if ( pc_found &&(pc_found->speech) )
	{

		if(abs(pc_found->getPosition().z-pc->getPosition().z) >3 ) return;

		responsevendor(socket, DEREF_pChar(pc_found));

		cScpIterator* iter = NULL;
		char script1[1024];
		char script2[1024];

		iter = Scripts::Speech->getNewIterator("SECTION SPEECH %i", pc_found->speech);
		if (iter==NULL) return;
		match = 0;
		strcpy(sect, "NO DEFAULT char DEFINED");
		int loopexit2 = 0;
		do
		{
			iter->parseLine(script1, script2);
			if ((script1[0]!='}')&&(script1[0]!='{'))
			{
				if (!(strcmp("DEFAULT", script1)))
				{
					strcpy(sect, script2);
				}
				if (!(strcmp("ON", script1)))
				{
					if (match != 1)
					{
						for(i=0; i < strlen(script2); i++)
							script2[i] = toupper( script2[i] );

						match = (speech.find( script2 ) != std::string::npos);
					}
				}
				if (!(strcmp("SAY", script1)))
				{
					if (match == 1)
					{
						pc_found->talk(socket, script2, 0);
						match = 2;
					}
				}

				if (!(strcmp("TRG", script1))) // Added by Magius(CHE) §
				{
					if (match == 1)
					{
						pc_found->trigger = str2num(script2);
						triggerNpc(socket, pc_found, TRIGTYPE_NPCWORD);
						strcpy(script1, "DUMMY");
						match = 2;
					}
				}

				if ( !strcmp("@CALL", script1) )	// Courtesy of Flamegod - Revelation emu
					AmxFunction::g_prgOverride->CallFn( AmxFunction::g_prgOverride->getFnOrdinal( script2 ), static_cast<int>(pc_found->getSerial()), static_cast<int>(socket) );

			}
		}
		while ((script1[0] != '}')  && (++loopexit2 < MAXLOOPS) && (match != 2));
		if (match == 0)
			pc_found->talk(socket, sect, 0);
		safedelete(iter);
	}
}
