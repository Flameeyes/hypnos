/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

/*!
\file
\brief Item use
*/

#include "common_libs.h"
#include "network.h"
#include "itemid.h"
#include "sndpkg.h"
#include "debug.h"
#include "magic.h"
#include "house.h"
#include "npcai.h"
#include "mount.h"
#include "nxw_utils.h"
#include "data.h"
#include "boats.h"
#include "set.h"
#include "map.h"
#include "basics.h"
#include "inlines.h"
#include "skills.h"
#include "classes.h"
#include "range.h"
#include "utils.h"
#include "fishing.h"

bool cItem::usableWhenLockedDown(pPc pc)
{
//! \todo this function
	switch(type)
        {
        	case ITYPE_CONTAINER:
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
                case ITYPE_SLOTMACHINE:
                case ITYPE_DECIPHERED_MAP:
                	return true;		// These items are always usable when locked down


                case ITYPE_KEY:
                	{
                        	if (pc == owner) return true;
                        	if (inHouse!= NULL)
                                	if(inHouse->isFriend(pc)) return true;


                	}	// these items can be used only by owner or house owner/coowner/friend when locked down
        }
        return false
}

/*!
\brief single click on item
\return bool
\param client client of player who clicked the item
*/

void cItem::singleClick(pClient client )
{
	char temp[TEMP_STR_SIZE];
	int32_t amt = 0, wgt;
	char itemname[100];
	char temp2[100];
//	extern skill_st skill[SKILLS + 1]; // unused variable

	pChar pj;

	if ( events[evtItmOnClick] ) {
		tVariantVector params = tVariantVector(2);
		params[0] = getSerial(); params[1] = client->currChar()->getSerial();
		events[evtItmOnClick]->setParams(params);
		events[evtItmOnClick]->execute();
		if ( events[evtItmOnClick]->bypassed() )
			return;
	}
	
	getName( itemname );

	if ( type == ITYPE_SPELLBOOK )
	{
		sprintf( temp, "[%i spells]", countSpellsInSpellBook() );
		itemmessage(client, temp, serial, 0x0481);
	}

	if ( CanSeeSerials() )
	{
		if (amount > 1)
			sprintf( temp, "%s [%x]: %i", itemname, getSerial(), amount);
		else
			sprintf( temp, "%s [%x]", itemname, getSerial());
		itemmessage(client, temp, serial);
		return;
	}

	// Click in a Player Vendor item, show description, price and return
	if (!isInWorld() && isItemSerial(getContSerial()))
	{
		pItem cont = getContainer();
		if( cont ) {
			pj = cont->getPackOwner();
			if( pj )
			{
				if( pj->npcaitype==NPCAI_PLAYERVENDOR )
				{
					if ( !creator.empty() && madewith>0)
						sprintf( temp2, "%s %s by %s", vendorDescription.c_str(), ::skillinfo[madewith - 1].madeword, creator.c_str());
					else
						strcpy( temp2, vendorDescription.c_str() );

					sprintf( temp, "%s at %igp", temp2, value );
					itemmessage(client, temp, serial);
					return;
				}
			}
		}
	}

	// From now on, we will build the message into temp, and let itemname with just the name info
	// Add amount info.
	if (!pileable || amount == 1)
		strncpy( temp, itemname, 100);
	else
		if (itemname[strlen(itemname) - 1] != 's') // avoid iron ingotss : x
			sprintf( temp, "%ss : %i", itemname, amount);
		else
			sprintf( temp, "%s : %i", itemname, amount);

	// Add creator's mark (if any)
	if ( !creator.empty() && madewith > 0)
		sprintf( temp, "%s %s by %s", temp, ::skillinfo[madewith - 1].madeword, creator.c_str());

	if (type == ITYPE_WAND) // Fraz
	{
		if (!(strcmp(getSecondaryNameC(), getCurrentName().c_str())))
		{
			sprintf( temp, "%s %i charge", temp, morez);
			if (morez != 1)
				strcat(temp, "s");
		}
	}
	else if (type == ITYPE_ITEMID_WAND || type == ITYPE_FIREWORKS_WAND)
	{
			if (!(strcmp(getSecondaryNameC(), getCurrentName().c_str())))
			{
				sprintf( temp, "%s %i charge", temp, morex);
				if (morex != 1)
					strcat(temp, "s");
			}
	}
	// Corpse highlighting...Ripper
	if (corpse==1)
	{
		if(more2==1)
		        itemmessage( client,"[Innocent]",serial, 0x005A);
		else if(more2==2)
			itemmessage( client,"[Criminal]",serial, 0x03B2);
		else if(pi->more2==3)
			itemmessage( client,"[Murderer]",serial, 0x0026);
	}  // end highlighting
	// Let's handle secure/locked down stuff.
	if (magic == 4 && type != ITYPE_DOOR && type != ITYPE_GUMPMENU)
	{
		if (secureIt !=1)
			itemmessage( client, "[locked down]", serial, 0x0481);
		if (secureIt == 1 && magic == 4)
			itemmessage( client, "[locked down & secure]", serial, 0x0481);
	}

	itemmessage(client, temp, serial);

	/*! \todo This should be changed to a virtual function */
#if 0
	// Send the item/weight as the last line in case of containers
	if (type == ITYPE_CONTAINER || type == ITYPE_UNLOCKED_CONTAINER || type == ITYPE_NODECAY_ITEM_SPAWNER || type == ITYPE_TRASH)
	{
		wgt = (int32_t) weights::LockeddownWeight(this, &amt); // get stones and item #, LB
		if (amt>0)
		{
			sprintf( temp2, "[%i items, %i stones]", amt, wgt);
			itemmessage( client, temp2, serial);
		}
		else
			itemmessage( client, "[0 items, 0 stones]", serial);
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

\todo URGENT! Split the function in two: one will check the double click if is correct, and then a virtual one
	who inherit the switch, so the new cClass can simply have their doubleClicked(pClient) function to call
*/
void cItem::doubleClick(pClient client)
{

	if (client==NULL) return;
	pChar pc = client->currChar();
	if ( ! pc ) return;

	if ( events[evtItmOnDoubleClick] ) {
		tVariantVector params = tVariantVector(2);
		params[0] = getSerial(); params[1] = pc->getSerial();
		events[evtItmOnDoubleClick]->setParams(params);
		events[evtItmOnDoubleClick]->execute();
		if ( events[evtItmOnDoubleClick]->bypassed() )
			return;
	}
	
	if (!checkItemUsability(pc, ITEM_USE_DBLCLICK))
		return;

	Location charpos= pc->getPosition();

        // Luxor: cannot use items if under invisible spell
        // Chronodt: 1/2/2004 added a server parameter check to allow servers to let items be used when invisible
        if (pc->IsHiddenBySpell() && !nSettings::Actions::canUseItemsWhenInvisible()) return;

	if ( !pc->IsGM() && pc->objectdelay >= getClock() )
	{
		pc->sysmsg("You must wait to perform another action.");
		return;
	}
	else
		pc->objectdelay = SrvParms->objectdelay * MY_CLOCKS_PER_SEC + getClock();

	///MODIFY, CANT CLICK ITEM AT DISTANCE >2//////////////
	if ( (pc->distFrom(pi)>2) && !pc->IsGM() && !(pc->hasTelekinesis()) ) //Luxor: let's check also for the telekinesys spell
	{
		pc->sysmsg( "Must be closer to use this!");
		pc->objectdelay=0;
		return;
	}


//<Anthalir> VARIAIBLI

	tile_st item;

	pContainer pack = pc->getBackpack();
	VALIDATEPI( pack );


	data::seekTile( getId(), item );
//////FINEVARIABILI


	//<Luxor>: Circle of transparency bug fix
	pContainer cont;
	Location dst;

	cont = getOutMostCont();

	if(cont->isInWorld()) {
		dst = cont->getPosition();
	} else {
		pChar pg_dst = cSerializable::findCharBySerial( cont->getContSerial() );  //TODO: verify if doing a getContSerial to a body returns a char
		VALIDATEPC(pg_dst);
		dst = pg_dst->getPosition();
	}

	Location charPos = pc->getPosition();
	charPos.z = dst.z;
	charPos.dispz = dst.dispz;

	if ( !pc->IsGM() && !lineOfSight( charPos, dst ) && !pc->hasTelekinesis() ) {
		pc->sysmsg(  "You cannot reach the item" );
		return;
	}
	//</Luxor>

	pChar itmowner = getPackOwner();

	if(!isInWorld()) {
		if (isItemSerial(getContSerial()) && type != ITYPE_CONTAINER)
		{// Cant use stuff that isn't in your pack.

			if ( itmowner && tmowner->getSerial()!=pc->getSerial() )
					return;
		}
		else
			if (isCharSerial(getContSerial()) && type!=(uint32_t)INVALID)
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
			pc->sysmsg( "That is a secured chest!");
			return;
		}
	}

	if ((magic == 4) && !usableWhenLockedDown())  // Chronodt: without this last check, locked down chests could not be opened by nonowners even if unlocked
	{
		pc->sysmsg( "That item is locked down.");
		return;
	}

	if (pc->dead && >type!=ITYPE_RESURRECT) // if you are dead and it's not an ankh, FORGET IT!
	{
		pc->sysmsg("You may not do that as a ghost.");
		return;
	}
	else if (!pc->IsGMorCounselor() && layer!=0 && !pc->IsWearing(this))
	{// can't use other people's things!
		if (!(layer==LAYER_BACKPACK  && SrvParms->rogue==1)) // bugfix for snooping not working, LB
		{
			pc->sysmsg("You cannot use items equipped by other players.");
			return;
		}
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
					pc->sysmsg("%s", disabledmsg->c_str());
				else
					pc->sysmsg("That doesnt seem to work right now.");
				return;
			}
		}
		else
		{
			pc->sysmsg( "You are not close enough to use that.");
			return;
		}
	}

	// check this on trigger in the event that the .trigger property is not set on the item
	// trigger code.  Check to see if item is envokable by id
	else if (checkenvoke( getId() ))
	{
		pc->envokeitem = getSerial();
		pc->envokeid = getId();

                //TODO: REVISE WHEN TARGETS REDONE!!

		pTarget targ = clientInfo[s]->newTarget( new cObjectTarget() );
		targ->code_callback=target_envoke;
		targ->send( client );
		client->sysmsg( "What will you use this on?");
		return;
	}
	// END trigger stuff
	// BEGIN Check items by type

	int los = 0;

	pTarget targ = NULL;
	
	doubleClicked(client);
}

