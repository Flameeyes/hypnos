/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cClient class
*/

#include "networking/cclient.h"
#include "settings.h"
#include "misc.h"

static ClientSList cClient::clients;
static ClientSList cClient::cGMs;

cClient::cClient(int32_t sd, struct sockaddr_in* addr)
{
	sock = new cSocket(sd, addr);
	pc = NULL;
	acc = NULL:
	visualRange = VISRANGE;
	dragItem = NULL;
	target.source = NULL;
	resetDragging();
	clients.push_back(this);
}

cClient::~cClient()
{
	if (isDragging()) // if client crashes while still dragging, revert item position
	{
		dragItem->moveTo(dragItem->getOldPosition());
		dragItem->setContainer(dragItem->getOldContainer());
		dragItem->Refresh();
	}
	if (hasOpenTradeSessions())
	{
		/*
		clear all open secure trade sessions before destroying this client, automatically canceling it
		and putting all items back on respective backpacks
		*/
		std::list<sSecureTradeSession>::iterator it = SecureTrade.begin();
		for(;it!=SecureTrade.end(); ++it) endtrade(*it);
	}

	if ( pc )
		pc->setClient(NULL);
	if ( acc )
		acc->setClient(NULL);

	delete sock;
	clients.erase(find(clients.begin(), clients.end(), this));
}

/*!
\brief Disconnects client

Updated to hypnos (Chronodt 29/1/04)
\todo find something to replace socket number output with
*/
void cClient::disconnect()
{
	static const char msgDisconnect[]	= "Client %i disconnected. [Total online clients: %i]\n";
	static const char msgPart[]		= "%s has left the realm";

	time_t ltime;
	time( &ltime );

        int clientnumber = 0;
	for (cClients::iterator i = cClient::clients.begin(); (i != client) || (cClient::clients.end()); i++ ) clientnumber++;

	outInfof( msgDisconnect, clientnumber , cClient::clients.size() - 1 );

	if (SrvParms->server_log)
		ServerLog.Write( msgDisconnect, clientnumber , cClient::clients.size() - 1);

	if( currChar() )
	{
		if (SrvParms->partmsg && currChar() )
			sysbroadcast( msgPart, currChar()->getCurrentName().c_str() );
		
		if ( currChar()->IsOnline() )
		{
			LogOut( client );


			if( currChar()->party!=INVALID ) {
				P_PARTY party = Partys.getParty( currChar()->party );
				if( party!=NULL )
					party->removeMember( currChar() );
			}


			uint32_t pc_serial = currChar()->getSerial();

			for (cClients::iterator i = cClient::clients.begin(); cClient::clients.end(); i++ )
			{
                                pChar pi = (*i)->currChar();
				if ( pi )
					if( currChar() != pi && currChar()->hasInRange(pi) )
					{
						nPackets::Sent::DeleteObj pk(currChar());
        		                        (*i)->sendPacket(&pk);
					}
			}
		}
	}
	//! \todo revise from here
	FlushBuffer( client );

	closesocket( client[ socket ] ); //so it bombs and free the mutex :]

#ifdef ENCRYPTION
	if ( clientCrypter[socket] != NULL && !clientCrypter[socket]->getEntering())
	{
		delete (ClientCrypt * ) clientCrypter[socket] ;
		clientCrypter[socket]=NULL;
	}
#endif

	if( currChar() )
	{
		currChar()->setClient( NULL );

		if( currChar()->murderrate>getclock() ) //save murder decay
			currChar()->murdersave= (currChar()->murderrate -getclock()) / SECS;

	}

	loginchars[socket] = NULL;

	safedelete( clientInfo[socket] );

	for ( j = socket; j < now - 1; ++j )
	{
		int jj = j+1;
		client[j]=client[jj];
		loginchars[socket] = loginchars[jj];
		cryptedClient[j]=cryptedClient[jj];
#ifdef ENCRYPTION
		clientCrypter[jj] = clientCrypter[j];
#endif
		clientip[j][0]=clientip[jj][0];
		clientip[j][1]=clientip[jj][1];
		clientip[j][2]=clientip[jj][2];
		clientip[j][3]=clientip[jj][3];
		acctno[j]=acctno[jj];
		binlength[j]=binlength[jj];
		boutlength[j]=boutlength[jj];
		clientInfo[j]=clientInfo[jj];
		walksequence[j]=walksequence[jj];
		clientDimension[j]=clientDimension[jj];

		memcpy(&buffer[j], &buffer[jj], MAXBUFFER); // probably not nec.
		memcpy(&outbuffer[j], &outbuffer[jj], MAXBUFFER); // very important

	}

	pChar pj = NULL;
	for ( i = 0; i < MAXCLIENT; ++i ) {
		pj = loginchars[i];
		if ( pj )
			pj->setClient(NULL);

		if ( i >= now )
			loginchars[i] = INVALID;
	}

	--now;

	for ( i = 0; i < now; ++i ) {
		pj = loginchars[i];
		if( pj )
			pj->setClient(new cNxwClientObj(i));
	}
}

/*!
\brief Compress packet buffer
\param out_buffer buffer to send
\param out_len size of buffer to send
*/
void cClient::compress(uint8_t *&out_buffer, uint32_t& out_len)
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
\author Flameeyes
\param cont Container to show
*/
void cClient::showContainer(pContainer cont)
{
	if ( ! cont )
		return;
	
	nPackets::Sent::OpenGump pk(cont->getSerial(), cont->getGump());
	sendPacket(&pk);

	nPackets::Sent::ContainerItem pk2;
	
	cont->lockItemsMutex();
	for(ItemSLIst::const_iterator it = cont->getItems().begin(); it != cont->getItems().end() )
	{
		if ( ! (*it) ) continue;
		
		//! \todo The position of the items should be bound to client-specific protocol
		if ( (*it)->getPosition().x > 150 ) (*it)->setPositionX(150);
		if ( (*it)->getPosition().y > 150 ) (*it)->setPositionY(150);
		
		pk2.addItem(*it);
	}
	cont->unlockItemsMutex();
	
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

	nPackets::Sent::ShowItemInContainer pk(item);
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

	nPackets::Sent::OverallLight pk(level);

	sendPacket(&pk);
}


void cClient::updatePaperdoll()
{
	nPackets::Sent::PaperdollClothingUpdated pk();
	sendPacket(&pk);
}

void cClient::showBankBox(pPC dest)
{
	if ( ! dest || (dest != pc && acc->getPrivLevel() < privSeer ) )
		return;

	pEquippableContainer bank = dest->getBankBox();
	if ( ! bank )
		return;

	showContainer(bank);
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

	pEquippableContainer bank = dest->getSpecialBankBox();
	if ( ! bank )
		return;

	showContainer(bank);
}

/*!
\brief send status window of target to client
\param target character whose status window has to be reported to client
\param extended if extended info needed
*/

void cClient::statusWindow(pChar target, bool extended) //, bool canrename)  will be calculated from client data
{
	if(!target) return;
	pPC pc = currChar();  // pc who will get the status window

	bool canrename;

	if (pc->IsGM() || (target->getOwner() == pc && target != pc) ) canrename = true;
	else canrename = false;

	if ((pc->getBody()->getId() == bodyMaleDead) || (pc->getBody()->getId() == bodyFemaleDead)) canrename = false;
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

	nPackets::Sent::Status pk(sorg, ext, canrename);
	sendPacket(&pk);
}

/*!
\brief updates status window of client if drag & drop has modified player in any way, weight included
\param item item moved in drag & drop
*/
void cClient::updateStatusWindow(pItem item)
{
	if ( ! item ) return;

	pChar pc = currChar();
	pChar owner = item->getCurrentOwner();
	pContainer oldcont = item->getOldContainer();
	pChar oldowner = (oldcont) ? oldcont->getCurrentOwner() : NULL;

	if( oldcont->getOutMostCont() != item->getOutMostCont() && (owner == pc || oldowner == pc) )     //!< if item was in pack and has been moved out or has been equipped/deequipped update char
		statusWindow( pc, true );
}


/*!
\brief brings up the skill window in client
*/
void cClient::skillWindow() // Opens the skills list
{
	pChar pc = currChar();
	if ( ! pc ) return;
	nPackets::Sent::SendSkills pk(pc);
	sendPacket(&pk);
}


void cClient::sendMidi(uint16_t id)
{
	nPackets::Sent::PlayMidi pk(id);
	sendPacket(&pk);
}

void cClient::senditem(pItem pi) // Shows items to client (on the ground or inside containers)
{
	if ( ! pi ) return;
	pChar pc= currChar();
	if ( ! pc ) return;

	if (!pc->hasInRange(pi, visualRange) ) return; //we must check on client's selected visual range for items to send (see packet 0xc8)
	if ( pi->visible==2 && !pc->isGM()) return;
	if ( pi->visible==1 && pc != pi->getOwner() && !pc->isGM()) return; //On visible set to 1, only owners or GMs see the item
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

	nPackets::Sent::ObjectInformation pk(pi, pc);
	sendPacket(&pk);
	if (pi->isCorpse()) dressCorpse((pContainer)pi);
}

// sends item in differnt color and position than it actually is
// used for LSd potions now, LB 5'th nov 1999
void cClient::senditem_lsd(pItem pi, uint16_t color, sLocation position)
{
	if ( ! pi ) return;
	pChar pc= currChar();
	if ( ! pc ) return;

	if ( pi->visible>=1 && !(pc->IsGM()) ) return;
	// for lsd we dont need extra work nor type 1 as in senditem

	nPackets::Sent::LSDObject pk(pi, pc, color, position);
	sendPacket(&pk);
}


