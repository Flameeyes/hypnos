/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "basics.h"
#include "sndpkg.h"
#include "party.h"
#include "magic.h"
#include "house.h"
#include "targeting.h"
#include "commands.h"
#include "tmpeff.h"
#include "data.h"
#include "boats.h"
#include "fishing.h"
#include "map.h"
#include "skills.h"
#include "inlines.h"
#include "basics.h"

void CarveTarget(pClient client, int feat, int ribs, int hides, int fur, int wool, int bird)
{
	if(!client) return;

	pChar pc = client->currChar();
	if ( ! pc ) return;

	pItem pi1 = item::CreateFromScript( "$item_blood_puddle" );
	if(!pi1) return;

	pi1->setId( 0x122A );

	pItem pi2 = cSerializable::findItemBySerial(npcshape[0]);
	if(!pi2) return;

	mapRegions->remove(pi1);
	pi1->setPosition( pi2->getPosition() );
	mapRegions->add(pi1); // lord Binary
	pi1->magic=2;//AntiChrist - makes the item unmovable
	pi1->setDecayTime();
	pi1->Refresh();

	if(feat>0)
	{
		pItem pi=item::CreateFromScript( "$item_feathers", pc->getBackpack(), feat );
		if ( ! pi ) return;
		pi->Refresh();
		client->sysmessage("You pluck the bird and get some feathers.");
	}
	if(ribs>0)
	{
		pItem pi=item::CreateFromScript( "$item_cuts_of_raw_ribs", pc->getBackpack(), ribs );
		if ( ! pi ) return;
		pi->Refresh();
		client->sysmessage("You carve away some meat.");
	}

	if(hides>0)
	{
		pItem pi=item::CreateFromScript( "$item_hide", pc->getBackpack(), hides );
		if (!pi) return;
		pi->Refresh();
		client->sysmessage("You skin the corpse and get the hides.");
	}
	if(fur>0)
	{
		pItem pi=item::CreateFromScript( "$item_hide", pc->getBackpack(), fur );
		if ( ! pi ) return;
		pi->Refresh();
		client->sysmessage("You skin the corpse and get the hides.");
	}
	if(wool>0)
	{
		pItem pi=item::CreateFromScript( "$item_piles_of_wool", pc->getBackpack(), wool );
		if ( ! pi ) return;
		pi->Refresh();
		client->sysmessage("You skin the corpse and get some unspun wool.");
	}
	if(bird>0)
	{
		pItem pi = item::CreateFromScript( "$item_raw_bird", pc->getBackpack(), bird );
		if ( ! pi ) return;
		pi->Refresh();
		client->sysmessage("You carve away some raw bird.");
	}

	pc->getBody()->calcWeight();
}