void cItem::doubleClicked(pClient client)
{
	switch (type)
	{
	case ITYPE_RESURRECT:
		// Check for 'resurrect item type' this is the ONLY type one can use if dead.
		if (pc->dead)
		{
			pc->resurrect();
			pc->sysmsg("You have been resurrected.");
			return;
		}
		else
		{
			pc->sysmsg("You are already living!");
			return;
		}
	case ITYPE_BOATS:// backpacks - snooping a la Zippy - add check for SrvParms->rogue later- Morrolan

		if (type2 == 3)
		{
			switch( getId() & 0xFF ) {
				case 0x84:
				case 0xD5:
				case 0xD4:
				case 0x89:
					Boats->PlankStuff(client, this);
					break;
				default:
					pc->sysmsg( "That is locked.");
					break;
			}
			return;
		}
	case ITYPE_CONTAINER: // bugfix for snooping not working, lb
	case ITYPE_UNLOCKED_CONTAINER:
		// Wintermute: GMs or Counselors should be able to open trapped containers always
		if (moreb1 > 0 && !pc->IsGMorCounselor()) {
			magic::castAreaAttackSpell(getPosition().x, getPosition().y, magic::SPELL_EXPLOSION);
			moreb1--;
		}
		//Magic->MagicTrap(currchar[s], pi); // added by AntiChrist
		// only 1 and 63 can be trapped, so pleaz leave it here :) - Anti
	case ITYPE_NODECAY_ITEM_SPAWNER: // nodecay item spawner..Ripper
	case ITYPE_DECAYING_ITEM_SPAWNER: // decaying item spawner..Ripper
		if (isInWorld() || (pc->IsGMorCounselor()) || // Backpack in world - free access to everyone
			( isCharSerial(getContSerial()) && getContSerial()==pc->getSerial()))	// primary pack
		{
			pc->showContainer(this);
			pc->objectdelay=0;
			return;
		}
		else if( isItemSerial(getContSerial()) )
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
			snooping(pc, pi );  //!\todo: revise when snooping redone or updated
		return;
	case ITYPE_TELEPORTRUNE:

        //!\todo: REVISE WHEN TARGETS REDONE!!

		targ = clientInfo[s]->newTarget( new cLocationTarget() );
		targ->code_callback = target_tele;
		targ->send( client );
		client->sysmsg( "Select teleport target.");
		return;
	case ITYPE_KEY:

        //!\todo: REVISE WHEN TARGETS REDONE!!

		targ = clientInfo[s]->newTarget( new cItemTarget() );
		targ->code_callback = target_key;
		targ->buffer[0]= pi->more1;
		targ->buffer[1]= pi->more2;
		targ->buffer[2]= pi->more3;
		targ->buffer[3]= pi->more4;
		targ->send( client );
		client->sysmsg( "Select item to use the key on.");
		return;
	case ITYPE_LOCKED_ITEM_SPAWNER:
	case ITYPE_LOCKED_CONTAINER:

		// Added traps effects by AntiChrist
		// Wintermute: GMs or Counselors should be able to open locked containers always
		if ( !pc->IsGMorCounselor() )
		{
			if (moreb1 > 0 ) {
				magic::castAreaAttackSpell(getPosition().x, getPosition().y, magic::SPELL_EXPLOSION);
				>moreb1--;
			}

			pc->sysmsg("This item is locked.");
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
				pc->sysmsg("If you wish to open a spellbook, it must be equipped or in your main backpack.");
			return;
	case ITYPE_BLANK_MAP:
    		  //TODO check if pc has a pen to write maps with
                //! \todo map writing code
		return;
	case ITYPE_MAP:
                cPacketSendOpenMapGump pk((pMap)this);
		client->sendPacket(&pk);
                cPacketSendMapPlotCourse pk2((pMap)this, ClearAllPins); //Sending clear all pins command
		client->sendPacket(&pk2);

                std::vector<pindataobject>::iterator iter = ((pMap)this)->pinData.begin()
                for(int i = 1;i <= ((pMap)this)->getPinsNumber(); i++)
                {
                	cPacketSendMapPlotCourse pki((pMap)this, AddPin, 0, ((pMap)this)->getX(i), ((pMap)this)->getY(i));
			client->sendPacket(&pki);
                }


		return;
	case ITYPE_DOOR:

//TODO: redo when houses updated

		dooruse(s, pi);
		return;
	case ITYPE_LOCKED_DOOR:
		// Wintermute: GMs or Counselors should be able to open locked doors always
		if ( pc->IsGMorCounselor())
 		{
                //TODO: redo when houses updated
 			dooruse(s, pi);
 			return;
 		}

		if (pack)
		{
                //TODO: redo when sets updated
			NxwItemWrapper si;
			si.fillItemsInContainer( pack );
			for( si.rewind(); !si.isEmpty(); si++ )
			{
				pItem pj = si.getItem();
				if (pj && pj->type==ITYPE_KEY)
					if (((pj->more1 == more1) && (pj->more2 == more2) &&
						 (pj->more3 == more3) && (pj->more4 == more4)) )
					{
						pc->sysmsg("You quickly unlock, use, and then relock the door.");

                                        //TODO: redo when houses updated

						dooruse(s, pi);
						return;
					}
			}
		}
		pc->sysmsg("This door is locked.");
		return;
	case ITYPE_FOOD:

		if (pc->hunger >= 6)
		{
			pc->sysmsg( "You are simply too full to eat any more!");
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
				case 0:  pc->sysmsg( "You eat the food, but are still extremely hungry."); break;
				case 1:  pc->sysmsg( "You eat the food, but are still extremely hungry."); break;
				case 2:  pc->sysmsg( "After eating the food, you feel much less hungry."); break;
				case 3:  pc->sysmsg( "You eat the food, and begin to feel more satiated."); break;
				case 4:  pc->sysmsg( "You feel quite full after consuming the food."); break;
				case 5:  pc->sysmsg( "You are nearly stuffed, but manage to eat the food.");	break;
				default: pc->sysmsg( "You are simply too full to eat any more!"); break;
			}

			if (poisoned)
			{
				pc->sysmsg("The food was poisoned!");
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
						static_cast<magic::SpellId>((8*(morex - 1)) + morey - 1),
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
				pc->sysmsg("If you wish to use this, it must be equipped or in your backpack.");
			}
		}
		return; // case 15 (magic items)
#if 0 // REMOVE
	case 18: // crystal ball?
		switch (RandomNum(0, 9))
		{
		case 0: itemmessage(s, "Seek out the mystic llama herder.", pi->getSerial());
			break;
		case 1: itemmessage(s, "Wherever you go, there you are.", pi->getSerial());
			break;
		case 4: itemmessage(s, "The message appears to be too cloudy to make anything out of it.", pi->getSerial());
			break;
		case 5: itemmessage(s, "You have just lost five strength.. not!", pi->getSerial());
			break;
		case 6: itemmessage(s, "You're really playing a game you know", pi->getSerial());
			break;
		case 7: itemmessage(s, "You will be successful in all you do.", pi->getSerial());
			break;
		case 8: itemmessage(s, "You are a person of culture.", pi->getSerial());
			break;
		default: itemmessage(s, "Give me a break! How much good fortune do you expect!", pi->getSerial());
			break;
		}// switch
		soundeffect2(pc_currchar, 0x01EC);
		return;// case 18 (crystal ball?)
#endif  // ENDREMOVE
	case ITYPE_POTION: // potions
			if (morey != 3)
				pc->drink(this);   //Luxor: delayed potions drinking
			else    //explosion potion

                        //TODO: revise this
				usepotion(pc, pi);
			return;
	case ITYPE_RUNE:
			if (morex==0 && morey==0 && morez==0)
			{
				pc->sysmsg( "That rune is not yet marked!");
			}
			else
			{
				pc->runeserial = getSerial();
				pc->sysmsg( "Enter new rune name.");
			}
			return;
	case ITYPE_SMOKE:
			pc->smoketimer = morex*MY_CLOCKS_PER_SEC + getclock();
			ReduceAmount(1);
			return;
	case ITYPE_RENAME_DEED:
			pc->namedeedserial = getSerial();
			pc->sysmsg( "Enter your new name.");
			ReduceAmount(1);
			return;
	case ITYPE_POLYMORPH:

        //TODO: redo when polymorph redone, with cbody class use
			pc->setId( morex );
			pc->teleport();
			pi->type = ITYPE_POLYMORPH_BACK;
			return;
	case ITYPE_POLYMORPH_BACK:
        //TODO: redo when polymorph redone, with cbody class use
			pc->setId( pc->getOldId() );
			pc->teleport();
			type = ITYPE_POLYMORPH;
			return;
	case ITYPE_ARMY_ENLIST:

        //TODO: redo this
			enlist(s, morex);
			Delete();
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
			pc->sysmsg( "Gulp !");
			return;
	case ITYPE_GUILDSTONE:

                //!\todo: redo when guilds fixed

			if ( getId() == 0x14F0  ||  getId() == 0x1869 )	// Check for Deed/Teleporter + Guild Type
			{
				pc->fx1 = DEREF_pItem(pi);   //!\todo: <- check this
				Guilds->StonePlacement(s);    //!\todo: <- and this
				return;
			}
			else if (getId() == 0x0ED5)	// Check for Guildstone + Guild Type
			{
				pc->fx1 = DEREF_pItem(pi);
				Guilds->Menu(s, 1);
				return;
			}
			else
				WarnOut("Unhandled guild item type named: %s with ID of: %X\n", getCurrentName().c_str(), getId());
			return;
	case ITYPE_PLAYER_VENDOR_DEED:			// PlayerVendors deed
			{
			pNpc vendor = npcs::AddNPCxyz(-1, 2117, charpos.x, charpos.y, charpos.z);
			if ( !vendor )
			{
				WarnOut("npc-script couldnt find vendor !\n");
				return;
			}

			los = 0;
			vendor->npcaitype = NPCAI_PLAYERVENDOR;
			vendor->MakeInvulnerable();
			vendor->unHide();
			vendor->stealth=INVALID;
			vendor->dir = pc->dir;
			vendor->npcWander = WANDER_NOMOVE;
			vendor->SetInnocent();
			vendor->setOwnerSerial32( pc->getSerial() );
			vendor->tamed = false;
			Delete();
			vendor->teleport();
			char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
			sprintf( temp, "Hello sir! My name is %s and i will be working for you.", vendor->getCurrentName().c_str());
			vendor->talk(client, temp, 0);

			return;
			}
	case ITYPE_TREASURE_MAP:
                //TODO: redo when treasures redone
			Skills::Decipher(this, client);
			return;

	case ITYPE_DECIPHERED_MAP:

                	cPacketSendOpenMapGump pk((pMap)this);
			client->sendPacket(&pk);
	                cPacketSendMapPlotCourse pk2((pMap)this, ClearAllPins); //Sending clear all pins command
			client->sendPacket(&pk2);
        
			// Generate message to add a map point
			int16_t posx, posy;					// tempoary storage for map point
			int16_t tlx, tly, lrx, lry;				// tempoary storage for map extends
			tlx = (more1 << 8)  | more2;
			tly = (more3 << 8)  | more4;
			lrx = (moreb1 << 8) | moreb2;
			lry = (moreb3 << 8) | moreb4;
			posx = (256 * (morex - tlx)) / (lrx - tlx);		// Generate location for point
			posy = (256 * (morey - tly)) / (lry - tly);
                        
                        cPacketSendMapPlotCourse pk3((pMap)this, AddPin,0,posx, posy);//Sending add pin command
			client->sendPacket(&pk3);
			return;
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
				magic::SpellId spn = magic::spellNumberFromScrollId(getId());	// avoid reactive armor glitch
				if ((spn>=0)&&(magic::beginCasting(spn, client, magic::CASTINGTYPE_SCROLL)))
					ReduceAmount(1);							// remove scroll if successful
			}
			else pc->sysmsg("The scroll must be in your backpack to envoke its magic.");
	}
	CASE(IsAnvil) {
        //TODO: redo when targets redone
		targ = clientInfo[s]->newTarget( new cItemTarget() );
		targ->code_callback=Skills::target_repair;
		targ->send( client );
		client->sysmsg( "Select item to be repaired.");
	}
	CASE(IsAxe) {
        //TODO: redo when targets redone
		targ = clientInfo[s]->newTarget( new cTarget() );
		targ->code_callback=target_axe;
		targ->buffer[0]=pi->getSerial();
		targ->send( client );
		client->sysmsg( "What would you like to use that on ?");
	}
	CASEOR(IsFeather, IsShaft) {
        //TODO: redo when targets redone
		targ = clientInfo[s]->newTarget( new cItemTarget() );
		targ->buffer[0]= pi->getSerial();
		targ->code_callback=Skills::target_fletching;
		targ->send( client );
		client->sysmsg( "What would you like to use this with?");
	}
	CASEOR( IsFencing1H, IsSword ) {
        //TODO: redo when targets redone
		targ = clientInfo[s]->newTarget( new cTarget() );
		targ->code_callback=target_sword;
		targ->send( client );
		client->sysmsg( "What would you like to use that on ?");
	}
	else ///BEGIN IDENTIFICATION BY ID ( RAW MODE, DEPRECATED )
		doubleclick_itemid( client, pc, pi, pack );
}


