/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "networking/network.h"
#include "magic.h"
#include "house.h"
#include "npcai.h"
#include "mount.h"
#include "boats.h"
#include "map.h"
#include "inlines.h"
#include "skills/skills.h"
#include "fishing.h"

bool cItem::usableWhenLockedDown(pPc pc)
{
//! \todo this function
	switch(type)
        {
	case ITYPE_CONTAINER:		// These items are always usable when locked down
	case ITYPE_LOCKED_ITEM_SPAWNER:
	case ITYPE_SPELLBOOK:
	case ITYPE_MAP:
	case ITYPE_BOOK:
	case ITYPE_DOOR:
	case ITYPE_LOCKED_DOOR:
	case ITYPE_RESURRECT:
	case ITYPE_UNLOCKED_CONTAINER:
	case ITYPE_LOCKED_CONTAINER:
	case ITYPE_TRASH:
	case ITYPE_DECIPHERED_MAP:
		return true;

	case ITYPE_KEY:			// these items can be used only by owner or house owner/coowner/friend when locked down
		return pc == owner || ( inHouse && inHouse->isFriend(pc) );

	}
	return false;
}

/*!
\brief single click on item
\return bool
\param client client of player who clicked the item
*/

void cItem::singleClick(pClient client )
{
	int32_t amt = 0, wgt;
	char itemname[100];
	char *temp, *temp2;

	pChar pj;

	if ( events[evtItmOnClick] ) {
		cVariantVector params = cVariantVector(2);
		params[0] = getSerial(); params[1] = client->currChar()->getSerial();
		events[evtItmOnClick]->setParams(params);
		events[evtItmOnClick]->execute();
		if ( events[evtItmOnClick]->isBypassed() )
			return;
	}
	
	getName( itemname );

	if ( type == ITYPE_SPELLBOOK )
	{
		asprintf( &temp, "[%i spells]", countSpellsInSpellBook() );
		itemmessage(client, temp, this, 0x0481);
		free(temp);
	}

	if ( CanSeeSerials() )
	{
		if (amount > 1)
			sprintf( &temp, "%s [%x]: %i", itemname, getSerial(), amount);
		else
			sprintf( &temp, "%s [%x]", itemname, getSerial());

		itemmessage(client, temp, this);
		free(temp);
		return;
	}

	// Click in a Player Vendor item, show description, price and return
	if (!isInWorld() && cSerializable::isItemSerial(getContSerial()))
	{
		pItem cont = getContainer();
		if( cont ) {
			pj = cont->getPackOwner();
			if( pj )
			{
				if( pj->npcaitype==NPCAI_PLAYERVENDOR )
				{
					if ( !creator.empty() && madewith>0)
						asprintf( &temp2, "%s %s by %s", vendorDescription.c_str(), ::skillinfo[madewith - 1].madeword, creator.c_str());
					else
						asprintf( &temp2, "%s", vendorDescription.c_str() );

					asprintf( &temp, "%s at %igp", temp2, value );

					itemmessage(client, temp, this);
					free(temp); free(temp2);
					return;
				}
			}
		}
	}

	// From now on, we will build the message into temp, and let itemname with just the name info
	// Add amount info.
	if (!pileable || amount == 1)
		asprintf( &temp, "%s", itemname);
	else
		if (itemname[strlen(itemname) - 1] != 's') // avoid iron ingotss : x
			asprintf( &temp, "%ss : %i", itemname, amount);
		else
			asprintf( &temp, "%s : %i", itemname, amount);

	// Add creator's mark (if any)
	if ( !creator.empty() && madewith > 0) {
		asprintf( &temp2, "%s %s by %s", temp, ::skillinfo[madewith - 1].madeword, creator.c_str());
		free(temp);
		temp = temp2;
	}

	if (type == ITYPE_WAND || type == ITYPE_ITEMID_WAND || type == ITYPE_FIREWORKS_WAND ) // Fraz
	{
		if (!(strcmp(getSecondaryNameC(), getCurrentName().c_str())))
		{
			uint32_t n;

			if( type == ITYPE_WAND) n = morez;
			else n = morex;

			asprintf( &temp2, "%s %i ", temp, n, (n == 1)?  "charge" : "charges");
		}
	}

	// Corpse highlighting...Ripper
	if (corpse==1)
	{
		if(more2==1)
			itemmessage( client,"[Innocent]",this, 0x005A);
		else if(more2==2)
			itemmessage( client,"[Criminal]",this, 0x03B2);
		else if(pi->more2==3)
			itemmessage( client,"[Murderer]",this, 0x0026);
	}  // end highlighting
	// Let's handle secure/locked down stuff.
	if (magic == 4 && type != ITYPE_DOOR && type != ITYPE_GUMPMENU)
	{
		if (secureIt !=1)
			itemmessage( client, "[locked down]", this, 0x0481);
		if (secureIt == 1 && magic == 4)
			itemmessage( client, "[locked down & secure]", this, 0x0481);
	}

	itemmessage(client, temp, this);
	free(temp); free(temp2);

	/*! \todo This should be changed to a virtual function */
#if 0
	// Send the item/weight as the last line in case of containers
	if (type == ITYPE_CONTAINER || type == ITYPE_UNLOCKED_CONTAINER || type == ITYPE_NODECAY_ITEM_SPAWNER || type == ITYPE_TRASH)
	{
		wgt = (int32_t) weights::LockeddownWeight(this, &amt); // get stones and item #, LB
		if (amt>0)
		{
			sprintf( temp2, "[%i items, %i stones]", amt, wgt);
			itemmessage( client, temp2, this);
		}
		else
			itemmessage( client, "[0 items, 0 stones]", this);
	}
	*/
#endif
}