/*!
\author AntiChrist
\date 03/11/1999
\brief New Carving System
\note Human-corpse carving code added
\note Scriptable carving product added
*/
static void newCarveTarget(pClient client, pItem pi3)
{
	pChar pc = client->currChar();
	pItem pi1 = item::CreateFromScript( "$item_blood_puddle" );
	if( !pc || !pi1 || !pi3 ) return;

	bool deletecorpse=false;
	char sect[512];


	pi1->setId( 0x122A );

	pItem pi2i = cSerializable::findItemBySerial(npcshape[0]);
	if(!pi2) return;

	mapRegions->remove(pi1);
	pi1->setPosition( pi2->getPosition() );
	mapRegions->add(pi1); // lord Binary
	pi1->magic=2;//AntiChrist - makes the item unmovable
	pi1->setDecayTime();

	pi1->Refresh();

	//if it's a human corpse
	if(pi3->morey)
	{
		pc->modifyFame(ServerScp::g_nChopFameLoss); // Ripper..lose fame and karma and criminal.
		pc->IncreaseKarma(+ServerScp::g_nChopKarmaLoss);
		client->sysmessage("You lost some fame and karma!");
		pc->setCrimGrey(ServerScp::g_nChopWillCriminal);//Blue and not attacker and not guild

		//create the Head
		pItem pi = item::CreateFromScript( "$item_hardcoded" );
		if ( ! pi ) return;
		pi->setCurrentName( "the head of " + pi3->getSecondaryName() );
		pi->setId( 0x1DA0 );
		pi->setContainer(0);
		pi->MoveTo(pi3->getPosition());
		pi->layer=0x01;
		pi->att=5;
		//AntiChrist & Magius(CHE) - store item's owner, so that lately
		//if we want to know the owner we can do it
		pi->setSameOwnerAs(pi3);
		pi->setDecay();
		pi->Refresh();

		//create the Heart
		pi=item::CreateFromScript( "$item_a_heart" );
		if ( ! pi ) return;
		pi->setCurrentName( "the heart of " + pi3->getSecondaryName() );
		pi->setContainer(0);
		pi->MoveTo(pi3->getPosition());
		pi->layer=0x01;
		pi->att=5;
		pi->setSameOwnerAs(pi3);  // see above
		pi->setCurrentName( temp ); //Luxor
		pi->setDecay();
		pi->Refresh();

		//create the Body
		pi = item::CreateFromScript( "$item_hardcoded" );
		if ( ! pi ) return;
		pi->setCurrentName( "the body of " + pi3->getSecondaryName() );
		pi->setId( 0x1DAD );
		pi->setContainer(0);
		pi->MoveTo(pi3->getPosition());
		pi->att=5;
		pi->setSameOwnerAs(pi3);  // see above
		pi->setDecay();
		pi->Refresh();

		//create the Left Arm
		pi = item::CreateFromScript( "$item_hardcoded" );
		if ( ! pi ) return;
		pi->setCurrentName( "the left arm of " + pi3->getSecondaryName() );
		pi->setId( 0x1DA1 );
		pi->setContainer(0);
		pi->MoveTo(pi3->getPosition());
		pi->att=5;
		pi->setSameOwnerAs(pi3);  // see above
		pi->setDecay();
		pi->Refresh();

		//create the Right Arm
		pi = item::CreateFromScript( "$item_hardcoded" );
		if ( ! pi ) return;
		pi->setCurrentName( "the right arm of " + pi3->getSecondaryName() );
		pi->setId( 0x1DA2 );
		pi->setContainer(0);
		pi->MoveTo(pi3->getPosition());
		pi->att=5;
		pi->setSameOwnerAs(pi3);  // see above
		pi->setDecay();
		pi->Refresh();

		//create the Left Leg
		pi = item::CreateFromScript( "$item_hardcoded" );
		if ( ! pi ) return;
		pi->setCurrentName( "the left leg of " + pi3->getSecondaryName() );
		pi->setId( 0x1DAE );
		pi->setContainer(0);
		pi->MoveTo(pi3->getPosition());
		pi->att=5;
		pi->setSameOwnerAs(pi3);  // see above
		pi->setDecay();
		pi->Refresh();

		//create the Rigth Leg
		pi = item::CreateFromScript( "$item_hardcoded" );
		if ( ! pi ) return;
		pi->setCurrentName( "the right leg of " + pi3->getSecondaryName() );
		pi->setId( 0x1DA4 );
		pi->setContainer(0);
		pi->MoveTo(pi3->getPosition());
		pi->att=5;
		pi->setSameOwnerAs(pi3);  // see above
		pi->setDecay();
		pi->Refresh();

		//human: always delete corpse!
		deletecorpse = true;
	} else {
		cScpIterator* iter = NULL;
		char script1[1024];
		char script2[1024];
		sprintf(sect,"SECTION CARVE %i",pi3->carve);
		iter = Scripts::Carve->getNewIterator(sect);
		if (iter==NULL) return;


		int loopexit=0;
		do
		{
			iter->parseLine(script1, script2);
			if ((script1[0]!='}')&&(script1[0]!='{'))
			{
				if (!(strcmp("ADDITEM",script1)))
				{
					std::string itemnum, amount;
					splitLine( script2, itemnum, amount );
					int amt = str2num( amount );
					if( amt == 0 )
						amt=INVALID;
					pItem pi = item::CreateFromScript( (char*)itemnum.c_str(), pi3, amt );
					if( pi ) {
						pi->layer=0;
						pi->Refresh();//let's finally refresh the item
					}
				}
			}
		} while ( (script1[0]!='}') && (++loopexit < MAXLOOPS) );
	
		safedelete(iter);
	}

	if(deletecorpse)//if corpse has to be deleted
	{
		NxwItemWrapper si;
		si.fillItemsInContainer( pi3, false );
		for( si.rewind(); !si.isEmpty(); si++ )
		{
			pItem pj=si.getItem();
			if(pj) {
				pj->setContainer(0);
				pj->MoveTo( pi3->getPosition() );
				pj->setDecayTime();
				pj->Refresh();
			}
		}
		pi3->Delete();
	}
}