//! Sends items worn on corpse for display on client
void cClient::dressCorpse(pContainer corpse)
{
	nPackets::Sent::CorpseClothing pk1(corpse);
	nPackets::Sent::ContainerItem pk2;

	corpse->lockItemsMutex();
	for(ItemSList::const_iterator it = corpse->getItems().begin(); it != corpse->getItems().end() )
	{
		if ( ! (*it) ) continue;
		pEquippable pi = dynamic_cast<pEquippable> *it;
		if (pi && pi->getLayer())
		{
			pk1.addItem(pi);
			pk2.addItem(pi);
		}
	}
	corpse->unlockItemsMutex();

	sendPacket(&pk);
	sendPacket(&pk2);
}


/*!
\brief shows char to client
\param pc character to show
\param z use z coordinate? (else use dispz)
*/
void cClient::sendchar(pChar pc, bool z)
{

	pChar pc_currchar = currChar();
	if(!pc_currchar) return;

	if (pc->isStabled() || pc->mounted) return; // dont **show** stabled pets

	if (pc->IsHidden() && pc!=pc_currchar && !pc_currchar->IsGM()) return;

	if( !pc->npc && !pc->IsOnline()  && !pc_currchar->IsGM() )
	{
		nPackets::Sent::DeleteObj pk(pc);
		sendPacket(&pk);
		return;
	}
	// hidden chars can only be seen "grey" by themselves or by gm's
	// other wise they are invisible=dont send the packet

	nPackets::Sent::DrawObject pk(this, pc, z);
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
	if( !pc_currchar ) return;

	//Luxor: not-movable items
	/*if (pi->magic == 2 || (cSerializable::isCharSerial(pi->getContSerial()) && pi->getContSerial() != pc_currchar->getSerial()) ) {
		if (cSerializable::isCharSerial(pi->getContSerial())) {
			pChar pc_i = cSerializable::findCharBySerial(pi->getContSerial());
			if ( pc_i )
//				pc_i->getClient()->sysmessage("Warning, backpack bug located!");
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

	// Check if item is equipped
	pContainer container=pi->getOutMostCont();
	pEquippable equipcont = dynamic_cast<pEquippable> container;
	pEquippable equipitem = dynamic_cast<pEquippable> pi;
	pBody body = pc_currchar->getBody();
	pChar owner=pi->getCurrentOwner();

	if( equipitem && equipitem->getContainer() == body && !equipitem->getLayer()) //if item is equippable and equipped on this client's (current) body
	{
 		if( body->unEquip( equipitem, true ) == 1 )	// bypass called
		{
			if( isDragging() )
			{
				resetDragging();
				updatePaperdoll();
			}
			return;
		}
	}

	if ( owner && (owner != pc_currchar) ) //Item is on another char
	{
		if ( !pc_currchar->IsGM() && !pc_currchar->isOwnerOf(owner))
		{
			//if not pets or player vendors (of this client's current char) bounce item
			nPackets::Sent::BounceItem pk(0);
			sendPacket(&pk);
			if (isDragging())
			{
				resetDragging();
				//pi->setOldContainer(pi->getContainer());
				item_bounce3(pi);
			}
			return;
		}
	}


	if ( container )
	{
		pFunctionHandle evt = pi->getEvent(cItem::evtItmOnTakeFromContainer);
		if ( evt )
		{
			cVariantVector params = cVariantVector(3);
			params[0] = pi->getSerial(); params[1] = container->getSerial();
			params[2] = pc_currchar->getSerial();
			evt->setParams(params);
			evt->execute();
			if ( evt->isBypassed() )
			{
				nPackets::Sent::BounceItem pk(5);
				sendPacket(&pk);
				if (isDragging())
				{
					resetDragging();
					updateStatusWindow(pi);
				}
				if (equipitem && !equipitem->getOldLayer())
				{
					if ( body->equip(equipitem, true) == 1 )
					{
						equipitem->setOldLayer(0);
						pack_item(pi, pc_currchar->getBackpack()); // If reequip canceled due to script bypass, dump item to the backpack
					}
				}
				else
				{
					pi->setContainer( pi->getOldContainer() );
					pi->setPosition( pi->getOldPosition() );
				}
				pi->Refresh();
				return;
			}
		}

		if(hasOpenTradeSessions())	//Secure trade check, but only if at least one is open
		{
			sSecureTradeSession session = findTradeSession(container);
			if (session.tradepartner)
			{
				if (session.status1 || session.status2)
				{
					session.status1=0;
					session.status2=0;
					sSecureTradeSession session2 = session.tradepartner->findTradeSession(this);
					session2.status1=0;
					session2.status2=0;
					dest->morez=0;
					sendtradestatus(session);
				}
			}
		}
		
		pContainer pi_cont = dynamic_cast<pContainer> pi->getContainer();
		if ( pi_cont ) //pi_cont could have been a body if it was the player's backpack
		{
			evt = picont->getEvent(cContainer::evtCntOnTakeItem);
			if ( evt )
			{
				cVariantVector params = cVariantVector(3);
				params[0] = pi_cont->getSerial(); params[1] = pi->getSerial();
				params[2] = pc_currchar->getSerial();
				evt->setParams(params);
				evt->execute();
				if ( evt->isBypassed() )
				{
					nPackets::Sent::BounceItem pk(5);
					sendPacket(&pk);
					if (isDragging())
					{
						resetDragging();
						updateStatusWindow(pi);
					}
					if (equipitem && !equipitem->getOldLayer())
					{
						if (body->equip(equipitem, true) == 1)
						{
							equipitem->setOldLayer(0);
							pack_item(pi, pc_currchar->getBackpack()); // If reequip canceled due to script bypass, dump item to the backpack
						}
					}
					else
					{
						pi->setContainer( pi->getOldContainer() );
						pi->setPosition( pi->getOldPosition() );
					}
					pi->Refresh();
					return;
				}
			}
		}
		if ( container->isCorpse() )
		{
			if ( container->getOwner() != pc_currchar)
			{ //Looter :P
				pc_currchar->unHide();
				bool bCanLoot = false;
				if( pc_currchar->party!=INVALID ) {
					pChar dead = container->getOwner();
					//!\todo check when party redone
					if( dead && dead->party==pc_currchar->party )
					{
						pParty party = Partys.getParty( pc_currchar->party );
						if( !party )
						{
							pPartyMember member = party->getMember( pc_currchar->getSerial() );
							if( member!=NULL ) bCanLoot = member->canLoot;
						}
					}
				}
				if ( !bCanLoot && container->more2==1 ) //!< \todo the more2 here represent the innocence level of corpse (1 = blue). Replace with something better than an anonymous more2
				{
					pc_currchar->IncreaseKarma(-5);
					//!\todo should be investigated
					pc_currchar->setCrimGrey(nSettings::Reputation::getLootingAction());
					sysmessage("You are loosing karma!");
				}
			}
		} // corpse stuff

		container->setMulti(NULL);

		//at end reset decay of container
		container->setDecayTime();

	} // end cont valid


	if ( !pi->corpse )
	{
		updateStatusWindow(pi);

		if ( ! isMovable(pc_currchar, pi) )
		{
			nPackets::Sent::BounceItem pk(0);
			sendPacket(&pk);
			if (isDragging()) // only restore item if it got dragged before !!!
			{
				resetDragging();
				item_bounce4(pi);
			}
		} // end of can't get
		else
		{
			// AntiChrist bugfix for the bad bouncing bug ( disappearing items when bouncing )
			setDragging();
			pi->setOldPosition( pi->getPosition() ); // first let's save the position

			if (equipitem) //remember: equipitem is pi dynamic-casted to pEquippable
			{
				equipitem->setOldLayer(equipitem->getLayer());	// then the layer
				body->setLayerItem(layNone, equipitem);
			}

			if (!pi->isInWorld()) pc_currchar->playSFX(0x0057);
			else if (body->getSkill(skillStealth) < nSettings::Skills::getStealthToTakeItemsWhileHid() && !pc_isGM()) pc_currchar->unHide();

			if (pi->getAmount()>1)
			{
				if (amount > pi->getAmount())
					amount = pi->getAmount();
				else if (amount < pi->getAmount())
				{ //get not all but a part of item ( piled? ), but anyway make a new one item

					pItem pin = new cItem();
					(*pin)=(*pi);

					pin->setAmount(pi->getAmount() - amount);

					pin->setContainer(pi->getContainer());	//Luxor
					pin->setPosition( pi->getPosition() );

					/*if( !pin->isInWorld() && cSerializable::isItemSerial( pin->getContainer()->getSerial() ) )
						pin->SetRandPosInCont( (pItem)pin->getContainer() );*/

					statusWindow(pc_currchar, true); //since a new item has been created, it is better to do directly the statusWindow
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

			} // end if not corpse
			pointers::delFromLocationMap( pi );
			if (!pc_currchar->isGM() && !pc_currchar->isHidden() && owner != pc_currchar)
			{
				//the dragging of the item should not be shown if pc is a gm, is invisible or is trying to get an item on his person (or bank)
				//! \todo this packet has to be sent to all surrounding clients EXCEPT "this"
				//! \todo complete when sets remade
				//! \todo verify if picking up items from the ground while hidden should unhide
				nPackets::Sent::DragItem pk(pi, pc_currchar->getPosition(), amount);
				sw->sendPacket(&pk);		//this packets shows to those clients the item being dragged to the char
			}
			pi->setPosition( 0, 0, 0 );
			pi->setContainer(NULL);
			dragItem = pi;
		}
	}
	body->calcWeight();
	updatestatusWindow(pi);
}