#define CASE(FUNC) else if( ( FUNC() ) )
#define CASEOR(A, B) else if( (A())||(B()) )
static void doubleclick_itemid( pClient client, pChar pc, pItem pi, pContainer pack );

/*!
\brief Double click
\author Ripper, rewrite by Endymion, then by Chronodt (1/2/2004)
\param client client of player dbclick
\note Completely redone by Morrolan 20-07-99
\warning I use a define CASE for make more readable the code, if you change name of pItem pi chage also the macro
\note pItem pi removed since merging with cItem, so the macro has been changed too (1/2/2004)
\todo review when sets redone
\todo los
*/
void cItem::doubleClick(pClient client)
{
	if (!client) return;
	pChar pc = client->currChar();
	if ( ! pc ) return;

	if ( events[evtItmOnDoubleClick] ) {
		cVariantVector params = cVariantVector(2);
		params[0] = getSerial(); params[1] = pc->getSerial();
		events[evtItmOnDoubleClick]->setParams(params);
		events[evtItmOnDoubleClick]->execute();
		if ( events[evtItmOnDoubleClick]->isBypassed() )
			return;
	}
	
	if (!checkItemUsability(pc, ITEM_USE_DBLCLICK))
		return;

	sLocation charpos= pc->getPosition();

	// Luxor: cannot use items if under invisible spell
	// Chronodt: 1/2/2004 added a server parameter check to allow servers to let items be used when invisible
	if (pc->IsHiddenBySpell() && !nSettings::Actions::canUseItemsWhenInvisible()) return;

	if ( !pc->IsGM() && pc->objectdelay >= getClockmSecs() )
	{
		client->sysmessage("You must wait to perform another action.");
		return;
	}
	else
		pc->objectdelay = nSettings::Actions::getObjectsDelay() * SECS + getClockmSecs();

	///MODIFY, CANT CLICK ITEM AT DISTANCE >2//////////////
	if ( (pc->distFrom(pi)>2) && !pc->IsGM() && !(pc->hasTelekinesis()) ) //Luxor: let's check also for the telekinesys spell
	{
		client->sysmessage( "Must be closer to use this!");
		pc->objectdelay=0;
		return;
	}


	pContainer pack = pc->getBackpack();
	if(!pack) return;

	//<Luxor>: Circle of transparency bug fix
	pContainer cont;
	sLocation dst;

	cont = getOutMostCont();

	if(cont->isInWorld()) {
		dst = cont->getPosition();
	} else {
		pChar pg_dst = cSerializable::findCharBySerial( cont->getContSerial() );
		//!\todo verify if doing a getContSerial to a body returns a char
		if(!pg_dist) return;

		dst = pg_dst->getPosition();
	}

	sLocation charPos = pc->getPosition();
	charPos.z = dst.z;
	charPos.dispz = dst.dispz;

	if ( !pc->IsGM() && !lineOfSight( charPos, dst ) && !pc->hasTelekinesis() ) {
		client->sysmessage(  "You cannot reach the item" );
		return;
	}
	//</Luxor>

	pChar itmowner = getPackOwner();

	if(!isInWorld()) {
		if (cSerializable::isItemSerial(getContSerial()) && type != ITYPE_CONTAINER)
		{// Cant use stuff that isn't in your pack.

			if ( itmowner && tmowner->getSerial()!=pc->getSerial() )
					return;
		}
		else
			if (cSerializable::isCharSerial(getContSerial()) && type!=(uint32_t)INVALID)
			{// in a character.
				pChar wearedby = cSerializable::findCharBySerial(getContSerial());
				if ( wearedby )
					if (wearedby->getSerial()!=pc->getSerial() && layer!=LAYER_UNUSED_BP && type!=ITYPE_CONTAINER)
						return;
			}
	}

	if ((magic==4) && (secureIt==1))
	{
		if (!pc->isOwnerOf(this) || !pc->IsGMorCounselor())
		{
			client->sysmessage( "That is a secured chest!");
			return;
		}
	}

	if ((magic == 4) && !usableWhenLockedDown())  // Chronodt: without this last check, locked down chests could not be opened by nonowners even if unlocked
	{
		client->sysmessage( "That item is locked down.");
		return;
	}

	if (pc->dead && >type!=ITYPE_RESURRECT) // if you are dead and it's not an ankh, FORGET IT!
	{
		client->sysmessage("You may not do that as a ghost.");
		return;
	}
	else if (!pc->IsGMorCounselor() && layer!=0 && !pc->IsWearing(this) && layer != LAYER_BACKPACK)
	{// can't use other people's things!
		client->sysmessage("You cannot use items equipped by other players.");
		return;
	}


	// BEGIN checking objects that we force an object delay for (std objects)
	// start trigger stuff
	if (trigger > 0)
	{
		if (trigtype == 0)
		{
			if ( TIMEOUT( disabled ) ) // changed by Magius(CHE) 
			{
				triggerItem(client, TRIGTYPE_DBLCLICK); // if players uses trigger
				return;
			}
			else
			{
				if ( disabledmsg!=NULL )
					client->sysmessage("%s", disabledmsg->c_str());
				else
					client->sysmessage("That doesnt seem to work right now.");
				return;
			}
		}
		else
		{
			client->sysmessage( "You are not close enough to use that.");
			return;
		}
	}

	// check this on trigger in the event that the .trigger property is not set on the item
	// trigger code.  Check to see if item is envokable by id
	else if (checkenvoke( getId() ))
	{
		pc->envokeitem = getSerial();
		pc->envokeid = getId();

                //! \todo REVISE WHEN TARGETS REDONE!!

		pTarget targ = clientInfo[s]->newTarget( new cObjectTarget() );
		targ->code_callback=target_envoke;
		targ->send( client );
		client->sysmessage( "What will you use this on?");
		return;
	}
	// END trigger stuff

	doubleClicked(client);
}