static void CorpseTarget(pClient client)
{
	if (!client) return;
	int n=0;
	
	uint32_t serial=LongFromCharPtr(buffer[s]+7);
	pItem pi = cSerializable::findItemBySerial( serial );
	pChar pc = client->currChar();
	
	if ( ! pi || ! pc->hasInRange(pi, 1) )
		return;
    
	
	//!\todo Move to new carve system always
	
	npcshape[0]=i;
	pc->playAction(0x20);
	n=1;
	if(pi->more1.moreb1 !=0 )
	{
		client->sysmessage("You carve the corpse but find nothing usefull.");
		return;
	}
	
	pi->more1=1;//corpse being carved...can't carve it anymore

	if( pi->morey || pi->carve > -1 )	//if specified, use enhanced carving system!
	{
		newCarveTarget(client, pi);
		return;
	}
	
	switch(pi->amount)
	{
	case 0x01: CarveTarget(client, 0, 2, 0, 0, 0, 0); break; //Ogre
	case 0x02: CarveTarget(client, 0, 5, 0, 0, 0, 0); break; //Ettin
	case 0x05: CarveTarget(client,36, 0, 0, 0, 0, 1); break; //Eagle
	case 0x06: CarveTarget(client,25, 0, 0, 0, 0, 1); break; //Bird
	case 0x07: CarveTarget(client, 0, 1, 0, 0, 0, 0); break; //Orc w/axe
	case 0x09: CarveTarget(client, 0, 1, 0, 0, 0, 0); break; //Deamon
	case 0x0A: CarveTarget(client, 0, 1, 0, 0, 0, 0); break; //Deamon w/sword
	case 0x0C: CarveTarget(client, 0,19,20, 0, 0, 0); break; //Dragon (green)
	case 0x11: CarveTarget(client, 0, 3, 0, 0, 0, 0); break; //Orc
	case 0x12: CarveTarget(client, 0, 5, 0, 0, 0, 0); break; //Ettin w/club
	case 0x15: CarveTarget(client, 0, 4,20, 0, 0, 0); break; //Giant Serpent
	case 0x16: CarveTarget(client, 0, 1, 0, 0, 0, 0); break; //Gazer
	case 0x1D: CarveTarget(client, 0, 1, 0, 1, 0, 0); break; //Gorilla
	case 0x1E: CarveTarget(client,50, 0, 0, 0, 0, 1); break; //Harpy
	case 0x1F: CarveTarget(client, 0, 1, 0, 0, 0, 0); break; //Headless
	case 0x21: CarveTarget(client, 0, 1,12, 0, 0, 0); break; //Lizardman
	case 0x0122: CarveTarget(client, 0,10, 0, 0, 0, 0); break; // Boar
	case 0x23: CarveTarget(client, 0, 1,12, 0, 0, 0); break; //Lizardman w/spear
	case 0x24: CarveTarget(client, 0, 1,12, 0, 0, 0); break; //Lizardman w/mace
	case 0x27: CarveTarget(client, 0, 1, 0, 0, 0, 0); break; //Mongbat
	case 0x29: CarveTarget(client, 0, 1, 0, 0, 0, 0); break; //Orc w/club
	case 0x34: CarveTarget(client, 0, 1, 0, 0, 0, 0); break; //Snake
	case 0x35: CarveTarget(client, 0, 2, 0, 0, 0, 0); break; //Troll w/axe
	case 0x36: CarveTarget(client, 0, 2, 0, 0, 0, 0); break; //Troll
	case 0x37: CarveTarget(client, 0, 2, 0, 0, 0, 0); break; //Troll w/club
	case 0x3B: CarveTarget(client, 0,19,20, 0, 0, 0); break; //Dragon (red)
	case 0x3C: CarveTarget(client, 0,10,20, 0, 0, 0); break; //Drake (green)
	case 0x3D: CarveTarget(client, 0,10,20, 0, 0, 0); break; //Drake (red)
	case 0x46: CarveTarget(client, 0, 0, 0, 0, 0, 0); break; //Terathen Matriarche - t2a
	case 0x47: CarveTarget(client, 0, 0, 0, 0, 0, 0); break; //Terathen drone - t2a
	case 0x48: CarveTarget(client, 0, 0, 0, 0, 0, 0); break; //Terathen warrior, Terathen Avenger - t2a
	case 0x4B: CarveTarget(client, 0,4, 0, 0, 0, 0); break; //Titan - t2a
	case 0x4C: CarveTarget(client, 0, 4, 0, 0, 0, 0); break; //Cyclopedian Warrior - t2a
	case 0x50: CarveTarget(client, 0,10, 2, 0, 0, 0); break; //Giant Toad - t2a
	case 0x51: CarveTarget(client, 0, 4, 1, 0, 0, 0); break; //Bullfrog - t2a
	case 0x55: CarveTarget(client, 0, 5, 7, 0, 0, 0); break; //Ophidian apprentice, Ophidian Shaman - t2a
	case 0x56: CarveTarget(client, 0, 5, 7, 0, 0, 0); break; //Ophidian warrior, Ophidian Enforcer, Ophidian Avenger - t2a
	case 0x57: CarveTarget(client, 0, 5, 7, 0, 0, 0); break; //Ophidian Matriarche - t2a
	case 0x5F: CarveTarget(client, 0,19,20, 0, 0, 0); break; //Kraken - t2a
	case 0x96: CarveTarget(client, 0,10, 0, 0, 0, 0); break; //Sea Monster
	case 0x97: CarveTarget(client, 0, 1, 0, 0, 0, 0); break; //Dolphin
	case 0xC8: CarveTarget(client, 0, 3,10, 0, 0, 0); break; //Horse (tan)
	case 0xC9: CarveTarget(client, 0, 1, 0, 1, 0, 0); break; //Cat
	case 0xCA: CarveTarget(client, 0, 1,12, 0, 0, 0); break; //Alligator
	case 0xCB: CarveTarget(client, 0, 6, 0, 0, 0, 0); break; //Pig
	case 0xCC: CarveTarget(client, 0, 3,10, 0, 0, 0); break; //Horse (dark)
	case 0xCD: CarveTarget(client, 0, 1, 0, 1, 0, 0); break; //Rabbit
	case 0xCE: CarveTarget(client, 0, 1,12, 0 ,0, 0); break; //Lava Lizard - t2a
	case 0xCF: CarveTarget(client, 0, 3, 0, 0, 1, 0); break; //Sheep
	case 0xD0: CarveTarget(client,25, 0, 0, 0, 0, 1); break; //Chicken
	case 0xD1: CarveTarget(client, 0, 2, 8, 0, 0, 0); break; //Goat
	case 0xD2: CarveTarget(client, 0,15, 0, 0, 0, 0); break; //Desert Ostarge - t2a
	case 0xD3: CarveTarget(client, 0, 1, 12, 0, 0, 0); break; //Bear
	case 0xD4: CarveTarget(client, 0, 1, 0, 2, 0, 0); break; //Grizzly Bear
	case 0xD5: CarveTarget(client, 0, 2, 0, 3, 0, 0); break; //Polar Bear
	case 0xD6: CarveTarget(client, 0, 1, 10, 0, 0, 0); break; //Cougar
	case 0xD7: CarveTarget(client, 0, 1, 0, 1, 0, 0); break; //Giant Rat
	case 0xD8: CarveTarget(client, 0, 8,12, 0, 0, 0); break; //Cow (black)
	case 0xD9: CarveTarget(client, 0, 1, 0, 0, 0, 0); break; //Dog
	case 0xDA: CarveTarget(client, 0,15, 0, 0, 0, 0); break; //Frenzied Ostard - t2a
	case 0xDB: CarveTarget(client, 0,15, 0, 0, 0, 0); break; //Forest Ostard - t2a
	case 0xDC: CarveTarget(client, 0, 1, 12,0, 0, 0); break; //Llama
	case 0xDD: CarveTarget(client, 0, 1,12, 0, 0, 0); break; //Walrus
	case 0xDF: CarveTarget(client, 0, 3, 0, 0, 0, 0); break; //Sheep (BALD)
	case 0xE1: CarveTarget(client, 0, 1, 0, 1, 0, 0); break; //Timber Wolf
	case 0xE2: CarveTarget(client, 0, 3,10, 0, 0, 0); break; //Horse (Silver)
	case 0xE4: CarveTarget(client, 0, 3,10, 0, 0, 0); break; //Horse (tan)
	case 0xE7: CarveTarget(client, 0, 8,12, 0, 0, 0); break; //Cow (brown)
	case 0xE8: CarveTarget(client, 0,10,15, 0, 0, 0); break; //Bull (brown)
	case 0xE9: CarveTarget(client, 0,10,15, 0, 0, 0); break; //Bull (d-brown)
	case 0xEA: CarveTarget(client, 0, 6,15, 0, 0, 0); break; //Great Heart
	case 0xED: CarveTarget(client, 0, 5, 8, 0, 0, 0); break; //Hind
	case 0xEE: CarveTarget(client, 0, 1, 0, 0, 0, 0); break; //Rat
	}// switch
}

