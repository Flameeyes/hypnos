/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Alchemy stuff
*/

#include "skills/skills.h"
#include "skills/alchemy.h"

/*!
\author Duke
\date 20/04/2000
\brief Helper function for DoPotion()
\param client client of the crafter
\param regid reagent identifier
\param regamount amount of reagents
\param regname name of the reagent

checks if player has enough regs for selected potion and delets them
*/
static bool DoOnePotion(pClient client, uint16_t regid, uint16_t regamount, std::string regname)
{
	pChar pc = NULL;
	if ( ! client || ! (pc = client->currChar()) ) //Luxor
		return;

	if (pc->getAmount(regid) < regamount)
	{
		client->sysmessage("You do not have enough reagents for that potion.");
		return false;
	}
	
	pc->emoteall("*%s starts grinding some %s in the mortar.*", true, pc->getCurrentName().c_str(), regname);
		// LB, the true stops stupid alchemy spam
	pc->delItems(regid, regamount);
	return true;
}

/*!
\author Duke
\brief Determines regs and quantity, creates working sound
	indirectly calls CreatePotion() on success
\param client Client who's creating the potion
\param type Type of the potion
\param sub Subtype of the potion
\param mortar Pointer to the mortar
*/
void Skills::DoPotion(pClient client, uint8_t type, uint8_t sub, pItem mortar)
{
	pChar pc = NULL;
	if ( ! client || ! (pc = client->currChar()) || ! mortar ) //Luxor
		return;

	bool success=false;

	switch((type*10)+sub)
	{
		case 11: success = DoOnePotion(client, 0x0F7B, 1,"blood moss");     break;//agility
		case 12: success = DoOnePotion(client, 0x0F7B, 3,"blood moss");     break;//greater agility
		case 21: success = DoOnePotion(client, 0x0F84, 1,"garlic");         break;//lesser cure
		case 22: success = DoOnePotion(client, 0x0F84, 3,"garlic");         break;//cure
		case 23: success = DoOnePotion(client, 0x0F84, 6,"garlic");         break;//greater cure
		case 31: success = DoOnePotion(client, 0x0F8C, 3,"sulfurous ash");  break;//lesser explosion
		case 32: success = DoOnePotion(client, 0x0F8C, 5,"sulfurous ash");  break;//explosion
		case 33: success = DoOnePotion(client, 0x0F8C,10,"sulfurous ash");  break;//greater explosion
		case 41: success = DoOnePotion(client, 0x0F85, 1,"ginseng");        break;//lesser heal
		case 42: success = DoOnePotion(client, 0x0F85, 3,"ginseng");        break;//heal
		case 43: success = DoOnePotion(client, 0x0F85, 7,"ginseng");        break;//greater heal
		case 51: success = DoOnePotion(client, 0x0F8D, 1,"spider's silk");  break;//night sight
		case 61: success = DoOnePotion(client, 0x0F88, 1,"nightshade");     break;//lesser poison
		case 62: success = DoOnePotion(client, 0x0F88, 2,"nightshade");     break;//poison
		case 63: success = DoOnePotion(client, 0x0F88, 4,"nightshade");     break;//greater poison
		case 64: success = DoOnePotion(client, 0x0F88, 8,"nightshade");     break;//deadly poison
		case 71: success = DoOnePotion(client, 0x0F7A, 1,"black pearl");    break;//refresh
		case 72: success = DoOnePotion(client, 0x0F7A, 5,"black pearl");    break;//total refreshment
		case 81: success = DoOnePotion(client, 0x0F86, 2,"mandrake");       break;//strength
		case 82: success = DoOnePotion(client, 0x0F86, 5,"mandrake");       break;//greater strength
		default:
			 LogError("switch reached default",(type*10)+sub);
			 return;
	}

	if (success)
	{
		tempfx::add(pc, pc, tempfx::ALCHEMY_GRIND, 0, 0, 0); // make grinding sound for a while
		tempfx::add(pc, pc, tempfx::ALCHEMY_GRIND, 0, 3, 0);
		tempfx::add(pc, pc, tempfx::ALCHEMY_GRIND, 0, 6, 0);
		tempfx::add(pc, pc, tempfx::ALCHEMY_GRIND, 0, 9, 0);
		tempfx::add(pc, mortar, tempfx::ALCHEMY_END, type, sub, 0);  // this will indirectly call CreatePotion()
	}
}