void cItem::doubleClicked(pClient client)
{
	int los = 0;

	pTarget targ = NULL;
	
	pPC pc = client->currChar();
	switch (type)
	{
	case ITYPE_RESURRECT:
		// Check for 'resurrect item type' this is the ONLY type one can use if dead.
		if (pc->isDead())
		{
			pc->resurrect();
			client->sysmessage("You have been resurrected.");
		}
		else
		{
			client->sysmessage("You are already living!");
		}
		return;
	case ITYPE_BOATS:
		if ( type2 != 3 ) return;
		
		switch( getId() & 0xFF ) {
			case 0x84:
			case 0xD5:
			case 0xD4:
			case 0x89:
				Boats->PlankStuff(client, this);
				break;
			default:
				client->sysmessage( "That is locked.");
				break;
		}
		return;
	case ITYPE_NODECAY_ITEM_SPAWNER: // nodecay item spawner..Ripper
	case ITYPE_DECAYING_ITEM_SPAWNER: // decaying item spawner..Ripper
		if (isInWorld() || (pc->IsGMorCounselor()) || // Backpack in world - free access to everyone
			( cSerializable::isCharSerial(getContSerial()) && getContSerial()==pc->getSerial()))	// primary pack
		{
			pc->showContainer(this);
			pc->objectdelay=0;
			return;
		}
		else if( cSerializable::isItemSerial(getContSerial()) )
		{
			pItem pio = getOutMostCont();
			if (pio->getContSerial()==pc->getSerial() || pio->isInWorld() )
			{
				pc->showContainer(this);
				pc->objectdelay=0;
				return;
			}
		}
		if( itmowner )
			snooping(pc, pi );  //!\todo revise when snooping redone or updated
		return;
	case ITYPE_TELEPORTRUNE:

        //!\todo REVISE WHEN TARGETS REDONE!!

		targ = clientInfo[s]->newTarget( new cLocationTarget() );
		targ->code_callback = target_tele;
		targ->send( client );
		client->sysmessage( "Select teleport target.");
		return;
	case ITYPE_KEY:

        //!\todo REVISE WHEN TARGETS REDONE!!

		targ = clientInfo[s]->newTarget( new cItemTarget() );
		targ->code_callback = target_key;
		targ->buffer[0]= pi->more1;
		targ->buffer[1]= pi->more2;
		targ->buffer[2]= pi->more3;
		targ->buffer[3]= pi->more4;
		targ->send( client );
		client->sysmessage( "Select item to use the key on.");
		return;
	case ITYPE_LOCKED_ITEM_SPAWNER:
	case ITYPE_LOCKED_CONTAINER:

		// Added traps effects by AntiChrist
		// Wintermute: GMs or Counselors should be able to open locked containers always
		if ( !pc->IsGMorCounselor() )
		{
			if (more2.moreb1 > 0 ) {
				magic::castAreaAttackSpell(getPosition(), magic::spellExplosion);
				more2.moreb1--;
			}

			client->sysmessage("This item is locked.");
			return;
		}
		else
		{
			pc->showContainer(this);
 			return;
		}
	case ITYPE_SPELLBOOK:
		if (pack) // morrolan
			if(getContSerial()==pack->getSerial() || pc->IsWearing(this))
				client->sendSpellBook(this);
			else
				client->sysmessage("If you wish to open a spellbook, it must be equipped or in your main backpack.");
			return;
	case ITYPE_BLANK_MAP:
    		  //! \todo check if pc has a pen to write maps with
                //! \todo map writing code
		return;
	case ITYPE_DOOR:

//! \todo redo when houses updated

		dooruse(s, pi);
		return;
	case ITYPE_LOCKED_DOOR:
		// Wintermute: GMs or Counselors should be able to open locked doors always
		if ( pc->IsGMorCounselor())
 		{
                //! \todo redo when houses updated
 			dooruse(s, pi);
 			return;
 		}

		if (pack)
		{
                //! \todo redo when sets updated
			NxwItemWrapper si;
			si.fillItemsInContainer( pack );
			for( si.rewind(); !si.isEmpty(); si++ )
			{
				pItem pj = si.getItem();
				if (pj && pj->type==ITYPE_KEY)
					if ( pj->more1.more == more1.more )
					{
						client->sysmessage("You quickly unlock, use, and then relock the door.");

                                        //! \todo redo when houses updated

						dooruse(s, pi);
						return;
					}
			}
		}
		client->sysmessage("This door is locked.");
		return;
	case ITYPE_FOOD:

		if (pc->hunger >= 6)
		{
			client->sysmessage( "You are simply too full to eat any more!");
			return;
		}
		else
		{
			switch (RandomNum(0, 2))
			{
				case 0: pc->playSFX(0x3A); break;
				case 1: pc->playSFX(0x3B); break;
				case 2: pc->playSFX(0x3C); break;
			}

			switch (pc->hunger)
			{
				case 0:  client->sysmessage("You eat the food, but are still extremely hungry."); break;
				case 1:  client->sysmessage("You eat the food, but are still extremely hungry."); break;
				case 2:  client->sysmessage("After eating the food, you feel much less hungry."); break;
				case 3:  client->sysmessage("You eat the food, and begin to feel more satiated."); break;
				case 4:  client->sysmessage("You feel quite full after consuming the food."); break;
				case 5:  client->sysmessage("You are nearly stuffed, but manage to eat the food.");	break;
				default: client->sysmessage("You are simply too full to eat any more!"); break;
			}

			if (poisoned)
			{
				client->sysmessage("The food was poisoned!");
				pc->applyPoison(PoisonType(poisoned));

			}

			ReduceAmount(1);
		    pc->hunger++;
		}
		return;
	case ITYPE_WAND: // -Fraz- Modified and tuned up, Wands must now be equipped or in pack
	case ITYPE_MANAREQ_WAND: // magic items requiring mana (xan)
		if (pack)
		{
			if (getContSerial() == pack->getSerial() || pc->IsWearing(this))
			{
				if (morez != 0)
				{
					morez--;
					if (magic::beginCasting(
						static_cast<SpellId>((8*(morex - 1)) + morey - 1),
						client,
						(type==ITYPE_WAND) ? magic::CASTINGTYPE_ITEM : magic::CASTINGTYPE_NOMANAITEM))
						{
							if (morez == 0)
							{
								type = type2;
								morex = 0;
								morey = 0;
								offspell = 0;
							}
						}
				}
			}
			else
			{
				client->sysmessage("If you wish to use this, it must be equipped or in your backpack.");
			}
		}
		return; // case 15 (magic items)
	case ITYPE_POTION: // potions
			if (morey != 3)
				pc->drink(this);   //Luxor: delayed potions drinking
			else    //explosion potion

                        //! \todo revise this
				usepotion(pc, pi);
			return;
	case ITYPE_RUNE:
			if (morex==0 && morey==0 && morez==0)
			{
				client->sysmessage("That rune is not yet marked!");
			}
			else
			{
				pc->runeserial = getSerial();
				client->sysmessage("Enter new rune name.");
			}
			return;
	case ITYPE_SMOKE:
			pc->smoketimer = morex*SECS + getClockmSecs();
			ReduceAmount(1);
			return;
	case ITYPE_RENAME_DEED:
			pc->namedeedserial = getSerial();
			client->sysmessage("Enter your new name.");
			ReduceAmount(1);
			return;
	case ITYPE_POLYMORPH:

        //! \todo redo when polymorph redone, with cbody class use
			pc->setId( morex );
			pc->teleport();
			pi->type = ITYPE_POLYMORPH_BACK;
			return;
	case ITYPE_POLYMORPH_BACK:
        //! \todo redo when polymorph redone, with cbody class use
			pc->setId( pc->getOldId() );
			pc->teleport();
			type = ITYPE_POLYMORPH;
			return;
	case ITYPE_TELEPORT:
			pc->MoveTo(morex, morey, morez);
			pc->teleport();
			return;
	case ITYPE_DRINK:
			switch (rand()%2)
			{
				case 0: pc->playSFX(0x0031); break;
				case 1: pc->playSFX(0x0030); break;
			}
			ReduceAmount(1);
			client->sysmessage("Gulp !");
			return;
	case ITYPE_GUILDSTONE:

                //!\todo redo when guilds fixed
#if 0
			if ( getId() == 0x14F0  ||  getId() == 0x1869 )	// Check for Deed/Teleporter + Guild Type
			{
				pc->fx1 = pi->getSerial();   //!\todo <- check this
				Guilds->StonePlacement(s);    //!\todo <- and this
				return;
			}
			else if (getId() == 0x0ED5)	// Check for Guildstone + Guild Type
			{
				pc->fx1 = pi->getSerial();
				Guilds->Menu(s, 1);
				return;
			}
			else
				LogWarning("Unhandled guild item type named: %s with ID of: %X\n", getCurrentName().c_str(), getId());
			return;
#endif
	case ITYPE_PLAYER_VENDOR_DEED:			// PlayerVendors deed
			{
			pNpc vendor = npcs::AddNPCxyz(-1, 2117, charpos.x, charpos.y, charpos.z);
			if ( !vendor )
			{
				LogWarning("npc-script couldnt find vendor !\n");
				return;
			}

			los = 0;
			vendor->npcaitype = NPCAI_PLAYERVENDOR;
			vendor->MakeInvulnerable();
			vendor->unHide();
			vendor->stealth=INVALID;
			vendor->dir = pc->dir;
			vendor->npcWander = cNPC::WANDER_NOMOVE;
			vendor->SetInnocent();
			vendor->setOwner( pc );
			vendor->tamed = false;
			Delete();
			vendor->teleport();
			
			char *temp;
			asprintf( &temp, "Hello sir! My name is %s and i will be working for you.", vendor->getCurrentName().c_str());
			vendor->talk(client, temp, 0);
			free(temp);

			return;
			}
		default:
			break;
	}
	///END IDENTIFICATION BY TYPE

/////////////////READ UP :D////////////////////////////////

	///BEGIN IDENTIFICATION BY ID
	if (IsSpellScroll())
	{
		if (pack)
			if( getContSerial()==pack->getSerial()) {
				SpellId spn = magic::spellNumberFromScrollId(getId());	// avoid reactive armor glitch
				if ((spn>=0)&&(magic::beginCasting(spn, client, magic::CASTINGTYPE_SCROLL)))
					ReduceAmount(1);							// remove scroll if successful
			}
			else client->sysmessage("The scroll must be in your backpack to envoke its magic.");
	}
	CASE(IsAnvil) {
        //! \todo redo when targets redone
		targ = clientInfo[s]->newTarget( new cItemTarget() );
		targ->code_callback=Skills::target_repair;
		targ->send( client );
		client->sysmessage("Select item to be repaired.");
	}
	CASE(IsAxe) {
	//! \todo move it to cWeapon
        //! \todo redo when targets redone
		targ = clientInfo[s]->newTarget( new cTarget() );
		targ->code_callback=target_axe;
		targ->buffer[0]=pi->getSerial();
		targ->send( client );
		client->sysmessage("What would you like to use that on ?");
	}
	CASEOR(IsFeather, IsShaft) {
        //! \todo redo when targets redone
		targ = clientInfo[s]->newTarget( new cItemTarget() );
		targ->buffer[0]= pi->getSerial();
		targ->code_callback=Skills::target_fletching;
		targ->send( client );
		client->sysmessage("What would you like to use this with?");
	}
	CASEOR( IsFencing1H, IsSword ) {
	//! \todo move it to cWeapon
        //! \todo redo when targets redone
		targ = clientInfo[s]->newTarget( new cTarget() );
		targ->code_callback=target_sword;
		targ->send( client );
		client->sysmessage("What would you like to use that on ?");
	}
}