int BuyShop(pClient client, pChar pc)
{
	pItem buyRestockContainer=NULL, buyNoRestockContainer=NULL;

	if(!client || !pc) return 0;

	pChar curr = client->currChar();
	if(!curr) return 0;

	//!\todo Update to new layer system
	NxwItemWrapper si;
	si.fillItemWeared( pc, true, true, false );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi=si.getItem();
		if(!pi) continue;
	
		if( pi->layer==LAYER_TRADE_RESTOCK )
			buyRestockContainer=pi;
	
		if( pi->layer==LAYER_TRADE_NORESTOCK )
			buyNoRestockContainer=pi;

		if( buyRestockContainer && buyNoRestockContainer )
			break;
 	}

    if (!buyRestockContainer || !buyNoRestockContainer )
        return 0;

    impowncreate(s, pc, 0); // Send the NPC again to make sure info is current. (OSI does this we might not have to)

    sendshopinfo(s, pc, buyRestockContainer); // Send normal shop items
//  sendshopinfo(s, c, buyNoRestockContainer); // Send items sold to shop by players
    nPackets::Sent::OpenGump ok(pc->getSerial(), 0x0030);
    client->sendPacket(&pk);

    //! \todo check second argument
    client->statusWindow(curr,true); // Make sure the gold total has been sent.

    return 1;
}