/*!
\brief Drop an item
\author Unknown, moved here by Chronodt (3/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param dest container into which *pi has to be dropped (NULL = world)
*/
void cClient::drop_item(pItem pi, sLocation &loc, pSerializable dest) // Item is dropped
{

	if (clientDimension==3)
	{
	// UO:3D clients send SOMETIMES two dragg packets for a single dragg action.
	// sometimes we HAVE to swallow it, sometimes it has to be interpreted
	// if UO:3D specific item loss problems are reported, this is probably the code to blame :)
	// LB

		if ( loc == (0xFFFF, 0xFFFF, 0) && evilDrag )
		{
			evilDrag=false;
			return;
		}	 // swallow! note: previous evildrag !

		if ( loc == (0xFFFF, 0xFFFF, 0) && !evilDrag )
		{
			item_bounce6( pi);
			return;
		}
		if ( ( loc != (0xFFFF, 0xFFFF, 0) && !cont ) || ( (pi->getSerial()>=0x40000000) && (cont && cont->getSerial()>=0x40000000) ) )
			evilDrag=true; // calc new evildrag value
		else evilDrag=false;
	}

	if ( !dest) dump_item(pi, loc); // Invalid target => invalid container => put inWorld !!!
	else if (dynamic_cast<pChar>(dest)) droppedOnChar(pi, loc, dynamic_cast<pChar>(dest));
	else if (dynamic_cast<pItem>(dest)) pack_item(pi, dynamic_cast<pItem>(dest));
}

/*!
\brief Item is dragged on another item
\author Unknown, moved here by Chronodt (3/2/2004)
\param pi Item to be dropped (already in dragging mode)
\param dest Container into which the item has to be dropped (or item being dragged upon)
*/
void cClient::pack_item(pItem pi, pItem dest) // Item is dragged on another item
{
	pChar pc= currChar();
	if ( ! pc ) return;

	sLocation charpos = pc->getPosition();


	if (pi->getId() >= 0x4000)
	{
		sysmessage("Hey, putting houses in your pack crashes your back and client!");
		return;
	}

	//ndEndy recurse only a time
	pChar destOwner = dest->getCurrentOwner();
	pNPC npc = dynamic_cast<pNPC> destOwner;

	if( destOwner )
	{				 	               					// it has to bounce if:
		if ( (destOwner != pc && !pnc && !pc->IsGM())  ||				// a) trying to put item on another pc's pack
		     (npc && npc->npcaitype==NPCAI_PLAYERVENDOR && !pc->isOwnerOf(npc)))	// b) npc is not your player vendor
		{ // Luxor
			sysmessage("This aint your backpack!");
			nPackets::Sent::BounceItem pk(5);
			sendPacket(&pk);
			if (isDragging()) {
				resetDragging();
				item_bounce3(pi);
				if (dest->getId() >= 0x4000)
					senditem(dest); //If tried to put a house in backpack, resend all items in it
			}
			return;
		}
	}
	
	pFunctionHandle evt = pi->getEvent(cItem::evtItmOnPutInContainer);
	if ( evt )
	{
		cVariantVector params = cVariantVector(3);
		params[0] = pi->getSerial(); params[1] = dest->getSerial();
		params[2] = pc->getSerial();
		evt->setParams(params);
		evt->execute();
		if ( evt->isBypassed() )
		{
			item_bounce6(pi);
			return;
		}
	}

	pContainer destcont = dynamic_cast<pContainer>(dest);
	if ( destcont && ( evt = destcont->getEvent(cContainer::evtCntOnPutItem) ) )
	{
		cVariantVector params = cVariantVector(3);
		params[0] = destcont->getSerial(); params[1] = pi->getSerial();
		params[2] = pc->getSerial();
		evt->setParams(params);
		evt->execute();
		if ( evt->isBypassed() )
		{
			item_bounce6(pi);
			return;
		}
	}

	if(hasOpenTradeSessions())	//Secure trade check, but only if at least one is open
	{
		sSecureTradeSession session = findTradeSession(destcont->getOutMostCont());
		if (session.tradepartner)
		{
			if (session.status1 || session.status2)
			{
				session.status1=0;
				session.status2=0;
				sSecureTradeSession session2 = session.tradepartner->findTradeSession(this);
				session2.status1=0;
				session2.status2=0;
				dest->morez=0;
				sendtradestatus(session);
			}
		}
	}

	if(SrvParms->usespecialbank)//only if special bank is activated
	{
		if(dest->morey==MOREY_GOLDONLYBANK && dest->morex==MOREX_BANK)
		{
			if ( pi->getId() == ITEMID_GOLD )
			{//if they're gold ok
				pc->playSFX( goldsfx(2) );
			}
			else
			{	//if they're not gold..bounce on ground
				sysmessage("You can only put golds in this bank box!");

				pi->setContainer(NULL);
				pi->MoveTo( charpos );
				resetDragging();
				dragItem = NULL;

				//! \todo this packet has to be sent to all surrounding clients INCLUDING "this" (not sure it is needed)
				//! \todo complete when sets remade

				nPackets::Sent::DragItem pk(pi, pc_currchar->getPosition(), pi->getAmount());
                        	sw->sendPacket(&pk);		//this packets shows to those clients the item being dropped on the ground

				pc->playSFX( itemsfx(pi->getId()) );
				pi->Refresh();
				pc_currchar->getBody()->calcWeight();
				if (destOwner) destOwner->getBody()->calcWeight();
				updateStatusWindow(pi);
				return;
			}
		}
	}


	// Xanathars's Bank Limit Code
	if ( nSettings::Server::getBankMaxItems() )
	{
		pContainer contOutMost = pi->getOutMostCont();
		if ( ! contOutMost || contOutMost->morex != MOREX_BANK )
			continue;
		
		int n = contOutMost->CountItems( INVALID, INVALID, false);
		n -= contOutMost->CountItems( ITEMID_GOLD, INVALID, false);
		if( isContainer(pi) )
			n += (dynamic_cast<pContainer>(pi))->CountItems( INVALID, INVALID, false);
		else
			++n;
		
		if( n > nSettings::Server::getBankMaxItems() )
		{
			sysmessage("You exceeded the number of maximimum items in bank of %d", nSettings::Server::getBankMaxItems());
			item_bounce6(pi);
			return;
		}
	}


	//ndEndy this not needed because when is dragging dest serial is INVALID
	//testing UOP Blocking Tauriel 1-12-99
	if (!pi->isInWorld())
	{
		item_bounce6(pi);
		return;
	}

	if ( ! isMovable(pc, pi) )
	{
		nPackets::Sent::BounceItem pk(5);
		sendPacket(&pk);
		if (isDragging())
		{
			resetDragging();
			item_bounce3(pi);
			if (dest->getId() >= 0x4000)
				senditem(dest);
		}
		return;
	}
	// - Trash container
	if( dest->type==ITYPE_TRASH)   //!< \todo find a true trash container check
	{
		pi->Delete();
		//! \todo when tempfx revised, do a timed deletion
		sysmessage("As you let go of the item it disappears.");
		dragItem = NULL;
		resetDragging();
		return;
	}
	// - Spell Book
	if (dest->type==ITYPE_SPELLBOOK)   //!< \todo find a true spellbook check
	{
		if (!pi->IsSpellScroll72())
		{
			sysmessage("You can only place spell scrolls in a spellbook!");
			nPackets::Sent::BounceItem pk(5);
			sendPacket(&pk);
			if (isDragging())
			{
				resetDragging();
				item_bounce3(pi);
			}
			if (dest->getId() >= 0x4000)
				senditem(dest);
			return;
		}
		if (destOwner != pc && !pc->CanSnoop())
		{
			sysmessage("You cannot place spells in other people's spellbooks.");
			item_bounce6(pi);
			return;
		}
                std::string temp1, temp2;
		if( pi->getCurrentName()[0] == "#") temp2 = pi->getName();   //!<\todo tile search for default name
		else temp2 = pi->getCurrentName();


		NxwItemWrapper sii;
		sii.fillItemsInContainer( dest, false );
		for( sii.rewind(); !sii.isEmpty(); sii++ )
		{
			pItem pi_ci=sii.getItem();
			if ( pi_ci )
			{
				if( pi_ci->getCurrentName()[0] == "#" )
					temp1 = pi_ci->getName();
				else
					temp1 = pi_ci->getCurrentName();

				if( temp1 == temp2 || temp1 == "All-Spell Scroll")
				{
					sysmessage("You already have that spell!");
					item_bounce6(pi);
					return;
				}
			}
			// Juliunus, to prevent ppl from wasting scrolls.
			if (pi->amount > 1)
			{
				sysmessage("You can't put more than one scroll at a time in your book.");
				item_bounce6(pi);
				return;
			}
		}
		dest->addItem( pi ); //add item without trying to stack
		sendSpellBook(dest);
		dragItem = NULL;
		resetDragging();
		return;
	}

        if (isContainer(dest))
	{
		if ( npc && npc->npcaitype==NPCAI_PLAYERVENDOR && npc->getOwner()==pc )
		{
			//!\todo modify speech awaiting triggers
			pc->fx1= pi->getSerial();
			pc->fx2=17;
			sysmessage("Set a price for this item.");
		}


		if (!pc_currchar->isGM() && !pc_currchar->isHidden() && destOwner != pc_currchar)
		{
                       	//NOTE!!!!! The drag item packet is done BEFORE the actual sending, because after a combinable addItem(), pi may be NULL
			//the dragging of the item should not be shown if pc is a gm, is invisible or is trying to drop an item on his backpack (or bank)
			//! \todo this packet has to be sent to all surrounding clients EXCEPT "this"
			//! \todo complete when sets remade
			nPackets::Sent::DragItem pk(pi, dest->getWorldPosition(), pi->getAmount());
			sw->sendPacket(&pk);		//this packets shows to those clients the item from the char to the destination container (or char if vendor)
		}
		pc->playSFX( itemsfx(pi->getId()) );
		dest->AddItem(pi,0,0);
		pc_currchar->getBody()->calcWeight();
		updateStatusWindow(pi);
		if (destOwner) destOwner->getBody()->calcWeight();
		dragItem = NULL;
		resetDragging();
        } // end of if isContainer(dest). From here dest is supposed to be a simple item
	else if (dest->isInWorld())
	{
		if (!dest->isCombinableWith(pi))
		{
			dump_item (pi, dest->getPosition()); //if items are not stackable, we dump the item in the same tile
			return;
		}
		if (dest->getAmount() + pi->getAmount() > 65535))
		{ //If target is not a container, but an item in the world that is not stackable with dragged item, bounce.. but only if combined amount is too big!
			message("Too many of %s are already stacked there", pi->getCurrentName().c_str())
			item_bounce6(pi);
			return;
		}
		if (!pc_currchar->isGM() && !pc_currchar->isHidden())
		{
			//the dragging of the item should not be shown if pc is a gm, is invisible or is trying to drop an item on his backpack (or bank). Since it is already checked is on the ground...
			//! \todo this packet has to be sent to all surrounding clients EXCEPT "this"
			//! \todo complete when sets remade
			nPackets::Sent::DragItem pk(pi, dest->getWorldPosition(), pi->getAmount());
			sw->sendPacket(&pk);		//this packets shows to those clients the item from the char to the destination container (or char if vendor)
		}
		dest->setAmount(dest->getAmount() + pi->getAmount());
		dest->Refresh();
		pi->Delete();
		pc_currchar->getBody()->calcWeight();
		if (destOwner) destOwner->getBody()->calcWeight();
		updateStatusWindow(pi);                
		dragItem = NULL;
		resetDragging();
		return;
	}
	else  //item is not in world, so we check if item and dest are combinable. If they are, we pile them, if not we put pi down in a random position in dest's container
	{
		//NOTE!!!!! The drag item packet is done BEFORE the actual sending, because after a combinable addItem(), pi may be NULL
		if (!pc_currchar->isGM() && !pc_currchar->isHidden() && destOwner != pc_currchar)
		{
			//the dragging of the item should not be shown if pc is a gm, is invisible or is trying to drop an item on his backpack (or bank)
			//! \todo this packet has to be sent to all surrounding clients EXCEPT "this"
			//! \todo complete when sets remade
			nPackets::Sent::DragItem pk(pi, dest->getWorldPosition(), pi->getAmount());
			sw->sendPacket(&pk);		//this packets shows to those clients the item being dragged to the char
		}
		if (dest->isCombinableWith(pi))
			dest->getContainer()->addItem(pi, 0, 0); //since items are stackable, putting x & y as 0 will still stack & randomize position if combined amount > 65535
		else
			dest->getContainer()->addItem(pi); // random positioning

		pc_currchar->getBody()->calcWeight();
		if (destOwner) destOwner->getBody()->calcWeight();
		updateStatusWindow(pi);
		dragItem = NULL;
		resetDragging();
	 }
}