//TODO revise from here

void target_selectdyevat( pClient client, pTarget t )
{
    pItem pi=cSerializable::findItemBySerial(t->getClicked());
    if ( ! pi ) return;

    if( pi->getId()==0x0FAB ||                     //dye vat
        pi->getId()==0x0EFF || pi->getId()==0x0E27 )  //hair dye
            client->sndDyevat(pi->getSerial(), pi->getId() );
        else
            client->sysmsg( "You can only use this item on a dye vat.");
}

void target_dyevat( pClient client, pTarget t )
{
	pChar curr = client->currChar();
	if ( ! curr ) return;

	pItem pi=cSerializable::findItemBySerial( t->getClicked() );
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
			curr->sysmsg("That is not yours!!");
	}
	else
		curr->sysmsg( "You can only dye clothes with this.");
}


static void doubleclick_itemid(pClient client, pChar pc, pItem pi, pContainer pack )
{
	pTarget targ = NULL;

	switch (pi->getId())
	{
		case 0x0FA9: // dye
			targ = client->clientInfo->newTarget( new cItemTarget() );
			targ->code_callback=target_selectdyevat;
			targ->send( client );
			client->sysmsg( "Which dye vat will you use this on?");
			return;// dye
		case 0x0FAB:// dye vat
			targ = client->clientInfo->newTarget( new cItemTarget() );
			targ->code_callback=target_dyevat;
			targ->buffer[0]=pi->getColor();
			targ->send( ps );
			ps->sysmsg( "Select the clothing to use this on.");
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
				targ->send( ps );
				ps->sysmsg( "Select material to use.");
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
			targ->send( ps );
			ps->sysmsg( "Select material to use.");
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
				targ->send( ps );
				ps->sysmsg( "Where do you want to dig?");
			}
			return; // mining
		case 0x0DF9:
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_wheel;
			targ->buffer[0]=THREAD;
			targ->buffer[1]=pi->getSerial();
			targ->send( ps );
			ps->sysmsg( "Select spinning wheel to spin cotton.");
			return;
		case 0x0DF8: // wool to yarn
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_wheel;
			targ->buffer[0]=YARN;
			targ->buffer[1]=pi->getSerial();
			targ->send( ps );
			ps->sysmsg( "Select your spin wheel to spin wool.");
			return;
		case 0x0FA0:
		case 0x0FA1: // thread to Bolt
		case 0x0E1D:
		case 0x0E1F:
		case 0x0E1E:  // yarn to cloth
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_loom;
			targ->buffer[0]=pi->getSerial();
			targ->send( ps );
			ps->sysmsg( "Select loom to make your cloth");
			return;
		case 0x0F9D: // sewing kit for tailoring
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_tailoring;
			targ->send( ps );
			ps->sysmsg( "Select material to use.");
			return;
		case 0x19B7:
		case 0x19B9:
		case 0x19BA:
		case 0x19B8: // smelt ore
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_smeltOre;
			targ->buffer[0]=pi->getSerial();
			targ->send( ps );
			ps->sysmsg( "Select forge to smelt ore on.");// smelting  for all ore changed by Myth 11/12/98
			return;
		case 0x1E5E:
		case 0x1E5F: // Message board opening
			((pMsgBoard)this)->openBoard( client );
			return;
		case 0x0DE1:
		case 0x0DE2: // camping
  			if ( !pc->hasInRange(pi, 3) )
  			{
				pc->sysmsg("You are to far away to reach that");
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
				pc->sysmsg("You fail to light a fire.");
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
				pc->sysmsg("You failed to use this statue.");
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
				pc->sysmsg("You failed to use this statue.");
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
				pc->sysmsg("You failed to use this.");
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
				pc->sysmsg("You failed to use this.");
			}
			return;
		case 0x0DBF:
		case 0x0DC0:// fishing
			if( pi->getContSerial()==pc->getSerial() || pi->getContSerial()==pack->getSerial() ) {
				targ = clientInfo[s]->newTarget( new cLocationTarget() );
				targ->code_callback = Fishing::target_fish;
				targ->send( ps );
				ps->sysmsg("Fish where?");
			}
			else
				pc->sysmsg("If you wish to use this, it must be equipped or in your backpack.");
			return;
		case 0x104B:
		case 0x104C:
		case 0x1086: // Clock and bracelet
			telltime(ps);
			return;
		case 0x0E9B: // Mortar for Alchemy
			pc->objectdelay = ((SrvParms->objectdelay * MY_CLOCKS_PER_SEC)*3) + getClock();
			if (pi->type == ITYPE_MANAREQ_WAND)
			{
				targ = clientInfo[s]->newTarget( new cItemTarget() );
				targ->code_callback=Skills::target_bottle;
				targ->buffer[0]=pi->getSerial();
				targ->send( ps );
				ps->sysmsg( "Where is an empty bottle for your potion?");
			}
			else
			{
				targ = clientInfo[s]->newTarget( new cItemTarget() );
				targ->code_callback=Skills::target_alchemy;
				targ->buffer[0]=pi->getSerial();
				targ->send( ps );
				ps->sysmsg( "What do you wish to grind with your mortar and pestle?");
			}
			return; // alchemy
		case 0x0E21: // healing
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_healingSkill;
			targ->buffer[0]=pi->getSerial();
			targ->send( ps );
			ps->sysmsg( "Who will you use the bandages on?");
			return;
		case 0x1057:
		case 0x1058: // sextants
			{
				char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp2 var
				getSextantCoords( pc->getPosition().x, pc->getPosition().y, (pc->getPosition().x >= 5121), temp);
				pc->sysmsg("You are at: %s", temp);
			}
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
			targ->send( ps );
			ps->sysmsg( "What lock would you like to pick?");
			return;
		case 0x097A: // Raw Fish steaks
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_cookOnFire;
			targ->buffer[0]=0x097B;
			targ->buffer[1]=pi->getSerial();
			targ->buffer_str[0] = "fish steaks";
			targ->send( ps );
			ps->sysmsg( "What would you like to cook this on?");
			return;
		case 0x09b9: // Raw Bird
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_cookOnFire;
			targ->buffer[0]=0x09B7;
			targ->buffer[1]=pi->getSerial();
			targ->buffer_str[0] = "bird";
			targ->send( ps );
			ps->sysmsg( "What would you like to cook this on?");
			return;
		case 0x1609: // Raw Lamb
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_cookOnFire;
			targ->buffer[0]=0x160A;
			targ->buffer[1]=pi->getSerial();
			targ->buffer_str[0] = "lamb";
			targ->send( ps );
			ps->sysmsg( "What would you like to cook this on?");
			return;
		case 0x09F1: // Raw Ribs
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_cookOnFire;
			targ->buffer[0]=0x09F2;
			targ->buffer[1]=pi->getSerial();
			targ->buffer_str[0] = "ribs";
			targ->send( ps );
			ps->sysmsg( "What would you like to cook this on?");
			return;
		case 0x1607: // Raw Chicken Legs
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_cookOnFire;
			targ->buffer[0]=0x1608;
			targ->buffer[1]=pi->getSerial();
			targ->buffer_str[0] = "chicken legs";
			targ->send( ps );
			ps->sysmsg( "What would you like to cook this on?");
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
					pc->sysmsg("You reach down and pick some cotton.");
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
			targ->send( ps );
			ps->sysmsg( "Select part to combine that with.");
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
			targ->send( ps );
			ps->sysmsg( "Select part to combine it with.");
			return;
		case 0x104F:
		case 0x1050:
		case 0x104D:
		case 0x104E:// tinker clock
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback=Skills::target_tinkerClock;
			targ->buffer[0]=pi->getSerial();
			targ->send( ps );
			ps->sysmsg( "Select part to combine with");
			return;
		case 0x1059:
		case 0x105A:// tinker sextant
			if (pc->checkSkill(  skTinkering, 500, 1000))
			{
				pc->sysmsg("You create the sextant.");
				pItem pi_c = item::CreateFromScript( "$item_sextant", pc->getBackpack() );
				if (pi_c)
					pi_c->setDecay();
				pi->ReduceAmount(1);
			}
			else
				pc->sysmsg("you fail to create the sextant.");
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
				pc->sysmsg("You need to be closer to use that.");
				return;
		case 0x1071:
		case 0x1073:
		case 0x1075:
		case 0x1077:// swinging training dummy
			pc->sysmsg("You must wait for it to stop swinging !");
			return;
		//case 0x1EA8:
		//	slotmachine(s, DEREF_pItem(pi));
		//	return; // Ripper
		case 0x1EBC: // tinker's tools
			targ = clientInfo[s]->newTarget( new cItemTarget() );
			targ->code_callback = Skills::target_tinkering;
			targ->send( ps );
			ps->sysmsg( "Select material to use.");
			return;
		default:
			pc->sysmsg( "You can't think of a way to use that item.");
			break;
	}
}