void target_playerVendorBuy( pClient client, pTarget t )
{
	pChar pc = cSerializable::findCharBySerial(t->buffer[0]);
	if (!pc) return;

	pChar pc_currchar = client->currChar();
	if(!pc_currchar) return;

	pItem pBackpack= pc_currchar->getBackpack();
	if (!pBackpack) {
		client->sysmessage("Time to buy a backpack");
		return; 
	}

	pItem pi=cSerializable::findItemBySerial(LongFromCharPtr(buffer[s] +7));     // the item
    
	if ( !pi || pi->isInWorld() )
		return;

	int price=pi->value;


	pItem thepack=(pItem)pi->getContainer();
	if(!thepack) return;

	pChar npc = thepack->getPackOwner();               // the vendor

	if(npc->getSerial() != pc->getSerial() || pc->npcaitype!=NPCAI_PLAYERVENDOR) return;
	
	if (pc_currchar->isOwnerOf(pc))
	{
		pc->talk(s, "I work for you, you need not buy things from me!",0);
		return;
	}
	
	int gleft=pc_currchar->CountGold();
	if (gleft<pi->value)
	{
		pc->talk(s, "You cannot afford that.",0);
		return;
	}
	pBackpack->DeleteAmount(price,0x0EED);  // take gold from player
	
	pc->talk(s, "Thank you.",0);
	pc->holdg+=pi->value; // putting the gold to the vendor's "pocket"
	
	// sends item to the proud new owner's pack
	pi->setContainer( pBackpack );
	pi->Refresh();
}