/*!
\brief dragged item on the ground (drop)
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi Item to be dropped (already in dragging mode)
\param loc Position to drop item at
*/
void cClient::dump_item(pItem pi, sLocation &loc) // Item is dropped on the ground
{
	if ( ! pi ) return;

	pChar pc=currChar();
	if ( ! pc ) return;

	//Ripper...so order/chaos shields disappear when on ground.
	if( pi->getId()==0x1BC3 || pi->getId()==0x1BC4 )
	{
		pc->playSFX( 0x01FE);
		staticFX(pc, 0x372A, 9, 6);
		pi->Delete();
		dragItem = NULL;
		resetDragging();
		return;
	}


	//test UOP blocking Tauriel 1-12-99
	if (!pi->isInWorld())
	{
		item_bounce6(pi);
		return;
	}

	if ( ! isMovable(pc, pi) )
	{
		item_bounce6(pi);
		return;
	}

	pFunctionHandle evt = pi->getEvent(cItem::evtItmOnDropInLand);
	if ( evt )
	{
		cVariantVector params = cVariantVector(2);
		params[0] = pi->getSerial(); params[1] = pc->getSerial();
		evt->setParams(params);
		evt->execute();
		if ( evt->isBypassed() )
		{
			item_bounce6(pi);
		        pi->Refresh();                        
		        return;
	        }
	}
	
	if ( !lineOfSight( pc->getPosition(), loc ) )
        {
                sysmessage("You cannot place an item there!");
		nPackets::Sent::BounceItem pk(5);
		sendPacket(&pk);
               	if (isDragging())
		{
                       	resetDragging();
			item_bounce6(pi);
               		pi->Refresh();
               		updateStatusWindow(pi);
      		}
        	return;
	}

	if (!pc->IsGM())
	{
		NxwItemWrapper si;
		si.fillItemsAtXY( loc->x, loc->y );
		if (si.size() >= nSettings::Server::getMaximumItemsOnTile()) //Only those many items permitted on a tile
                {
			sysmessage("There is not enough space there!");
			nPackets::Sent::BounceItem pk(5);
			sendPacket(&pk);
			if (isDragging())
                        {
				resetDragging();
                                item_bounce6(pi);
				pi->Refresh();
				updateStatusWindow(pi);
			}
                        return;
		}
	}
	if (pc->getBody()->getSkill(skillStealth) < nSettings::Skills::getStealthToDropItemsWhileHid() && !pc_isGM()) pc->unHide();
	NxwSocketWrapper sw;
	sw.fillOnline( pi );
        for( sw.rewind(); !sw.isEmpty(); sw++ )
        {
		nPackets::Sent::DeleteObj pk(pi);
		sw->sendPacket(&pk);

		if (!pc->isGM() && !pc->isHidden() && sw != this)
		{
	               	//the dragging of the item should not be shown if pc is a gm or if it's invisible
	                //! \todo complete when sets remade
			nPackets::Sent::DragItem pk(pi, Loc, pi->getAmount());
	                sw->sendPacket(&pk);		//this packets shows to those clients the item being dragged to the char
                }
	}

	pc->getBody()->calcWeight();
	updateStatusWindow(pi);
	pc->playSFX( itemsfx(pi->getId()) );

        pi->MoveTo(Loc);
        pi->setContainer(NULL);
	dragItem = NULL;
	resetDragging();
	
	pi->setMulti( cMulti::getAt(Loc) );

	pi->Refresh();
}

/*!
\brief Verifies if item has been dropped on a char and if so executes necessary code
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi Item to be dropped (already in dragging mode)
\param dest Char where to drop the item to
*/
void cClient::droppedOnChar(pItem pi, pChar dest)
{
	if( !pi || !dest ) return false;

	pChar pc_currchar = currChar();
	if(!pc_currchar) return false;

	sLocation charpos = pc_currchar->getPosition();
	pNPC npc = dynamic_cast<pNPC> dest;

	if (pc_currchar == dest)
	{
		droppedOnSelf(pi);
		return;
	}
	
	if (npc)
	{
		if(!npc->getBody()->isHuman()) droppedOnPet( pi, npc);
		else	// Item dropped on a Human character
		{
			// Item dropped on a Guard (possible bounty quest)
			if ( npc->npcaitype == NPCAI_TELEPORTGUARD ) droppedOnGuard(pi, npc);
			if ( npc->npcaitype == NPCAI_BEGGAR )	droppedOnBeggar(pi, npc);
			//! \todo add a money-accepting part even for non-trainers. They won't give karma for it, but they will thank nonetheless :P
			if(pc_currchar->getTrainer() != npc)
			{
				npc->talk(this, "Thank thee kindly, but I have done nothing to warrant a gift.", false);
				nPackets::Sent::BounceItem pk(5);
				sendPacket(&pk);
				if (isDragging())
				{
					resetDragging();
					item_bounce5(pi);
				}
				return;
			}
			else // The player is training from this NPC
			{
				droppedOnTrainer( pi, npc);
			}
		}//if human or not
	}
	else // dropped on another player
	{
		// Avoid starting the trade if GM drops item on logged out char (crash fix)
		if ((pc_currchar->IsGM()) && !dest->isOnline())
		{
			// Drop the item in the players pack instead
			// Get the pack
			pItem pack = dest->getBackpack();
			if (pack != NULL)	// Valid pack?
			{
				pack->AddItem(pi);	// Add it
				dest->getBody()->calcWeight();
			}
			else	// No pack, give it back to the GM
			{
				nPackets::Sent::BounceItem pk(5);
				sendPacket(&pk);
				if (isDragging())
				{
					resetDragging();
					item_bounce5(pi);
					updateStatusWindow(pi);
				}
			}
		}
		else
		{
			//Now finding if they have already a secure trade session open
			pClient tradeClient = (dynamic_cast<pPC>dest)->getClient();
			sSecureTradeSession session = findTradeSession(tradeClient);
			if (session.tradepartner)
			{
				pContainer tradeCont = tradestart(tradeClient);
				if ( tradeCont )
				{
					tradeCont->AddItem( pi, 30, 30 );
					dragItem = NULL;
					resetDragging();
					return
				}
				else
				{
					nPackets::Sent::BounceItem pk(5);
					sendPacket(&pk);
					if (isDragging())
					{
						resetDragging();
						item_bounce5(pi);
						updateStatusWindow(pi);
					}
				}
			}
			else	//if trade session already open, send item to that container
			{
				pack_item(pi, session.container1);
				return;
			}
		}
	}
	return;
}

