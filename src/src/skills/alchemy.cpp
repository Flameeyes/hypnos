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
\brief Helper function for DoPotion()
\param client client of the crafter
\param regid reagent identifier
\param regamount amount of reagents
\param regname name of the reagent

checks if player has enough regs for selected potion and delets them

\note DoPotion() should check client and character, so we here will assert them
*/
static bool DoOnePotion(pClient client, uint16_t regid, uint16_t regamount, string regname)
{
	pChar pc = NULL;
	assert(client);
	assert( (pc = client->currChar()) );

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
\brief Determines regs and quantity, creates working sound indirectly calls
	CreatePotion() on success
\param client Client who's creating the potion
\param type Type of the potion
\param sub Subtype of the potion
\param mortar Pointer to the mortar
*/
void nSkills::DoPotion(pClient client, uint8_t type, uint8_t sub, pItem mortar)
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
		LogError("Unknown type/subtype combination in nSkills::DoPotion(): type %d subtype %d", type, sub);
		return;
	}

	if (success)
	{
		tempfx::add(pc, pc, tempfx::tmpfxAlchemyGrind, 0, 0, 0); // make grinding sound for a while
		tempfx::add(pc, pc, tempfx::tmpfxAlchemyGrind, 0, 3, 0);
		tempfx::add(pc, pc, tempfx::tmpfxAlchemyGrind, 0, 6, 0);
		tempfx::add(pc, pc, tempfx::tmpfxAlchemyGrind, 0, 9, 0);
		tempfx::add(pc, mortar, tempfx::tmpfxAlchemyEnd, type, sub, 0);  // this will indirectly call CreatePotion()
	}
}

/*!
\brief Does the appropriate skillcheck for the potion, creates it in the mortar
	on success and tries to put it into a bottle
\param pc Player crafter
\param type Type of potion
\param sub Subtype of potion
\param mortar Mortar item
*/
void nSkills::CreatePotion(pPC pc, uint8_t type, uint8_t sub, pItem mortar)
{
	pClient client = NULL;
	if ( ! pc || ! (client = pc->getClient()) )
		return;

	bool success = false;

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
		LogError("Unknown type/subtype combination in nSkills::CreatePotion(): type %d subtype %d", type, sub);
		return;
	}

	if (! success && !pc->isGM()) // AC bugfix
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
		client->sysmessage("Where is an empty bottle for your potion?");
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
\brief Uses the targeted potion bottle \b outside the backpack to pour in the
	potion from the mortar
\param client Crafter's client
*/
void nSkills::target_bottle( pClient client, pTarget t )
{
	pChar pc = client->currChar();
	if ( ! pc ) return;

	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if ( ! pi ) return;

	if(pi->magic==4)
		return;    // Ripper

	//!\todo Fix this using the new string ID
	if (pi->getId() != 0x0F0E)   // an empty potion bottle ?
	{
		client->sysmessage("This is not an appropriate container for a potion.");
		return;
	}
	
	pi->ReduceAmount(1);

	pItem pi_mortar = cSerializable::findItemBySerial( t->buffer[0] );
	if(!pi_mortar) return;

	if (pi_mortar->type != 17)
		return;
		
	pc->emoteall("*%s pours the completed potion into a bottle.*", false, pc->getCurrentName().c_str());
	nSkills::PotionToBottle(pc, pi_mortar);
}

/*!
\brief This really creates the potion
\param pc pointer to the crafter's character
\param pi_mortar pointer to the mortar's item
*/
void nSkills::PotionToBottle( pPC pc, pItem pi_mortar )
{
	pClient ps = NULL;
	if ( ! pc || ! ( ps = pc->getClient() ) || ! pi_mortar )
		return;

	pItem pi = NULL;

	/*
	 * Ok now it's time for some Flameeyes' magic.
	 * pi_mortar->more1.moreb1 represents the main type of the potion
	 * pi_mortar->more2.moreb1 represents the grade of the potion
	 * 
	 * So we have a table to lookup for the main type, and then
	 * two tables to look for the grade of the potion.
	 * 
	 * They are needed because Nightsight has only a grade, instead
	 * Agility, Strength and Energy have only two grades instead of 3
	 * 
	 * Poison has four grades, but this is not a problem for us :)
	*/
	static const char potionMainTypes[][] =
	{
		strNull,
		"item_potion_agility",
		"item_potion_cure",
		"item_potion_explosion",
		"item_potion_healing",
		"item_potion_nightsight",
		"item_potion_poison",
		"item_potion_energy",
		"item_potion_strength"
	};
	
	static const char potionGrades[][] =
	{
		strNull,
		"_lesser",
		"_normal",
		"_greater",
		"_deadly"
	};
	
	switch(pi_mortar->more1.moreb1)
	{
		case 5: // Nightsight
			// We have only a grade :)
			pi_mortar->more1.moreb2 = 2;
			break;
		
		case 1: // Agility
		case 7: // Energy
		case 8: // Strength
			// We have only 2 grades
			if ( ! pi_mortar->more1.moreb2 || pi_mortar->more1.moreb2 > 2 )
				pi_mortar->more1.moreb2 = 1;
			break;
		
		case 6: // Poison
			if ( ! pi_mortar->more1.moreb2 || pi_mortar->more1.moreb2 > 4 )
				pi_mortar->more1.moreb2 = 1;
			break;
			
		default: // The rest
			if ( ! pi_mortar->more1.moreb2 || pi_mortar->more1.moreb2 > 3 )
				pi_mortar->more1.moreb2 = 1;
	}
	
	pi = nArchetypes::createItem(
		string( potionMainTypes[pi_mortar->more1.moreb1] ) +
		string( potionGrades[pi_mortar->more1.moreb2] )
		);
	
	if ( ! pi ) return;

	pi->setContainer(pc->getBody()->getBackpack());

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
	pi_mortar->more1.more=0;
}

/*!
\brief Alchemy targeting function
\param client Client who's performing the target
\param t Called target

This function checks for valid reg and brings up gumpmenu to select potion.
It's called after the user doubleclicked a mortar and targeted a reg.

\todo Require rewrite after the menus are completed
*/
void nSkills::target_alchemy(pClient client, pTarget t)
{
	pChar pc_currchar = client->currChar();
	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if ( ! pc_currchar || ! pi ) return;

	pContainer pack = pc_currchar->getBody()->getBackpack(true);    // Get the packitem
	assert(pack);

	pItem pfbottle = pack->findItem(strEmptyBottleId, true);
	
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