void target_envoke( pClient client, pTarget t )
{
	pChar curr = client->currChar();
	pItem pi = NULL; pChar pc = NULL;

	if( pi = dynamic_cast<pItem>(t->getClicked()) )
	{
		pi->triggerItem(client, TRIGTYPE_ENVOKED );
		curr->envokeid=0x0000;
	} else if( pc = dynamic_cast<pChar>(t->getClicked()) )
	{
		triggerNpc(client, pc, TRIGTYPE_NPCENVOKED );
		curr->envokeid=0x0000;
	} else {
		triggerTile( ps->toInt() );
		curr->envokeid=0x0000;
	}
}


void target_key( pClient client, pTarget t )
{
	pChar pc = client->currChar();
	if ( ! pc ) return;

	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if ( ! pi ) return;

    if ( !pi->more )
        {
            if ( pi->type==ITYPE_KEY && (pc->hasInRange(pi, 2) || (!pi->isInWorld()) ) )
            {
                if (!pc->checkSkill(skTinkering, 400, 1000))
                {
                    client->sysmessage("You fail and destroy the key blank.");
                    // pi->playSFX( <whatever> );
                    pi->Delete();
                }
                else
                {
                    pi->more = LongFromCharPtr( t->buffer +0);
                    // pi->playSFX( <whatever> );
                    client->sysmessage("You copy the key."); //Morrolan can copy keys
                }
            }
            return;
        }//if
        else if ( (pi->more == LongFromCharPtr( t->buffer +0)) ||
            (t->buffer[0]==(unsigned char) 0xFF))
        {
            if (((pi->type==ITYPE_CONTAINER)||(pi->type==ITYPE_UNLOCKED_CONTAINER))&&(pc->hasInRange(pi, 2)))
            {
                if(pi->type==ITYPE_CONTAINER) pi->type=ITYPE_LOCKED_ITEM_SPAWNER;
                if(pi->type==ITYPE_UNLOCKED_CONTAINER) pi->type=ITYPE_LOCKED_CONTAINER;
                // pi->playSFX( <whatever> );
                client->sysmessage("You lock the container.");
                return;
            }
            else if ((pi->type==ITYPE_KEY)&&(pc->hasInRange(pi, 2)))
            {
                pc->keyserial=pi->getSerial();
                client->sysmessage("Enter new name for key.");//morrolan rename keys
                return;
            }
            else if ((pi->type==ITYPE_LOCKED_ITEM_SPAWNER)||(pi->type==ITYPE_LOCKED_CONTAINER)&& pc->hasInRange(pi, 2) )
            {
                if(pi->type==ITYPE_LOCKED_ITEM_SPAWNER) pi->type=ITYPE_CONTAINER;
                if(pi->type==ITYPE_LOCKED_CONTAINER) pi->type=ITYPE_UNLOCKED_CONTAINER;
                // pi->playSFX( <whatever> );
                client->sysmessage("You unlock the container.")
                return;
            }
            else if ((pi->type==ITYPE_DOOR)&& pc->hasInRange(pi, 2) )
            {
                pi->type=ITYPE_LOCKED_DOOR;
                // pi->playSFX( <whatever> );
                client->sysmessage("You lock the door.");
                return;
            }
            else if ((pi->type==ITYPE_LOCKED_DOOR)&& pc->hasInRange(pi, 2) )
            {
                pi->type=ITYPE_DOOR;
                // pi->playSFX( <whatever> );
                client->sysmessage("You unlock the door.");
                return;
            }
            else if (pi->getId()==0x0BD2)
            {
                client->sysmessage("What do you wish the sign to say?");
                pc->keyserial = pi->getSerial(); //Morrolan sign kludge
                return;
            }

            //Boats ->
            else if(pi->type==ITYPE_BOATS && pi->type2==3)
            {
                Boats->OpenPlank(pi);
                pi->Refresh();
            }
            //End Boats --^
        }//else if
        else
        {
            if (pi->type==ITYPE_KEY) client->sysmessage("That key is not blank!");
            else if (pi->more1==0x00) client->sysmessage("That does not have a lock.");
            else client->sysmessage("The key does not fit into that lock.");
            return;
        }//else
}