/*!
\brief item has been dropped on a pet and verifies if correct item given
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi Item to be dropped (already in dragging mode)
\param pet Pet where the item is being dropped on
*/
void cClient::droppedOnPet(pItem pi, pNPC pet)
{
	pChar pc = currChar();
	if( !pc || !pet ) return;

	//!\todo a better hunger system
	if((pet->hunger<6) && (pi->type==ITYPE_FOOD))//AntiChrist new hunger code for npcs
	{
		pc->playSFX( 0x3A+(rand()%3) );	//0x3A - 0x3C three different eating sounds

		if(pi->poisoned)
		{
			pet->applyPoison(PoisonType(pi->poisoned));
		}

		std::string itmname;
		if( pi->getCurrentName() == "#" ) itmname = pi->getName();	//!\todo get default itemname from tile
		else itmname = pi->getCurrentName();

		pet->emotecolor = 0x0026;
		pet->emoteall("* You see %s eating %s *", true, pet->getCurrentName().c_str(), itmname.c_str() );
		pet->hunger++;
                pi->Delete();
                resetDragging();
		dragItem = NULL;
	} else
	{	//! \todo a check for pack animals
		sysmessage("It doesn't appear to want the item");
		nPackets::Sent::BounceItem pk(5);
		sendPacket(&pk);
		if (isDragging())
		{
			resetDragging();
			item_bounce5(pi);
			dragItem = NULL;
		}
	}
}

/*!
\brief item has been dropped on a guard and verifies if correct item given
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi item to be dropped (already in dragging mode)
\param npc guard pointer
*/
void cClient::droppedOnGuard(pItem pi, pNPC npc)
{
	if( !pi || ! npc) return;

	pChar pc = currChar();
	if(!pc) return;

	if( pi->getId() == 0x1da0 || pi->getId() == 0x1dae )
	{
		// This is a head of someone, see if the owner has a bounty on them
		pChar own=cSerializable::findCharBySerial(pi->getOwner());
		if(!own) return;

		if( own->questBountyReward > 0 )
		{
			// Give the person the bounty assuming that they are not the
			// same person as the reward is for
			if( pc != own )
			{
				// give them the gold for bringing the villan to justice
				pc->addGold(own->questBountyReward);
				pc->playSFX( goldsfx( own->questBountyReward ) );

				// Now thank them for their hard work
				char *temp;
				asprintf( &temp, "Excellent work! You have brought us the head of %s. Here is your reward of %d gold coins.", own->getCurrentName().c_str(), own->questBountyReward );
				npc->talk( this, temp, 0);
                                free(temp);

				// Delete the Bounty from the bulletin board
				BountyDelete(own );	//! \todo bounty system

				// xan : increment fame & karma :)
				pc->IncreaseKarma( nSettings::Reputation::getBountyKarmaGain() );
				pc->modifyFame( nSettings::Reputation::getBountyFameGain() );
			}
			else
				npc->talk(this, "You can not claim that prize scoundrel. You are lucky I don't strike you down where you stand!",0);

			// Delete the item
			pi->Delete();
			resetDragging();
			dragItem = NULL;
                        statusWindow(pc, true);
		}
	}
}

/*!
\brief item has been dropped on a beggar and verifies if correct item given
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi Item to be dropped (already in dragging mode)
\param npc Beggar
*/
void cClient::droppedOnBeggar(pItem pi, pNPC npc)
{
	pChar pc = currChar();
	if ( !pi || !pc || !npc ) return;

       	char* temp;
	if(pi->getId()!=ITEMID_GOLD)
	{
		asprintf(&temp, "Sorry %s i can only use gold", pc->getCurrentName().c_str());
		npc->talk(this, temp, false);
                free(temp);
		nPackets::Sent::BounceItem pk(5);
		sendPacket(&pk);
		if (isDragging())
		{
		        resetDragging();
			item_bounce5(pi);
			return;
		}
	}
	else
	{
	        asprintf(&temp, "Thank you %s for the %i gold!", pc->getCurrentName().c_str(), pi->amount);
		npc->talk(this, temp, false);
                free(temp);
		if(pi->amount<=100)
		{
			pc->IncreaseKarma(10);
			sysmessage("You have gained a little karma!");
		}
		else if(pi->amount>100)
		{
			pc->IncreaseKarma(50);
			sysmessage("You have gained some karma!");
		}
		resetDragging();
		dragItem = NULL;
		pi->Delete();
		statusWindow(pc, true);
	}
}

/*!
\brief item has been dropped on a trainer and verifies if correct item given
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi item to be dropped (already in dragging mode)
\param npc Trainer
*/
void cClient::droppedOnTrainer(pItem pi, pNPC npc)
{
	if(!pi) return;
	if(!npc) return;

	pChar pc = currChar();
	if(!pc) return;

	if( pi->getId() == ITEMID_GOLD )
	{ 	// They gave the NPC gold
		uint8_t sk = npc->trainingplayerin;
		npc->talk(this, "I thank thee for thy payment. That should give thee a good start on thy way. Farewell!", false);

		uint32_t sum = pc->getBody()->getSkillSum();
		int delta = npc->getTeachingDelta(pc, sk, sum);
                int amount = pi->getAmount();
		if(amount > delta) // Paid too much
		{
			pi->setAmount(amount - delta);
			nPackets::Sent::BounceItem pk(5);
			sendPacket(&pk);
			if (isDragging())
			{
			        resetDragging();
				item_bounce5(pi);
			}
		}
		else
		{
			if(amount < delta)		// Gave less gold
				delta = amount;		// so adjust skillgain
			pi->Delete();
		}
                pBody body = pc->getTrueBody(); 	//training ONLY goes to true body
		pc->setSkillBase(sk, pc->getSkillBase(sk) + delta);
		Skills::updateSkillLevel(pc, sk);
		pc->updateSkill(sk);

		pc->setTrainer(NULL);
		npc->trainingplayerin=0xFF;
		pc->playSFX( itemsfx(pi->getId()) );
		statusWindow(pc, true);
	}
	else // Did not give gold
	{
		npc->talk(this, "I am sorry, but I can only accept gold.", false);
		nPackets::Sent::BounceItem pk(5);
		sendPacket(&pk);
		if (isDragging())
		{
			resetDragging();
			item_bounce5(pi);
		}
	}
}