//! \todo revise from here

void target_selectdyevat( pClient client, pTarget t )
{
	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if ( ! pi ) return;

	if( pi->getId()==0x0FAB ||				//dye vat
	    pi->getId()==0x0EFF || pi->getId()==0x0E27 )	//hair dye
	{
		nPackets::Sent::DyeWindow pk(pi);
		client->sendPacket(&pk);
	}
	else client->sysmessage("You can only use this item on a dye vat.");
}

void target_dyevat( pClient client, pTarget t )
{
	pChar curr = client->currChar();
	if ( ! curr ) return;

	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if ( ! pi ) return;

	if( pi->isDyeable() )
	{
		pChar pc = pi->getPackOwner();

		if( !pc || ( pc == curr ) ) //in world or owned
		{
			pi->setColor( t->buffer[0] );
			pi->Refresh();
			curr->playSFX(0x023E); // plays the dye sound, LB
		}
		else
			client->sysmessage("That is not yours!!");
	}
	else
		client->sysmessage("You can only dye clothes with this.");
}


static void doubleclick_itemid(pClient client, pChar pc, pItem pi, pContainer pack )
{
	pTarget targ = NULL;

	switch (pi->getId())
	{
		case 0x0FA9: // dye
			targ = client->clientInfo->newTarget( new cItemTarget() );
			targ->code_callback=target_selectdyevat;
			targ->send(client);
			client->sysmessage("Which dye vat will you use this on?");
			return;// dye

		case 0x0FAB:// dye vat
			targ = client->clientInfo->newTarget( new cItemTarget() );
			targ->code_callback=target_dyevat;
			targ->buffer[0]=pi->getColor();
			targ->send(client);
			client->sysmessage("Select the clothing to use this on.");
			return;// dye vat

		case 0x100A:
		case 0x100B:// archery butte
			Skills::AButte(s, pi);
			return;// archery butte

		case 0x0E9C:
			if (pc->checkSkill( skMusicianship, 0, 1000))
				pc->playSFX( 0x38);
			else
				pc->playSFX( 0x39);
			return;

		case 0x0E9D:
		case 0x0E9E:
			if (pc->checkSkill(  skMusicianship, 0, 1000))
				pc->playSFX( 0x52 );
			else
				pc->playSFX( 0x53 );
			return;

		case 0x0EB1:
		case 0x0EB2:
			if (pc->checkSkill( skMusicianship, 0, 1000))
				pc->playSFX(0x45);
			else
				pc->playSFX( 0x46);
			return;

		case 0x0EB3:
		case 0x0EB4:
			if (pc->checkSkill( skMusicianship, 0, 1000))
				pc->playSFX( 0x4C);
			else
				pc->playSFX( 0x4D);
			return;

		case 0x102A:// Hammer
		case 0x102B:
		case 0x0FBB:// tongs
		case 0x0FBC:
		case 0x13E3:// smith's hammers
		case 0x13E4:
		case 0x0FB4:// sledge hammers
		case 0x0FB5:
			if (!ToolWearOut(client)) {
				targ = clientInfo[s]->newTarget( new cItemTarget() );
				targ->code_callback=Skills::target_smith;
				targ->send(client);
				client->sysmessage("Select material to use.");
			}
			return; // Smithy

		case 0x1026:// Chisels
		case 0x1027:
		case 0x1028:// Dove Tail Saw
		case 0x1029:
		case 0x102C:// Moulding Planes
		case 0x102D:
		case 0x102E:// Nails
		case 0x102F:
		case 0x1030:// Jointing plane
		case 0x1031:
		case 0x1032:// Smoothing plane
		case 0x1033:
		case 0x1034:// Saw
		case 0x1035:
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_carpentry;
			targ->send(client);
			client->sysmessage("Select material to use.");
			return; // carpentry

		case 0x0E85:// pickaxes
		case 0x0E86:
		case 0x0F39:// shovels
		case 0x0F3A:
			if (!ToolWearOut(client))
			{
				targ = clientInfo[s]->newTarget( new cLocationTarget() );
				targ->code_callback=Skills::target_mine;
				targ->buffer[0]=pi->getSerial();
				targ->send(client);
				client->sysmessage( "Where do you want to dig?");
			}
			return; // mining

		case 0x0DF9:
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_wheel;
			targ->buffer[0]=THREAD;
			targ->buffer[1]=pi->getSerial();
			targ->send(client);
			client->sysmessage("Select spinning wheel to spin cotton.");
			return;

		case 0x0DF8: // wool to yarn
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_wheel;
			targ->buffer[0]=YARN;
			targ->buffer[1]=pi->getSerial();
			targ->send(client);
			client->sysmessage("Select your spin wheel to spin wool.");
			return;

		case 0x0FA0:
		case 0x0FA1: // thread to Bolt
		case 0x0E1D:
		case 0x0E1F:
		case 0x0E1E:  // yarn to cloth
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_loom;
			targ->buffer[0]=pi->getSerial();
			targ->send(client);
			client->sysmessage("Select loom to make your cloth");
			return;

		case 0x0F9D: // sewing kit for tailoring
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_tailoring;
			targ->send(client);
			client->sysmessage("Select material to use.");
			return;

		case 0x19B7:
		case 0x19B9:
		case 0x19BA:
		case 0x19B8: // smelt ore
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_smeltOre;
			targ->buffer[0]=pi->getSerial();
			targ->send(client);
			client->sysmessage("Select forge to smelt ore on.");// smelting  for all ore changed by Myth 11/12/98
			return;

		case 0x1E5E:
		case 0x1E5F: // Message board opening
			((pMsgBoard)this)->openBoard( client );
			return;

		case 0x0DE1:
		case 0x0DE2: // camping
  			if ( !pc->hasInRange(pi, 3) )
  			{
				client->sysmessage("You are to far away to reach that");
				return;
  			}
  					//</Luxor>
			if (pc->checkSkill(  skCamping, 0, 500)) // Morrolan TODO: insert logout code for campfires here
			{
				pItem pFire = item::CreateFromScript( "$item_a_campfire" );
				if(pFire)
				{
					pFire->type = 45;
					pFire->dir = 2;
					pFire->setDecay();
					if (pi->isInWorld())
						pFire->MoveTo( pi->getPosition() );
					else
						pFire->MoveTo( pc->getPosition() );
					pFire->setDecayTime();
					pFire->Refresh();// AntiChrist
					pi->ReduceAmount(1);
				}
			}
			else
			{
				client->sysmessage("You fail to light a fire.");
			}
			return; // camping

		case 0x1508: // magic statue?
			if (pc->checkSkill( ITEMID, 0, 10))
			{
				pi->setId( 0x1509 );
				pi->type = 45;
				pi->Refresh();// AntiChrist
			}
			else
			{
				client->sysmessage("You failed to use this statue.");
			}
			return;

		case 0x1509:
			if (pc->checkSkill(  ITEMID, 0, 10))
			{
				pi->setId( 0x1508 );
				pi->type = 45;
				pi->Refresh();// AntiChrist
			}
			else
			{
				client->sysmessage("You failed to use this statue.");
			}
			return;
		case 0x1230:
		case 0x1246: // guillotines?
			if (pc->checkSkill(  ITEMID, 0, 10))
			{
				pi->setId( 0x1245 );
				pi->type = 45;
				pi->Refresh();// AntiChrist
			}
			else
			{
				client->sysmessage("You failed to use this.");
			}
			return;
		case 0x1245: // Guillotine stop animation
			if (pc->checkSkill(  ITEMID, 0, 10))
			{
				pi->setId( 0x1230 );
				pi->type = 45;
				pi->Refresh();// AntiChrist
			}
			else
			{
				client->sysmessage("You failed to use this.");
			}
			return;
		case 0x0DBF:
		case 0x0DC0:// fishing
			if( pi->getContSerial()==pc->getSerial() || pi->getContSerial()==pack->getSerial() ) {
				targ = clientInfo[s]->newTarget( new cLocationTarget() );
				targ->code_callback = Fishing::target_fish;
				targ->send(client);
				client->sysmessage("Fish where?");
			}
			else
				client->sysmessage("If you wish to use this, it must be equipped or in your backpack.");
			return;
		case 0x104B:
		case 0x104C:
		case 0x1086: // Clock and bracelet
			ps->telltime();
			return;
		case 0x0E9B: // Mortar for Alchemy
			pc->objectdelay = ((nSettings::Actions::getObjectsDelay() * SECS)*3) + getClockmSecs();
			if (pi->type == ITYPE_MANAREQ_WAND)
			{
				targ = clientInfo[s]->newTarget( new cItemTarget() );
				targ->code_callback=Skills::target_bottle;
				targ->buffer[0]=pi->getSerial();
				targ->send(client);
				client->sysmessage("Where is an empty bottle for your potion?");
			}
			else
			{
				targ = clientInfo[s]->newTarget( new cItemTarget() );
				targ->code_callback=Skills::target_alchemy;
				targ->buffer[0]=pi->getSerial();
				targ->send(client);
				client->sysmessage("What do you wish to grind with your mortar and pestle?");
			}
			return; // alchemy
		case 0x0E21: // healing
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_healingSkill;
			targ->buffer[0]=pi->getSerial();
			targ->send(client);
			client->sysmessage("Who will you use the bandages on?");
			return;
		case 0x1057:
		case 0x1058: // sextants
			//!\todo Need to find out the center of the outer region.. waiting for new region system
			client->sysmessage( "You are at: %s", getSextantCoords(pc->getPosition(), ));
			return;
		case 0x0E27:
		case 0x0EFF:   // Hair Dye
			pc->useHairDye(pi);
			return;
		case 0x14FB:
		case 0x14FC:
		case 0x14FD:
		case 0x14FE: // lockpicks
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_lockpick;
			targ->buffer[0]=pi->getSerial();
			targ->send(client);
			client->sysmessage("What lock would you like to pick?");
			return;
		
		case 0x097A: // Raw Fish steaks
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_cookOnFire;
			targ->buffer[0]=0x097B;
			targ->buffer[1]=pi->getSerial();
			targ->buffer_str[0] = "fish steaks";
			targ->send(client);
			client->sysmessage("What would you like to cook this on?");
			return;
		
		case 0x09b9: // Raw Bird
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_cookOnFire;
			targ->buffer[0]=0x09B7;
			targ->buffer[1]=pi->getSerial();
			targ->buffer_str[0] = "bird";
			targ->send(client);
			client->sysmessage("What would you like to cook this on?");
			return;
		
		case 0x1609: // Raw Lamb
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_cookOnFire;
			targ->buffer[0]=0x160A;
			targ->buffer[1]=pi->getSerial();
			targ->buffer_str[0] = "lamb";
			targ->send(client);
			client->sysmessage("What would you like to cook this on?");
			return;
		
		case 0x09F1: // Raw Ribs
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_cookOnFire;
			targ->buffer[0]=0x09F2;
			targ->buffer[1]=pi->getSerial();
			targ->buffer_str[0] = "ribs";
			targ->send(client);
			ps->sysmessage("What would you like to cook this on?");
			return;
		
		case 0x1607: // Raw Chicken Legs
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_cookOnFire;
			targ->buffer[0]=0x1608;
			targ->buffer[1]=pi->getSerial();
			targ->buffer_str[0] = "chicken legs";
			targ->send(client);
			client->sysmessage("What would you like to cook this on?");
			return;
		
		case 0x0C4F:
		case 0x0C50:
		case 0x0C51:
		case 0x0C52:
		case 0x0C53:
		case 0x0C54: // cotton plants
			{
				if (!pc->isMounting())
					pc->playAction(0x0D);
				else
					pc->playAction(0x1D);
				pc->playSFX(0x013E);
				pItem itm = item::CreateFromScript( "$item_bales_of_cotton", pc->getBackpack() );
				if (itm) {
					client->sysmessage("You reach down and pick some cotton.");
				}
			}
			return; // cotton
		
		case 0x105B:
		case 0x105C:
		case 0x1053:
		case 0x1054: // tinker axle
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_tinkerAxel;
			targ->buffer[0]=pi->getSerial();
			targ->send(client);
			client->sysmessage("Select part to combine that with.");
			return;
		
		case 0x1051:
		case 0x1052:
		case 0x1055:
		case 0x1056:
		case 0x105D:
		case 0x105E:
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_tinkerAwg;
			targ->buffer[0]=pi->getSerial();
			targ->send(client);
			client->sysmessage("Select part to combine it with.");
			return;
		
		case 0x104F:
		case 0x1050:
		case 0x104D:
		case 0x104E:// tinker clock
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_tinkerClock;
			targ->buffer[0]=pi->getSerial();
			targ->send(client);
			client->sysmessage("Select part to combine with.");
			return;
		
		case 0x1059:
		case 0x105A:// tinker sextant
			if (pc->checkSkill(  skTinkering, 500, 1000))
			{
				client->sysmessage("You create the sextant.");
				pItem pi_c = item::CreateFromScript( "$item_sextant", pc->getBackpack() );
				if (pi_c)
					pi_c->setDecay();
				pi->ReduceAmount(1);
			}
			else
				client->sysmessage("you fail to create the sextant.");
			return;
		
		case 0x1070:
		case 0x1074: // training dummies
			if ( pc->hasInRange(pi, 1) )
			{
				if ( pc->isHiddenBySkill() )
					pc->unHide();
				Skills::TDummy(s);
			}
			else
				client->sysmessage("You need to be closer to use that.");
			return;
		case 0x1071:
		case 0x1073:
		case 0x1075:
		case 0x1077:// swinging training dummy
			client->sysmessage("You must wait for it to stop swinging !");
			return;
		
		//case 0x1EA8:
		//	slotmachine(s, pi);
		//	return; // Ripper
		
		case 0x1EBC: // tinker's tools
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback = Skills::target_tinkering;
			targ->send(client);
			client->sysmessage( "Select material to use.");
			return;
		
		default:
			client->sysmessage( "You can't think of a way to use that item.");
			break;
	}
}