/*!
\author Duke
\brief Does the appropriate skillcheck for the potion, creats it
in the mortar on success and tries to put it into a bottle
\param pc character crafter
\param type type of potion
\param sub subtype of potion
\param mortar serial of the mortar
*/
void Skills::CreatePotion(pChar pc, uint8_t type, uint8_t sub, pItem mortar)
{
	pClient client = NULL;
	if ( ! pc || ! (client = pc->getClient()) )
		return;

	int success=0;

	switch((10*type)+sub)
	{
		case 11:success=pc->checkSkill( ALCHEMY,151, 651);break;//agility
		case 12:success=pc->checkSkill( ALCHEMY,351, 851);break;//greater agility
		case 21:success=pc->checkSkill( ALCHEMY,  0, 500);break;//lesser cure
		case 22:success=pc->checkSkill( ALCHEMY,251, 751);break;//cure
		case 23:success=pc->checkSkill( ALCHEMY,651,1151);break;//greater cure
		case 31:success=pc->checkSkill( ALCHEMY, 51, 551);break;//lesser explosion
		case 32:success=pc->checkSkill( ALCHEMY,351, 851);break;//explosion
		case 33:success=pc->checkSkill( ALCHEMY,651,1151);break;//greater explosion
		case 41:success=pc->checkSkill( ALCHEMY,  0, 500);break;//lesser heal
		case 42:success=pc->checkSkill( ALCHEMY,151, 651);break;//heal
		case 43:success=pc->checkSkill( ALCHEMY,551,1051);break;//greater heal
		case 51:success=pc->checkSkill( ALCHEMY,  0, 500);break;//night sight
		case 61:success=pc->checkSkill( ALCHEMY,  0, 500);break;//lesser poison
		case 62:success=pc->checkSkill( ALCHEMY,151, 651);break;//poison
		case 63:success=pc->checkSkill( ALCHEMY,551,1051);break;//greater poison
		case 64:success=pc->checkSkill( ALCHEMY,901,1401);break;//deadly poison
		case 71:success=pc->checkSkill( ALCHEMY,  0, 500);break;//refresh
		case 72:success=pc->checkSkill( ALCHEMY,251, 751);break;//total refreshment
		case 81:success=pc->checkSkill( ALCHEMY,251, 751);break;//strength
		case 82:success=pc->checkSkill( ALCHEMY,451, 951);break;//greater strength

		default:
			LogError("switch reached default");
			return;
	}

	if (! success && !pc->IsGM()) // AC bugfix
	{
		pc->emoteall("*%s tosses the failed mixture from the mortar, unable to create a potion from it.*", false, pc->getCurrentName().c_str());
		return;
	}

	mortar->type=17;
	mortar->more1=type;
	mortar->more2=sub;
	mortar->morex=pc->skill[ALCHEMY];

	if (pc->getAmount(0x0F0E)<1)
	{
		pTarget targ = clientInfo[s]->newTarget( new cItemTarget() );
		targ->code_callback=Skills::target_bottle;
		targ->buffer[0]=pi_mortar->getSerial();
		targ->send( ps );
		ps->sysmsg("Where is an empty bottle for your potion?");
	}
	else
	{
		pc->playSFX(0x0240); // Liquid sfx
		pc->emoteall("*%s pours the completed potion into a bottle.*", 0, pc->getCurrentName().c_str());
		pc->delItems(0x0F0E);
		Skills::PotionToBottle(pc, mortar);
	}
}

/*!
\author Duke
\brief Uses the targeted potion bottle <b>outside</b> the backpack to
pour in the potion from the mortar
\param client client of the crafter
*/
void Skills::target_bottle( pClient ps, pTarget t )
{
	pChar pc=ps->currChar();
	if ( ! pc ) return;

	pItem pi=cSerializable::findItemBySerial( t->getClicked() );
	if ( ! pi ) return;

	pClient client = ps->toInt();

	if(pi->magic==4)
		return;    // Ripper

	if (pi->getId()==0x0F0E)   // an empty potion bottle ?
	{
		pi->ReduceAmount(1);

		pItem pi_mortar=cSerializable::findItemBySerial( t->buffer[0] );
		VALIDATEPI(pi_mortar);

		if (pi_mortar->type==17)
		{
			pc->emoteall("*%s pours the completed potion into a bottle.*", false, pc->getCurrentName().c_str());
			Skills::PotionToBottle(pc, pi_mortar);
		}
	}
	else
		sysmessage(s,"This is not an appropriate container for a potion.");
}

#define CREATEINBACKPACK( ITEM ) pi = item::CreateFromScript( ITEM, pc->getBackpack() );