/*!
\brief item has been dropped on self and verifies if it was a correct item
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi item to be dropped (already in dragging mode)
*/
void cClient::droppedOnSelf(pItem pi)
{
	if(!pi) return;

	pChar pc = currChar();
	if(!pc) return;

	sLocation charpos = pc->getPosition();

	if (pi->getId() >= 0x4000 ) // crashfix , prevents putting multi-objects ni your backback
	{
		sysmessage("Hey, putting houses in your pack crashes your back and client !");
		pi->MoveTo( charpos );
		pi->Refresh();//AntiChrist
		resetDragging();
		dragItem = NULL;
		statusWindow(pc, true);
		return;
	}

	pEquippableContainer pack = pc->getBackpack();
	if (pack==NULL) // if player has no pack, put it at its feet
	{
		pi->MoveTo( charpos );
		pi->Refresh();
		resetDragging();
		dragItem = NULL;
		updateStatusWindow(pi);
	}
	else
	{
		pack->addItem(pi); // player has a pack, put it in there (in a random position)
		pc->getBody()->calcWeight();
		updateStatusWindow(pi);
		pc->playSFX( itemsfx(pi->getId()) );
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

	pEquippable epi = dynamic_cast<pEquippable> pi;

	if (!epi || (pi->getId()>>8) >= 0x40)
	{
		sysmessage("You can't wear that!");
		item_bounce6(pi);
		return;
	}

	if ( ! isMovable(pc, pi) )
	{
		item_bounce6(pi);
		return;
	}
	pNPC npc = dynamic_cast<pNPC> pck;


	if ( pck != pc && !pc->IsGM() && !npc)	// players is trying to dress another pc and he isn't a gm
	{
		sysmessage("You can't put items on other people!");
		item_bounce6(pi);
		return;
	}
	else if (!pc->isGM() && npc && (!pc->isOwnerOf(npc) || npc->npcaitype!=NPCAI_PLAYERVENDOR )) // target npc isn't this pg's personal vendor (or a GM)
	{
		item_bounce6(pi);
		return;
	}

	NotEquippableReason res = pc->canEquip(ei);
	switch(res)
	{
	case nerUnknown:
		item_bounce6(pi);
		return;
	case nerNotHumanBody:
		sysmessage("Only humans can wear that!");
		item_bounce6(pi);
		return;
	case nerInsufficientStrength:
		sysmessage("%s not strong enough to equip that!", (pck == pc) ? "You are" : "Target is");
		item_bounce6(pi);
		return;
	case nerInsufficientDexterity:
		sysmessage("%s not dexterous enough to equip that!", (pck == pc) ? "You are" : "Target is");
		item_bounce6(pi);
		return;
	case nerInsufficientIntelligence:
		sysmessage("%s not smart enough to equip that!", (pck == pc) ? "You are" : "Target is");
		item_bounce6(pi);
		return;
	case nerInsufficientSkil1:
	case nerInsufficientSkil2:
	case nerInsufficientSkil3:
		sysmessage("%s sufficient skills to equip that!", (pck == pc) ? "You lack" : "Target lacks");
		item_bounce6(pi);
		return;
	case nerCharDead:		// Char is dead. Dead people cannot wear anything :D
		sysmessage("You can't dress ghosts!");
		item_bounce6(pi);
		return;
	case nerMaleEquippingFemaleArmor:	// It wouldn't fit anyway :D
		sysmessage("%s wear female armor!", (pck == pc) ? "You can't" : "Target can't");
		item_bounce6(pi);
		return;
	case nerEquipOk:
	}
	//Now it should be equippable

	Layer layer = epi->getPossibleLayer();
	pEquippable epj = body->getLayerItem(layer);	// we get the item already in that layer, or NULL if layer is empty

	pEquippable firsthand = body->getLayerItem(layWeapon1H);
	pEquippable secondhand = body->getLayerItem(layWeapon2H);

	if ( ServerScp::g_nUnequipOnReequip )
	{
		if (pi->isWeapon() && layer == layWeapon2H)	//If equipping a 2 handed sword, we must empty both layers, so, since the 2h layer is emptied below, we empty the first :D
			if (pck->unEquip(body->getLayerItem(layWeapon1H),false) == 2) //since it is not a drag on paperdoll, we set the drag bool of unequip to false
			{       //if unequip bypass
				item_bounce6(pi);
				return;
			}
		if (pi->isWeapon() && layer == layWeapon1H && secondhand->isWeapon()) //If equipping a 1 handed sword, while already equipping a 2 handed sword, we must empty both layers, so, since the 1h layer is emptied below, we empty the second :D
			if (pck->unEquip(body->getLayerItem(layWeapon2H),false) ==2) //since it is not a drag on paperdoll, we set the drag bool of unequip to false
			{       // if unequip bypass
				item_bounce6(pi);
				return;
			}

		if (epj && pck->unEquip(epj,true) == 2)
		{	// if unequip bypass
			item_bounce6(pi);
			return;
		}
		if (pck->equip(epi, true) == 2)
		{       // equip bounce, this time
			item_bounce6(pi);
			return;
		}
		pc->playSFX( itemsfx(pi->getId()) );
	}
	else
	{
		if (pi->isWeapon() &&					// If pi is a weapon...
		   ((layer == layWeapon2H && firsthand) ||		// ..a two handed weapon while another one-handed weapon present or...
		   (layer == layWeapon1H && secondhand->isWeapon()))	// ...a one-handed weapon when a 2 handed weapon is already equipped....
		{							// ...it must bounce because we are not allowed to automatically deequip them
			item_bounce6(pi);
			return;
		}

		if (epj) // if something already equipped in the same layer, bounce (we are not allowed to automatically deequip them)
		{
			item_bounce6(pi);
			return;
		}
		if (pck->equip(epi, true) == 2)
		{       // if script bypasses equip, bounce
			item_bounce6(pi);
			return;
		}
		pc->playSFX( itemsfx(pi->getId()) );
	}

	NxwSocketWrapper sws;
	sws.fillOnline( pi );
	for( sws.rewind(); !sws.isEmpty(); sws++ )
	{
		nPackets::Sent::DeleteObj pk(pi);
		sws->sendPacket(&pk);
	}

	//! \todo the sendpacket stuff
	wearIt(pi);

	NxwSocketWrapper sw;
	sw.fillOnline( pck, false );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		cClient j = sw.getClient();
		if( j!=NULL ) j->wornitems(pck );
	}


	pc->getBody()->calcWeight();
	updateStatusWindow(pi);
}

/*!
\brief holds some statements that were COPIED some 50 times
\param pi item to be bounced back (already in dragging mode)
*/

void cClient::item_bounce3(const pItem pi)
{
	if(!pi) return;

	pEquippable equipitem = dynamic_cast<pEquippable> pi;
	if (equipitem && equipitem->getOldLayer())
	{
		pBody body = dynamic_cast<pBody> equipitem->getOldContainer();  //If it was equipped, old container was a body
		pChar pc = body->getChar();
		if( body->equip(equipitem, true) == 1)
		{
			equipitem->setOldLayer(0); //to avoid infinite loop if bouncing again
			pack_item(pi, pc->getBackpack()); // If reequip canceled due to script bypass, dump item to the backpack
		}
	}
	else
	{
		pi->setContainer( pi->getOldContainer() );
		pi->setPosition( pi->getOldPosition() );
	}
	dragItem = NULL;
	resetDragging();

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
	nPackets::Sent::BounceItem pk(5);
	sendPacket(&pk);
	if ( isDragging() )
	{
		resetDragging();
		item_bounce4(pi);
	}
}

/*------------------------------------------------------------------------------
                             TRADING METHODS
------------------------------------------------------------------------------*/
/*!
\brief opensa buy gump on client
\author Chronodt
\param npc vendor whose goods client is trying to purchase
*/
bool buyShop(pNPC npc)
{
	if(!npc) return false;
	pEquippableContainer buyRestockContainer=NULL, buyNoRestockContainer=NULL;
	buyRestockContainer = dynamic_cast<pEquippableContainer>(npc->getBody()->getLayerItem(layNPCBuyR));
	buyNoRestockContainer = dynamic_cast<pEquippableContainer>(npc->getBody()->getLayerItem(layNPCBuyN));
	if (!buyRestockContainer || !buyNoRestockContainer ) return false;

	sendchar(pc, false); // Send the NPC again to make sure info is current. (OSI does this we might not have to)

	nPackets::Sent::ContainerItem pk1;
	buyRestockContainer->lockItemsMutex();
	for(ItemSLIst::const_iterator it = buyRestockContainer->getItems().begin(); it != buyRestockContainer->getItems().end() )
	{
		if ( ! (*it) ) continue;
		pk1.addItem(*it);
	}
	buyRestockContainer->unlockItemsMutex();

	nPackets::Sent::BuyWindow pk2(buyRestockContainer);

	nPackets::Sent::ContainerItem pk3;
	buyNoRestockContainer->lockItemsMutex();
	for(ItemSLIst::const_iterator it = buyNoRestockContainer->getItems().begin(); it != buyNoRestockContainer->getItems().end() )
	{
		if ( ! (*it) ) continue;
		pk3.addItem(*it);
	}
	buyNoRestockContainer->unlockItemsMutex();

	nPackets::Sent::BuyWindow pk4(buyNoRestockContainer);

	//! \todo verify if we have to send only 2 packets with the information of both containers instead
	client->sendPacket(&pk1);
	client->sendPacket(&pk2);
	client->sendPacket(&pk3);
	client->sendPacket(&pk4);

	nPackets::Sent::OpenGump pk(pc->getSerial(), 0x0030);
	client->sendPacket(&pk);

	//! \todo check second argument
	client->statusWindow(curr,true); // Make sure the gold total has been sent.

	return true;
}

/*!
\brief concludes buying of items
\author Unknown, updated to hypnos Chronodt (24/2/04)
\param npc vendor whose goods player is buying
\param allitemsbought list of items selected from player (layer, pItem and amount for each item)
*/
void cClient::buyaction(pNpc npc, std::list< boughtitem > &allitemsbought)
{
	int i, j;

	int playergoldtotal;

	int tmpvalue=0; // Fixed for adv trade system -- Magius(CHE) 

	pChar pc = currChar();
	if(!pc) return;

	pItem pack = pc->getBackpack();
	if(!pack) return;

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
		// Fixed for adv trade system -- Magius(CHE) 
		tmpvalue = iter.item->value;
		tmpvalue = iter.item->calcValue(tmpvalue);
		if (SrvParms->trade_system==1)
			tmpvalue=calcGoodValue(this,iter.item,tmpvalue,0);
		goldtotal+=iter.amount*tmpvalue;
		// End Fix for adv trade system -- Magius(CHE) 
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
							SWITCH_FALLOUT;
							return;
					}
				}
			}
		}
	}

	if (clear)
	{
                nPackets::Sent::ClearBuyWindow pk(npc);
	        sendPacket(&pk);
	}
	pc->getBody()->calcWeight();
	statusWindow(pc,true);  //!< \todo check second argument
}

void cClient::sellShop(pNPC npc)
{
	pChar pc = currChar();
	if(!pc || !npc) return;

	pEquippableContainer vendorpack= dynamic_cast<pEquippableContainer> (npc->getBody()->getLayerItem(laySell));
	if(!pp) return;

	pEquippableContainer pack= pc->getBackpack();
	if(!pack) return;

	// Pause the client only after the validity tests are completed
	// else we can have a deadlock on the client
	pause();

	uint16_t itemssent = 0;		// number of items sent to client;

	nPackets::Sent::SellList pk(npc);

	vendorpack->lockItemsMutex();
	pack->lockItemsMutex();

	for(ItemSList::const_iterator it = vendorpack->getItems().begin(); it != vendorpack->getItems().end() )
	{
		if ( ! (*it) ) continue;
		for(ItemSList::const_iterator it2 = pack->getItems().begin(); it2 != pack->getItems().end() )
		{
			if ( ! (*it2) ) continue;

			if (itemssent > nSettings::Server::getMaximumSoldItems()) break;

			pItem pj1 = s_pack.getItem();
			if ( ! (*it2) ) continue;

			if ((*it2)->getId()==(*it)->getId()  && (*it2)->type==(*it)->type &&
			   (SrvParms->sellbyname==0 || (SrvParms->sellbyname==1 && (*it2)->getCurrentName() == (*it)->getCurrentName())))
			{
				++itemssent;
				pk.addItem(pi);
			}
		}
	}

	pack->unlockItemsMutex();
	vendorpack->unlockItemsMutex();

	if (itemssent <= nSettings::Server::getMaximumSoldItems()) //With too many items, server crashes
	{
		if (!itemssent) sendPacket(&pk);
		else npc->talkAll("Thou dont posses nothing of interest to me.", false);
	}
	else npc->talkAll("Sorry i cannot take so many items.."), false);

	resume();
	return;
}