/*!
\brief wrap for check usability
\author Chronodt
\param pc player trying using
\param type type of usability
\remarks Luxor - Added REQSKILL command support, three bug fix applied
*/
bool cItem::checkItemUsability(pChar pc, int type)
{
	if(!pc) return false;

	pPC pc_tmp;
	pClient client = (pc_tmp = dynamic_cast<pPC>(pc))? pc_tmp->getClient() : NULL;

	if(!client) return true;
		
	g_nType = type;

	if( !isNewbie() )
	{
		if ( st > pc->getStrength() )
		{
			client->sysmessage("You are not strong enough to use that.");
			return false;
		}
		if ( dx > pc->getDexterity() )
		{
			client->sysmessage("You are not quick enough to use that.");
			return false;
		}
		if ( in > pc->getIntelligence() )
		{
			client->sysmessage("You are not intelligent enough to use that.");
			return false;
		}
		//Luxor: REQSKILL command support
		if (reqskill[0] > 0 && reqskill[1] > 0 )
		{
			if (reqskill[1] > skill[reqskill[0]]) {
				client->sysmessage("You are not skilled enough to use that.");
				return false;
			}
		}
	}

	if ( events[evtItmOnCheckCanUse] ) {
		cVariantVector params = cVariantVector(3);
		params[0] = getSerial(); params[1] = pc->getSerial();
		params[2] = g_nType;
		events[evtItmOnCheckCanUse]->setParams(params);
		cVariant ret = events[evtItmOnCheckCanUse]->execute();
		return ret.toBoolean();
	}
	
	return true;
}

/*!
\brief apply wear out to item, delete if necessary
\author Ripper, rewritten by Luxor
\param client client of player who wear out the item
*/
bool cItem::ToolWearOut(pClient client)
{
	pChar pc;
	if ( ! client || ! (pc = client->currChar()) )
		return false;
	
	if( ! chance(5) ) // has item been destroyed ??
		return false;
		
	hp--;
	if ( hp )
		return false
		
	client->sysmessage("Your %s has been destroyed", getCurrentName().c_str());
	Delete();
	return true;
}
