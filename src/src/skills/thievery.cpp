/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "sndpkg.h"
#include "npcai.h"
#include "skills.h"
#include "inlines.h"
#include "settings.h"
#include "backend/scripting.h"

/*!
\brief Snoop into container
\author Endymion and Flameeyes
\param snooper the snooper
\param cont the contanier
*/
void snooping( pPC snooper, pItem cont )
{
	pChar owner;
	pClient client_snoop, client_owner;

	if ( !snooper || !cont || ! ( owner = cont->getPackOwner() ) )
		return;

	if (snooper == owner) {
		snooper->showContainer(cont);
		return;
	}

	client_snoop = snooper->getClient();
	if(!client_snoop)
		return;

	if (snooper->IsGMorCounselor()) {
		snooper->showContainer(cont);
		return;
	}
	if ( ! snooper->hasInRange(owner, 2) && ! snooper->hasInRange(cont, 2) ) {
		client_snoop->sysmessage("You are too far away!");
		return;
	}
	if ( owner->HasHumanBody() && owner->getOwner() == snooper ) {
		snooper->showContainer(cont);
		return;
	}
	if ( owner->npcaitype == NPCAI_PLAYERVENDOR) {
		snooper->showContainer(cont);
		return;
	}
	
	if ( dynamic_cast<pBody>(cont->getContainer()) == snooper->getBody() )
		return;
	
	pFunctionHandle owner = src->getEvent(cChar::evtChrOnSnooped);
	if ( evt )
	{
		tVariantVector params = tVariantVector(2);
		params[0] = owner->getSerial(); params[1] = snooper->getSerial();
		evt->setParams(params);
		evt->execute();
		if ( evt->isBypassed() )
			return;
	}
	
	pPC pc_owner = dynamic_cast<pPC>(owner);

	snooper->objectdelay=SrvParms->snoopdelay * SECS + getclock();
	if ( owner->IsGMorCounselor())
	{
		client->sysmessage("You can't peek into that container or you'll be jailed.");
		pc_owner->getClient()->sysmessage("%s is trying to snoop you!", snooper->getCurrentName().c_str());
		return;
	}
	else if (snooper->checkSkill( skSnooping, 0, 1000))
	{
		snooper->showContainer(cont);
		client_snoop->sysmessage("You successfully peek into that container.");
	}
	else
	{
		client_snoop->sysmessage("You failed to peek into that container.");
		
		if (!pc_owner)
			owner->talk(s, "Art thou attempting to disturb my privacy?", 0);
		else {
			pc_owner->getClient()->sysmessage("You notice %s trying to peek into your pack!", snooper->getCurrentName().c_str());
		}
		snooper->IncreaseKarma( - nSettings::Skills::getSnoopKarmaLoss() );
		snooper->modifyFame( - nSettings::Skills::getSnoopFameLoss() );
		//!\todo This should be investigated
		snooper->setCrimGrey(ServerScp::g_nSnoopWillCriminal);
	}
}