void cClient::sellaction(pNpc npc, std::list< boughtitem > &allitemssold)
{
	if(!npc) return;

	pChar pc=currChar();
	if(!pc) return;

	pItem np_a=NULL, np_b=NULL, np_c=NULL;
	int i, amt, value=0, totgold=0;

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
				pItem pi=si2.getItem();
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
						value=calcGoodValue(this,pSell,value,1); // Fixed for adv trade --- by Magius(CHE) 
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
					nPackets::Sent::DeleteObj pk(pSell);
					sw->sendPacket(&pk);
				}

				pSell->setContainer( np_b );
				if (pSell->amount!=amt)
					Commands::DupeItem(this, pSell, pSell->amount-amt);
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

/*!
\brief Finding trade session between this and another client knowing "this" tradecontainer
\author Chronodt (14/8/04)
*/

sSecureTradeSession cClient::findTradeSession(pContainer tradecontainer)
{
	std::list<sSecureTradeSession>::iterator it = SecureTrade.begin();
	for(;it!= SecureTrade.end(); ++it) if (it->container1 == tradecontainer) return *it;
	sSecureTradeSession session;
	session.tradepartner = NULL;
	return session;
}

/*!
\brief Finds the trade session between "this" and another client knowing the other client
\author Chronodt (14/8/04)
*/

sSecureTradeSession cClient::findTradeSession(pClient tradeclient)
{
	std::list<sSecureTradeSession>::iterator it = SecureTrade.begin();
	for(;it!= SecureTrade.end(); ++it) if (it->tradepartner == tradeclient) return *it;
	sSecureTradeSession session;
	session.tradepartner = NULL;
	return session;
}


/*!
\brief The Trade must go on ...
\author Chronodt (14/8/04)
*/

pContainer cClient::tradestart(pClient targetClient)
{
	if ( ! targetClient) return NULL;
	pPC pc1 = currChar();
	pPC pc2 = targetClient->currChar();
	if ( ! pc1 || ! pc2 || pc1->isDead() || pc2->isDead() ||  pc1->distFrom( pc2 ) > 5 ) return NULL;

	//We just need 2 containers with bullettin board visual id, not a real bulletin board
	//and maybe we don't even need that... we could use any container (not verified)
	//!\todo when changing createfromscripts, use any container you wish here :D
	pContainer cont1 = item::CreateFromScript( "$item_a_bulletin_board" );
	pContainer cont2 = item::CreateFromScript( "$item_a_bulletin_board" );
	if ( ! cont1 || ! cont2 ) return NULL;

//	cont1->setPosition(26, 0, 0);
//	cont2->setPosition(26, 0, 0);
	cont1->setContainer(pc1->getBody());
	cont2->setContainer(pc2->getBody());
	sSecureTradeSession session1, session2;

	session1.tradepartner = targetClient;
	session1.container1 = cont1;
	session1.container2 = cont2;
	session1.status1 = false;
	session1.status2 = false;

	session2.tradepartner = this;
	session2.container1 = cont2;
	session2.container2 = cont1;
	session2.status1 = false;
	session2.status2 = false;

	addTradeSession(session1);
	targetClient->addTradeSession(session2);

	//Now showing container to both clients
	showItemInContainer(cont1);
	showItemInContainer(cont2);
	targetClient->showItemInContainer(cont1);
	targetClient->showItemInContainer(cont2);



	nPackets::Sent::SecureTrading pk1(0, pc2, cont1->getSerial(), cont2->getSerial());
	sendPacket(&pk1);

	nPackets::Sent::SecureTrading pk2(0, pc1, cont2->getSerial(), cont1->getSerial());
	targetClient->sendPacket(&pk2);
	return cont1;
}

void cClient::sendtradestatus(sSecureTradeSession &session)  //takes clients from containers' owners
{
	if(!session.tradepartner) return;

	nPackets::Sent::SecureTrading pk1(0x02, session.container1->getSerial(), (session.status1) ? 1: 0, (session.status2) ? 1: 0);
	nPackets::Sent::SecureTrading pk2(0x02, session.container2->getSerial(), (session.status2) ? 1: 0, (session.status1) ? 1: 0);
	sendPacket(&pk1);
	session.tradepartner->sendPacket(&pk2);
}

void cClient::dotrade(sSecureTradeSession &session)
{
	if(!session.tradepartner) return;

	pPC pc1 = currChar();
	pPC pc2 = session.tradepartner->currChar();

	if(!pc1) return;
	if(!pc2) return;

	pContainer bp1 = pc1->getBackpack();
	pContainer bp2 = pc2->getBackpack();

	if(!bp1) return;
	if(!bp2) return;

	bool trade = true;
	if (!session.status1 || !session.status2)
	{
		//If the trade is not accepted, then give items back to original owners
		pPC pc_dummy = NULL;
		pc_dummy = pc1;
		pc1 = pc2;
		pc2 = pc_dummy;
		trade = false;	//items are sent back to their owners
	}

	//Player1 items go to player2

	NxwItemWrapper si;
	si.fillItemsInContainer( session.container1, false );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi = si.getItem();
		if ( ! pi )
			continue;
		if (trade)	//Event ontransfer should be called only if a trade is done, not on trade cancel
		{
			pFunctionHandle evt = pi->getEvent(cItem::evtItmOnTransfer);
			if ( evt )
			{
				cVariantVector params = cVariantVector(3);
				params[0] = pi->getSerial(); params[1] = pc1->getSerial();
				params[2] = pc2->getSerial();
				evt->setParams(params);
				evt->execute();
				if ( evt->isBypassed() )	//If bypass, send item back to original owner
				{
					pi->setContainer( bp1 );
					pi->setPosition( 50+(rand()%80), 50+(rand()%80), 9);
					pc1->getClient()->showItemInContainer(pi);
					pi->Refresh();
					continue;
				}
			}
		}
		pi->setContainer( bp2 );
		pi->setPosition( 50+(rand()%80), 50+(rand()%80), 9);
		pc2->getClient()->showItemInContainer(pi);
		pi->Refresh();
	}

	si.clear();
	si.fillItemsInContainer( session.container2, false );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi = si.getItem();
		if ( ! pi )
			continue;
		if (trade)
		{
			pFunctionHandle evt = pi->getEvent(cItem::evtItmOnTransfer);
			if ( evt )
			{
				cVariantVector params = cVariantVector(3);
				params[0] = pi->getSerial(); params[1] = pc2->getSerial();
				params[2] = pc1->getSerial();
				evt->setParams(params);
				evt->execute();
				if ( evt->isBypassed() )	//If bypass, send item back to original owner
				{
					pi->setContainer( bp2 );
					pi->setPosition( 50+(rand()%80), 50+(rand()%80), 9);
					pc2->getClient()->showItemInContainer(pi);
					pi->Refresh();
					continue;
				}

			}
		}
		pi->setContainer( bp1 );
		pi->setPosition( 50+(rand()%80), 50+(rand()%80), 9);
		pc1->getClient()->showItemInContainer(pi);
		pi->Refresh();
	}

	sSecureTradeSession session2 = findTradeSession(session.tradepartner);

	removeTradeSession(session);
	session.tradepartner->removeTradeSession(session2);
	session.container1->Delete();
	session.container2->Delete();
}

void cClient::endtrade(sSecureTradeSession &session)
{
	if(!session.tradepartner) return;

	pChar pc1=currChar();
	if(!pc1) return;

	pChar pc2=session.tradepartner->currChar();
	if(!pc2) return;

	pItem bp1= pc1->getBackpack();
	if(!bp) return;

	pItem bp2= pc2->getBackpack();
	if(!bp2) return;

	nPackets::Sent::SecureTrading pk1(0x01, session.container1->getSerial(), 0, 0);
	sendPacket(&pk1);

	nPackets::Sent::SecureTrading pk2(0x01, session.container2->getSerial(), 0, 0);
	session.tradepartner->sendPacket(&pk2);

	NxwItemWrapper si;
	si.fillItemsInContainer( session.container1, false );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pj=si.getItem(); //</Luxor>
		if ( ! pj )
			continue;
			
		bp1->AddItem(pj);

		if (s1!=INVALID && pj) pj->Refresh();
	}

	NxwItemWrapper si2;
	si2.fillItemsInContainer( session.container2, false );
	for( si2.rewind(); !si2.isEmpty(); si2++ )
	{
		pItem pj=si2.getItem();
		if ( ! pj )
			continue;
		
		bp2->AddItem(pj);

		if (s2!=INVALID && pj) pj->Refresh();
	}

	sSecureTradeSession session2 = findTradeSession(session.tradepartner);
	removeTradeSession(session);
	session.tradepartner->removeTradeSession(session2);
	session.container1->Delete();
	session.container2->Delete();
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
		sendMidi(midiarray[i]);
	}
}