/*!
\author Endymion
\brief This really creates the potion
\param pc pointer to the crafter's character
\param pi_mortar pointer to the mortar's item
*/
void Skills::PotionToBottle( pChar pc, pItem pi_mortar )
{
	if ( ! pc ) return;

	pClient ps=pc->getClient();
	if( ps==NULL ) return;

	VALIDATEPI(pi_mortar);

	pItem pi=NULL;

	int potionType= (10*pi_mortar->more1)+pi_mortar->more2;

	switch( potionType )    {
		case 11: CREATEINBACKPACK( "$normal_agility_potion" )		break;
		case 12: CREATEINBACKPACK( "$greater_agility_potion" )		break;

		case 21: CREATEINBACKPACK( "$lesser_cure_potion" )		break;
		case 22: CREATEINBACKPACK( "$normal_cure_potion" )		break;
		case 23: CREATEINBACKPACK( "$greater_cure_potion" )		break;

		case 31: CREATEINBACKPACK( "$lesser_explosion_potion" )		break;
		case 32: CREATEINBACKPACK( "$normal_explosion_potion" )		break;
		case 33: CREATEINBACKPACK( "$greater_explosion_potion" )	break;

		case 41: CREATEINBACKPACK( "$lesser_healing_potion" )		break;
		case 42: CREATEINBACKPACK( "$normal_healing_potion" )		break;
		case 43: CREATEINBACKPACK( "$greater_healing_potion" )		break;

		case 51: CREATEINBACKPACK( "$normal_nightsight_potion" )	break;

		case 61: CREATEINBACKPACK( "$lesser_poison_potion" )		break;
		case 62: CREATEINBACKPACK( "$normal_poison_potion" )		break;
		case 63: CREATEINBACKPACK( "$greater_poison_potion" )		break;
		case 64: CREATEINBACKPACK( "$deadly_poison_potion" )		break;

		case 71: CREATEINBACKPACK( "$normal_energy_potion" )		break;
		case 72: CREATEINBACKPACK( "$greater_energy_potion" )		break;

		case 81: CREATEINBACKPACK( "$normal_strength_potion" )		break;
		case 82: CREATEINBACKPACK( "$greater_strength_potion" )		break;
		default:
			LogError("switch reached default into PotionToBottle");
			return;
	}

	if ( ! pi ) return;


	if(!pc->IsGM())

	{
		pi->creator = pc->getCurrentName();

		if (pc->skill[ALCHEMY]>950)
			pi->madewith=ALCHEMY+1;
		else
			pi->madewith=0-ALCHEMY-1;

	} else  {
		pi->creator = "";
		pi->madewith=0;
	}

    pi->Refresh();
    pi_mortar->type=0;
}

//////////////////////////
// name:    AlchemyTarget
// history: unknown, revamped by Duke,21.04.2000
// Purpose: checks for valid reg and brings up gumpmenu to select potion
//          This is called after the user dblclicked a mortar and targeted a reg
//
void Skills::target_alchemy(pClient client, pTarget t )
{
	pChar pc_currchar = ps->currChar();
	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if ( ! pc_currchar || ! pi ) return;

	pItem pack = pc_currchar->getBackpack();    // Get the packitem
	if ( ! pack ) return;

	pClient client = ps->toInt();


	pItem pfbottle=NULL; //candidate of the bottle

	NxwItemWrapper si;
	si.fillItemsInContainer( pack, false );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem piii=si.getItem();
		if( piii && piii->type==0) {
			pfbottle=pi;
			break;
		}
	}

	if (!pfbottle)
	{
		client->sysmessage("There is no bottle in your pack");
		return;
	}

	switch (pi->getId())
	{
	case 0x0F7B: itemmenu( s, 7021 ); break;   // Agility,
	case 0x0F84: itemmenu( s, 7022 ); break;   // Cure, Garlic
	case 0x0F8C: itemmenu( s, 7023 ); break;   // Explosion, Sulfurous Ash
	case 0x0F85: itemmenu( s, 7024 ); break;   // Heal, Ginseng
	case 0x0F8D: itemmenu( s, 7025 ); break;   // Night sight
	case 0x0F88: itemmenu( s, 7026 ); break;   // Poison, Nightshade
	case 0x0F7A: itemmenu( s, 7027 ); break;   // Refresh,
	case 0x0F86: itemmenu( s, 7028 ); break;   // Strength,
	case 0x0E9B: break; // Mortar
	default:
		client->sysmessage("That is not a valid reagent.");
	}
}