void target_attack( pClient client, pTarget t )
{
	//!\todo modify the parameter to get client instead of socket
	pChar pc_t1 = cSerializable::findCharBySerial( t->buffer[0] );
	pChar pc_t2 = dynamic_cast<pChar>( t->getClicked() );
	if ( ! pc_t1 || ! pc_t2 ) return;

	client->currChar()->attackStuff(pc_t2); //this will (eventually) flag the owner if ordering to attack an innocent
	npcattacktarget(pc_t1, pc_t2);
}

void target_follow( pClient client, pTarget t )
{
	pChar pc = cSerializable::findCharBySerial( t->buffer[0] );
	if (!pc) return;

	pChar pc2 = dynamic_cast<pChar>( t->getClicked() );
	if (!pc2) return;

	pc->ftargserial=pc2->getSerial();
	pc->npcWander=WANDER_FOLLOW;
}

void target_axe( pClient ps, pTarget t )
{
	uint16_t id=t->getModel();
	if (itemById::IsTree(id))
		Skills::target_tree( ps, t );
	else if (itemById::IsCorpse(id))
		CorpseTarget(ps);
	else if (itemById::IsLog(id)) //Luxor bug fix
		Skills::target_bowcraft( ps, t );
}


void target_sword( pClient client, pTarget t )
{
	pChar pc = client->currChar();
	if ( ! pc ) return;

	uint16_t id = t->getModel();
	if (itemById::IsTree2(id))
	{
		sLocation pcpos= pc->getPosition();
		sLocation location = t->getPosition();

		if( dist( location, pcpos )>5 )
		{
			client->sysmessage("You are to far away to reach that");
			return;
		}

		pc->playAction( pc->isMounting() ? 0x0D : 0x01D );
		pc->playSFX(0x013E);

		pItem pi=item::CreateFromScript( "$item_kindling" );
		if ( ! pi ) return;

		pi->setPosition( pcpos );
		mapRegions->add(pi);

		pi->Refresh();
		client->sysmessage("You hack at the tree and produce some kindling.");
	}
	else if(itemById::IsLog(id)) // vagrant
	{
		Skills::target_bowcraft( client, t );
	}
	else if(itemById::IsCorpse(id))
	{
		CorpseTarget(client);
	}
	else
		client->sysmessage("You can't think of a way to use your blade on that.");
}

void target_fetch( pClient client, pTarget t )
{
    client->sysmessage( "Fetch is not available at this time.");
}

void target_guard( pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pChar pPet = cSerializable::findCharBySerial(t->buffer[0]);
	if ( ! pc || ! pPet ) return;

	pChar pToGuard = dynamic_cast<pChar>( t->getClicked() );
	if( !pToGuard || pToGuard != pPet->getOwner() )
	{
		client->sysmessage( "Currently can't guard anyone but yourself!" );
		return;
	}
	pPet->npcaitype = NPCAI_PETGUARD;
	pPet->ftargserial=pc->getSerial();
	pPet->npcWander=WANDER_FOLLOW;
	client->sysmessage( "Your pet is now guarding you.");
	pc->guarded = true;
}