/*!
\brief Play a sound effect
\author Flameeyes
\todo Fix the set support after get working new sets
*/
void cClient::playSFX(uint16_t sound, bool onlyMe, bool repeat)
{
	nPackets::Sent::SoundFX pk(sound, pc->getPosition(), repeat);

	if(onlyMe) {
		client->send(&pk);
		return;
	}

	//!\todo Missing way to find out
/*
	NxwSocketWrapper sw;
	sw.fillOnline( pc, false );

	for( sw.rewind(); !sw.isEmpty(); sw++ ) {
		pClient ps=sw.getClient();
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


	uint32_t i, j;
	int match;
	char sect[512];

	char name[30] = {0,};	// it **IS** important to 0 out the remaining gaps
	strcpy(name, pc->getCurrentName().c_str());

	// len+font+color+type = same postion for non unicode and unicode speech packets
	// but 8 ... x DIFFER a lot for unicode and non unicode packets !!!

	if ( speech.getMode() == 0x09 && pc->CanBroadcast() )
	{
		broadcast(speech);
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
	pFunctionHandle evt = pc->getEvent(cChar::evtChrOnSpeech);
	if ( evt )
	{
		cVariantVector params = cVariantVector(3);
		params[0] = pc->getSerial(); params[1] = speech;
		evt->setParams(params);
		evt->execute();
		if ( evt->isBypassed() )
			return;
	}
	//</Luxor>

	//
	// Process speech which should not be send/echoed to others
	//


	//!\todo revise from here


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
		sysmessage("You have been squelched.");
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
	talk.color = ShortFromCharPtr(buffer[socket]+4);
	talk.font = ShortFromCharPtr(buffer[socket]+6);
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
		pClient ps=sw.getClient();
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

		pFunctionHandle evt = a_pc->getEvent(evtChrOnHearPlayer);
		if ( evt )
		{
			cVariantVector params = cVariantVector(2);
			params[0] = a_pc->getSerial(); params[1] = pc->getSerial();
			params[2] = ghost;
			evt->setParams(params);
			evt->execute();
		}
	
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

	// Check for multi-talks
	if ( pc->getMulti() )
	{
		pBoat pb = dynamic_cast<pBoat>(pc->getMulti());
		pHouse ph = dynamic_cast<pHouse>(pc->getMulti());
		
		if ( pb && pb->doSpeech(this, speech) )
			return;
		
		if ( ph && ph->doSpeech(this, speech) )
			return;
	}
	
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

		responsevendor(socket, pc_found);

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

				if (!(strcmp("TRG", script1))) // Added by Magius(CHE) 
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

void cClient::broadcast(cSpeech &speech) // GM Broadcast (Done if a GM yells something)
{

	pPC pc= currChar();
	if ( ! pc ) return;

	//Chronodt: i don't know why does it do this...
	speech.setFont = (speech.getFont()<<8)|(pc->fonttype%256);	// use font ("not only") from  client

	NxwSocketWrapper sw;
	sw.fillOnline();

	if(!(pc->unicode))
	{
		nPackets::Sent::Speech pk(speech);
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			pClient i=sw.getSocket();
			i->sendPacket(&pk);
		}
	}
	else
	{
		nPackets::Sent::UnicodeSpeech pk(speech);
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			pClient i=sw.getSocket();
			i->sendPacket(&pk)
		}
	}
}


void cClient::sysmessage(const char *txt, ...) // System message (In lower left corner)
{
	va_list argptr;
	char *msg;
	va_start( argptr, txt );
	vsprintf( &msg, txt, argptr );
	va_end( argptr );

	uint32_t spyTo = clientInfo[s]->spyTo;
	if( spyTo!=INVALID )
	{ //spy client
		pChar pc=cSerializable::findCharBySerial( spyTo );
		if( pc )
		{
			pClient gm = pc->getClient();
			if( gm!=NULL )
				gm->sysmessage( "spy %s : %s", pc->getCurrentName().c_str(), msg );
			else
				clientInfo[s]->spyTo=INVALID;
		}
		else
			clientInfo[s]->spyTo=INVALID;
	}

	cSpeech speech(std::string(msg));	//we must use string constructor or else it is supposed to be an unicode packet
	speech.setColor(0x387);
	speech.setFont(0x03);		// normal font
	speech.setMode(0x06);		// label

	nPackets::Sent::UnicodeSpeech pk(speech);
	sendPacket(&pk);

	free(msg);
}

void cClient::sysmessage(uint16_t color, const char *txt, ...) // System message (In lower left corner)
{
	va_list argptr;
	char *msg;
	va_start( argptr, txt );
	vasprintf( &msg, txt, argptr );
	va_end( argptr );
	uint16_t ucl = ( strlen ( msg ) * 2 ) + 2 ;

	cSpeech speech(std::string(msg));	//we must use string constructor or else it is supposed to be an unicode packet
	speech.setColor(color);
	speech.setFont(0x03);		// normal font
	speech.setMode(0x06);		// label

	nPackets::Sent::UnicodeSpeech pk(speech);
	sendPacket(&pk);

	free(msg);
}

void cClient::sysbroadcast(char *txt, ...) // System broadcast in bold text
{
	va_list argptr;
	char *msg;
	va_start( argptr, txt );
	vasprintf( msg, txt, argptr );
	va_end( argptr );

	cSpeech speech(std::string(msg));	//we must use string constructor or else it is supposed to be an unicode packet
	speech.setColor(0x084d);
	speech.setFont(0x00);		// bold text
	speech.setMode(0x06);		// label

	nPackets::Sent::UnicodeSpeech pk(speech);

	NxwSocketWrapper sw;
	sw.fillOnline();
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient ci = sw.getClient();
		if ( ci ) ci->sendPacket(&pk);
	}
	
	free(msg);
}

/*!
\brief Tells the client the current (game) time
*/
void cClient::telltime()
{
	static const char strTIs[]		= "It is";
	static const char strTQuarterPast[]	= "It is a quarter past";
	static const char strTHalfPast[]	= "It is half past";
	static const char strTQuarterTill[]	= "It is a quarter till";
	static const char strHours[12][] =
	{
		strNull,
		"one o'clock",
		"two o'clock",
		"three o'clock",
		"four o'clock",
		"five o'clock",
		"six o'clock",
		"seven o'clock",
		"eight o'clock",
		"nine o'clock",
		"ten o'clock",
		"eleven o'clock"
	};
	static const char strAfternoon[]	= "in the afternoon.";
	static const char strEvening[]		= "in the evening.";
	static const char strNight[]		= "at night.";
	static const char strMorning[]		= "in the morning.";

	const char *prefix = NULL;
	const char *strhour = NULL;
	const char *suffix = NULL;
	
	uint8_t hour = Calendar::g_nHour % 12;
	if (hour==0) hour = 12;
	bool ampm = (Calendar::g_nHour>=12) ? 1 : 0;
	uint8_t minute = Calendar::g_nMinute;
	uint8_t lhour = hour;

	if ((minute>=0)&&(minute<=14))
		prefix = strTIs;
	else if ((minute>=15)&&(minute<=30))
		prefix = strTQuarterPast;
	else if ((minute>=30)&&(minute<=45))
		prefix = strTHalfPast;
	else
	{
		prefix = strTQuarterTill;
		lhour++;
		if (lhour==0) lhour=12;
	}
	
	if ( lhour == 12 ) // No AM/PM stuff and no array stuff
	{
		if ( ampm )
			sysmessage("%s midnight.", prefix);
		else
			sysmessage("%s noon.", prefix);
		return;
	}
	
	strhour = strHours[lhour];
	
	if (ampm)
	{
		if ((lhour>=1)&&(lhour<6))
			suffix = strAfternoon;
		else if ((lhour>=6)&&(lhour<9))
			suffix = strEvening;
		else
			suffix = strNight;
	}
	else
	{
		if ((lhour>=1)&&(lhour<5))
			suffix = strNight;
		else
			suffix = strMorning;
	}
	
	sysmessage("%s %s %s", prefix, strhour, suffix);
}

/*!
\brief Pauses the client

This function pauses the client, making it not do stuff until the resume packet
is received.

\see nPackets::Sent::PauseClient
\see cClient::resume()
*/
void cClient::pause()
{
	static nPackets::Sent::PauseClient pk(0x00);
	sendPacket(&pk);
}

/*!
\brief Pauses the client

This function resumes the client, making it take a look to what was received
after the pause packet (sent by the above function)

\see nPackets::Sent::PauseClient
\see cClient::pause()
*/
void cClient::resume()
{
	static nPackets::Sent::PauseClient pk(0x01);
	sendPacket(&pk);
}


/*------------------------------------------------------------------------------
				TARGETING
------------------------------------------------------------------------------*/


void cClient::sendTarget(pSerializable source, processTarget callback)
{
	target.callback = callback;
	target.type = ttAll;
	nPackets::Sent::TargetingCursor pk(source, true);
	sendPacket(&pk);
}

void cClient::sendObjectTarget(pSerializable source, processTarget callback)
{
	target.callback = callback;
	target.type = ttObject;
	nPackets::Sent::TargetingCursor pk(source, false);
	sendPacket(&pk);
}

void cClient::sendCharTarget(pSerializable source, processTarget callback)
{
	target.callback = callback;
	target.type = ttChar;
	nPackets::Sent::TargetingCursor pk(source, false);
	sendPacket(&pk);
}

void cClient::sendItemTarget(pSerializable source, processTarget callback)
{
	target.callback = callback;
	target.type = ttItem;
	nPackets::Sent::TargetingCursor pk(source, false);
	sendPacket(&pk);
}

void cClient::sendLocationTarget(processTarget callback)
{
	target.callback = callback;
	target.type = ttLocation;
	nPackets::Sent::TargetingCursor pk(source, true);
	sendPacket(&pk);
}