/*!
\brief wrap for check usability
\author Xanathar, update by Chronodt (2/2/2004)
\return bool
\param pc player trying using
\param type type of usability
\remarks Luxor - Added REQSKILL command support, three bug fix applied
*/
bool cItem::checkItemUsability(pChar pc, int type)
{
	g_nType = type;
//	VALIDATEPIR(pi, false);
	VALIDATEPCR(pc, false);

	if( !isNewbie() )
	{
		if ( st > pc->getStrength() )
		{
			pc->sysmsg("You are not strong enough to use that.");
			return false;
		}
		if ( dx > pc->getDexterity() )
		{
			pc->sysmsg("You are not quick enough to use that.");
			return false;
		}
		if ( in > pc->getIntelligence() )
		{
			pc->sysmsg("You are not intelligent enough to use that.");
			return false;
		}
		//Luxor: REQSKILL command support
		if (reqskill[0] > 0 && reqskill[1] > 0 )
		{
			if (reqskill[1] > skill[reqskill[0]]) {
				pc->sysmsg("You are not skilled enough to use that.");
				return false;
			}
		}
	}

	if ( ! pc->getClient() )
		return true;
		
	if ( events[evtItmOnCheckCanUse] ) {
		tVariantVector params = tVariantVector(3);
		params[0] = getSerial(); params[1] = pc->getSerial();
		params[2] = g_nType;
		events[evtItmOnCheckCanUse]->setParams(params);
		tVariant ret = events[evtItmOnCheckCanUse]->execute();
		return ret.toBoolean();
	}
	
	return true;
}

/*!
\brief apply wear out to item, delete if necessary
\author Ripper, rewritten by Luxor
\return bool
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
		
	pc->sysmsg("Your %s has been destroyed", getCurrentName().c_str());
	Delete();
	return true;
}