/*!
\brief Steal something
\author Unknow, completly rewritten by Endymion
\param client the client
\param t target object
*/
void Skills::target_stealing( pClient client, pTarget t )
{
	pPC thief = client->currChar();
	if ( !thief ) return;
	

	AMXEXECSVTARGET( thief->getSerial(),AMXT_SKITARGS,skStealing,AMX_BEFORE);

	//steal a char
	if ( dynamic_cast<pChar>( t->getClicked() ) )
	{
		Skills::target_randomSteal(client, t);
        	return;
	}

	pItem pi = dynamic_cast<pItem>( t->getClicked() );
	if ( ! pi ) return;

	//steal a pickpocket, a steal training dummy
	if( pi->getId() == 0x1E2D || pi->getId() == 0x1E2C )
	{
		Skills::PickPocketTarget(client);
        	return;
	}

	//no stealing for items on layers other than 0 (equipped!) , newbie items, and items not being in containers allowed !
	if ( pi->layer!=0 || pi->isNewbie() || pi->isInWorld() )
	{
		client->sysmessage("You cannot steal that.");
		return;
	}

	pChar victim = pi->getPackOwner();
	if ( ! victim ) return;

	if (victim->npcaitype == NPCAI_PLAYERVENDOR)
	{
		client->sysmessage("You cannot steal from player vendors.");
		return;
	}

	if ( thief == victim || thief == victim->getOwner()) )
	{
		client->sysmessage("You catch yourself red handed.");
		return;
	}

	if (thief->distFrom( victim ) > 1)
	{
		client->sysmessage("You are too far away to steal that item.");
		return;
	}
	
	int result;

	float we = pi->getWeightActual();
	int bonus= (int)( (1800 - we)/5 );
	if ( thief->checkSkill( skStealing,0,(1000-bonus)) )
	{
		// 0 stealing 2 stones, 10  3 stones, 99.9 12 stones, 100 17 stones !!!
		int cansteal = thief->skill[skStealing] > 999 ? 1700 : thief->skill[skStealing] + 200;

		if ( we > cansteal )
		{
			client->sysmessage("That is too heavy.");
			return;
		}

		pFunctionHandle evt = pi->getEvent(cItem::evtItmOnStolen);
		if ( evt )
		{
			tVariantVector params = tVariantVector(3);
			params[0] = pi->getSerial(); params[1] = thief->getSerial();
			params[2] = victim->getSerial();
			evt->setParams(params);
			evt->execute();
			if ( evt->isBypassed() )
				return;
		}
	
		evt = victim->getEvent(cChar::evtChrOnStolen);
		if ( evt )
		{
			tVariantVector params = tVariantVector(2);
			params[0] = victim->getSerial(); params[1] = thief->getSerial();
			evt->setParams(params);
			evt->execute();
			if ( evt->isBypassed() )
				return;
		}
	
		pItem pack= thief->getBackpack();
		if ( ! pack ) return;

		pi->setContainer( pack );

		client->sysmessage("You successfully steal the item.");
		pi->Refresh();

		result=+200;
		//all_items(s); why all item?
	}
	else
	{
		client->sysmessage("You failed to steal the item.");
		result=-200;
		//Only onhide when player is caught!
	}

	if ( rand()%1000 <= ( thief->skill[skStealing] + result )  )
		return;
	
	thief->unHide();
	client->sysmessage("You have been caught!");
	thief->increaseKarma( - nSettings::Skills::getStealKarmaLoss() );
	thief->modifyFame( - nSettings::Skills::getStealFameLoss() );

	if ( victim->IsInnocent() && thief->attackerserial != victim->getSerial() && Guilds->Compare(thief,victim)==0)
		//!\todo should be investigated
		thief->setCrimGrey(ServerScp::g_nStealWillCriminal); //Blue and not attacker and not same guild


	std::string itmname;
	
	if ( pi->getCurrentName().length() )
		itmname = pi->getCurrentName();
	else
		itmname = pi->getName();
	
	pPC victim_PC = dynamic_cast<pPC>(victim);
	if (victim_PC)
		victim_PC->getClient()->sysmessage("You notice %s trying to steal %s from you!", thief->getCurrentName().c_str(), itmname.c_str());
	else
		if( victim->HasHumanBody() )
			victim->talkAll("Guards!! A thief is amoung us!", false);

	char *temp;
	asprintf(&temp,"You notice %s trying to steal %s from %s!", thief->getCurrentName().c_str(), itmname.c_str(), victim->getCurrentName().c_str());

	//send to all player temp = thief are stealing victim if are more intelligent and a bonus of luck :D
	NxwSocketWrapper sw;
	sw.fillOnline( thief, true );
	for( sw.rewind(); !sw.isEmpty(); sw++ ) {

		pClient ps_i=sw.getClient();
		if(!ps_i) continue;

		pChar pc_i=ps_i->currChar();
		if ( pc_i )
			if( (rand()%10+10==17) || ( (rand()%2==1) && (pc_i->in>=thief->in)))
				ps_i->sysmessage(temp);
	}
	free(temp);

	AMXEXECSVTARGET( thief->getSerial(),AMXT_SKITARGS,skStealing,AMX_AFTER);
}