void target_transfer( pClient client, pTarget t )
{
	pChar pc1 = cSerializable::findCharBySerial( t->buffer[0] );
	pChar pc2 = dynamic_cast<pChar>( t->getClicked() );
	if ( ! pc1 || ! pc2 )
		return;

	pFunctionHandle evt = pc1->getEvent(cNPC::evtNpcOnTransfer);
	if ( evt )
	{
		tVariantVector params = tVariantVector(2);
		params[0] = pc1->getSerial(); params[1] = pc2->getSerial();
		evt->setParams(params);
		evt->execute();
		if ( evt->isBypassed() )
			return;
	}

	pc1->talkAll("* %s will now take %s as his master *",0, pc1->getCurrentName().c_str(), pc2->getCurrentName().c_str());
	
	pc1->setOwner( pc2 );
	pc1->npcWander=WANDER_FOLLOW;
	pc1->ftargserial=INVALID;
	pc1->npcWander=WANDER_NOMOVE;
}

 //Throws the potion and places it (unmovable) at that spot
void target_expPotion( pClient client, pTarget t )
{
	pChar pc = client->currChar();
	if (!pc) return;

	sLocation loc = t->getPosition();

	if(!line_of_sight(s, pc->getPosition(), loc, losWallsChimneys | losDoors | losRoofingSlanted))
	{
		client->sysmessage("You cannot throw the potion there!");
		return;
	}
		
	pItem pi=cSerializable::findItemBySerial( t->buffer[0] );
	if ( ! pi ) return;
		
	pi->MoveTo( loc );
	pi->setContainer(0);
	pi->magic=2; //make item unmovable once thrown
	pc->movingFX2(pi, 0x0F0D, 17, 0, 0);
	pi->Refresh();
}

void target_trigger( pClient client, pTarget t )
{
	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if (!pi) return;

	pi->triggerItem(client, TRIGTYPE_TARGET);
}

void target_npcMenu( pClient ps, pTarget t )
{
	pChar pc=ps->currChar();
	if ( ! pc ) return;

	npcs::AddRespawnNPC(pc,t->buffer[0]);
}

/*!
\brief implements the 'telestuff GM command
\author Endymion
*/
void target_telestuff( pClient client, pTarget t )
{
	pChar pc = client->currChar();
	if ( !pc ) return;


	pObject po = objects.findObject( t->getClicked() );
	if( po ) { //clicked on obj to move
		pTarget targ=clientInfo[s]->newTarget( new cLocationTarget() );
		targ->code_callback=target_telestuff;
		targ->buffer[0]=po->getSerial();
		targ->send(client);
		client->sysmessage( "Select location to put this object.");
	} else { //on ground.. so move it

		sLocation loc=t->getPosition();
		loc.z+=tileHeight( t->getModel() );

		uint32_t serial = t->buffer[0];
		if( cSerializable::isCharSerial(serial) ) {
			pChar pt = cSerializable::findCharBySerial( serial );
			if(!pt) return;

			pt->MoveTo( loc );
			pt->teleport();
		} else if ( cSerializable::isItemSerial(serial) ) {
			pItem pi = cSerializable::findItemBySerial( serial );
			if ( ! pi ) return;

			pi->MoveTo(loc);
			pi->Refresh();
		}
	}
}

/*!
\author Luxor
\param s socket to attack
\brief Manages all attack command
*/
void target_allAttack( pClient client, pTarget t )
{
	pChar pc = client->currChar();
	if (!pc) return;

	pChar pc_target = dynamic_cast<pChar>( t->getClicked() );
	if(!pc_target) return;

	NxwCharWrapper sc;
	sc.fillOwnedNpcs( pc, false, true );
	pc->attackStuff(pc_target);
	for( sc.rewind(); !sc.isEmpty(); sc++ )
	{
		pChar pet=sc.getChar();
		if( ! pet ) return;
		npcattacktarget(pet, pc_target);
	}
}

void target_xTeleport( pClient client, pTarget t )
{
	pChar me = client->currChar();
	if ( !me ) return;

	if(pc = dynamic_cast<pChar>( t->getClicked() )) {
		pc->MoveTo( me->getPosition() );
		pc->teleport();
	} else if(pi = dynamic_cast<pItem>( t->getClicked() )) {
		pi->MoveTo( me->getPosition() );
		pi->Refresh();
	}
}