/*!
\brief Steal from a pickpocket, a stealing training dummy
\author Ripper, updated by Endymion
\param ps the client
*/
void Skills::PickPocketTarget(pClient client)
{
//	if( ps == 0 ) return;
	pChar Me = client->currChar();
	if ( ! Me ) return;

	
	// check if over 30 in stealing
	if (Me->skill[skStealing] >= 300)
	{
		client->sysmessage("You learn nothing from practicing here");
		return;
	}
	
	Me->checkSkill( skStealing, 0, 1000);
	// check their skill
	Me->playSFX(0x0249);
}

/*!
\brief Steal random
\author Unknow, updated by Endymion
\param ps the client
\todo add string because it's locked contanier into translate
*/
void Skills::target_randomSteal( pClient client, pTarget t )
{
	pPC thief = client->currChar();
	pChar victim = dynamic_cast<pChar>(t->getClicked());
	if ( ! thief || ! victim )
		return;

	if (thief == victim || thief == victim->getOwner())
	{
		client->sysmessage("You catch yourself red handed.");
		return;
	}

	if (victim->npcaitype == NPCAI_PLAYERVENDOR)
	{
		client->sysmessage("You cannot steal from player vendors.");
		return;
	}

	if (victim->IsGMorCounselor() )
	{
		client->sysmessage("You can't steal from gods.");
		return;
	}

	pItem pack= victim->getBackpack();
	if ( !pack)
	{
		client->sysmessage("bad luck, your victim doesn't have a backpack");
		return;
	}

	client->sysmessage("You reach into %s's pack to steal something ...", victim->getCurrentName().c_str() );

	if ( !thief->hasInRange(victim, 1) )
	{
		client->sysmessage( "... and realise you're too far away.");
		return;
	}
	pItem pi = NULL;

	NxwItemWrapper si;
	si.fillItemsInContainer( pack, false );
	if( si.size()>0 ) {
		int ra=rand()%si.size();
		int c=0;
		for( si.rewind(); !si.isEmpty(); si++ ) {
			c++;
			if( c==ra ) {
				pi=si.getItem();
				break;
			}
		}
	}

	if(!pi) {
		client->sysmessage("... and discover your victim doesn't have any posessions");
		return;
	}


	//Endy can't be not valid after this -^ loop, else error
	if (!pi) return;

	if( pi->isNewbie() )
	{//newbie
		client->sysmessage("... and fail because it is of no value to you.");
		return;
	}

	if(pi->isSecureContainer())
	{
		client->sysmessage("... and fail because it's a locked container.");
		return;
	}

	if ( thief->checkSkill( skStealing,0,999) )
	{
		// 0 stealing 2 stones, 10  3 stones, 99.9 12 stones, 100 17 stones !!!
		int cansteal = thief->skill[skStealing] > 999 ? 1700 : thief->skill[skStealing] + 200;

		if ( pi->getWeightActual() > cansteam )
			client->sysmessage("... and fail because it is too heavy.");
		else
		{
			pFunctionHandle evt = victim->getEvent(cChar::evtChrOnStolen);
			if ( evt )
			{
				tVariantVector params = tVariantVector(2);
				params[0] = victim->getSerial(); params[1] = thief->getSerial();
				evt->setParams(params);
				evt->execute();
				if ( evt->isBypassed() )
					return;
			}
	
		
			pItem thiefpack = thief->getBackpack();
			if (!thiefpack) return;
			pi->setContainer( thiefpack );
			client->sysmessage("... and you succeed.");
			pi->Refresh();
			//all_items(s);
		}
	}
	else
		client->sysmessage(".. and fail because you're not good enough.");

	if ( thief->skill[skStealing] >= rand()%1001 )
		return;
	
	thief->unHide();
	client->sysmessage("You have been caught!");
	thief->IncreaseKarma( - nSettings::Skills::getStealKarmaLoss() );
	thief->modifyFame( - nSettings::Skills::getStealFameLoss() );

	if (victim->IsInnocent() && thief->attackerserial!=victim->getSerial() && Guilds->Compare(thief,victim)==0)//AntiChrist
		thief->setCrimGrey(ServerScp::g_nStealWillCriminal);//Blue and not attacker and not guild

	std::string itmname = "";
	if ( pi->getCurrentName() != "#" )
		itmname = pi->getCurrentName();
	else
		itmname = pi->getName();

	if (dynamic_cast<pNPC>(victim))
		victim->talkAll( "Guards!! A thief is amoung us!",0);
	else {
		pPC victim_PC = dynamic_cast<pPC>(victim);
		victim_PC->getClient()->sysmessage("You notice %s trying to steal %s from you!", thief->getCurrentName().c_str(), itmname.c_str());
	}

	char *temp;
	asprintf(&temp,"You notice %s trying to steal %s from %s!", thief->getCurrentName().c_str(), itmname.c_str(), victim->getCurrentName().c_str());
	
	//send to all player temp = thief are stealing victim if are more intelligent and a bonus of luck :D
	NxwSocketWrapper sw;
	sw.fillOnline( thief, true );
	for( sw.rewind(); !sw.isEmpty(); sw++ ) {

		pClient ps_i=sw.getClient();
		if( ps_i==NULL ) continue;

		pChar pc_i=ps_i->currChar();
		if ( pc_i )
			if( (rand()%10+10==17) || ( (rand()%2==1) && (pc_i->in>=thief->in)))
				ps_i->sysmessage(temp);
	}
	
	free(temp);
}


/*!
\brief lockpicking skill
\author Unknow, rewrite by Endymion
\since 0.53
\param ps the client
*/
void Skills::target_lockpick( pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pContainer chest = dynamic_cast<pContainer>( t->getClicked() );
	pItem pick = cSerializable::findBySerial(t->buffer[0]);
	
	if ( !pc || !chest || !pick )
		return;
	
	AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skLockPicking,AMX_BEFORE);

	pFunctionHandle evt = chest->getEvent(cContainer::evtCntOnLockPick);
	if ( evt )
	{
		tVariantVector params = tVariantVector(2);
		params[0] = chest->getSerial(); params[1] = pc->getSerial();
		evt->setParams(params);
		evt->execute();
		if ( evt->isBypassed() )
			return;
	}
	
	if( !pc->hasInRange(pick, 1) )
	{
		client->sysmessage("You are too far away!");
		return;
	}

	if (chest->magic==4)
	{
		return;
	}

	if( !chest->isSecureContainer() )
	{
		client->sysmessage("That is not locked.");
		return;
	}

	if( !chest->getKeyCode() )
	{
		client->sysmessage("That cannot be unlocked without a key.");
		return;
	}
	
	if(pc->checkSkill( skLockPicking, 0, 1000))
	{
		switch(chest->type)
		{
			case 8: chest->type=1; break;
			case 13: chest->type=12; break;
			case 64: chest->type=63; break;
			default:
				LogError("switch reached default");
				return;
		}
		chest->playSFX(0x0241);
		client->sysmessage("You manage to pick the lock.");
	} else {
		if((rand()%100)>50)
		{
			client->sysmessage( "You broke your lockpick!");
			pick->ReduceAmount(1);
		}
		else
			client->sysmessage( "You fail to open the lock.");
	}

	AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skLockPicking,AMX_AFTER);
}
