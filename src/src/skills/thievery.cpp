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
#include "nxw_utils.h"
#include "weight.h"
#include "set.h"
#include "skills.h"
#include "classes.h"
#include "inlines.h"
#include "range.h"
#include "settings.h"

/*!
\brief Snoop into container
\author Endymion and Flameeyes
\param snooper the snooper
\param cont the contanier
*/
void snooping( pPC snooper, pItem cont )
{
	pChar owner;
	if ( ! snooper || ! cont || ! snooper->getClient() || ! ( owner = cont->getPackOwner() ) )
		return;

	if (snooper == owner) {
		snooper->showContainer(cont);
		return;
	}
	if (snooper->IsGMorCounselor()) {
		snooper->showContainer(cont);
		return;
	}
	if ( ! snooper->hasInRange(owner, 2) && ! snooper->hasInRange(cont, 2) ) {
		snooper->sysmsg("You are too far away!");
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
	
	snooper->objectdelay=SrvParms->snoopdelay * MY_CLOCKS_PER_SEC + getclock();
	if ( owner->IsGMorCounselor())
	{
		snooper->sysmsg("You can't peek into that container or you'll be jailed.");// AntiChrist
		owner->sysmsg("%s is trying to snoop you!", snooper->getCurrentName().c_str());
		return;
	}
	else if (snooper->checkSkill( skSnooping, 0, 1000))
	{
		snooper->showContainer(cont);
		snooper->sysmsg("You successfully peek into that container.");
	}
	else
	{
		snooper->sysmsg("You failed to peek into that container.");
		if ( owner->npc )
			owner->talk(s, "Art thou attempting to disturb my privacy?", 0);
		else {
			owner->sysmsg("You notice %s trying to peek into your pack!", snooper->getCurrentName().c_str());
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
\param ps the client
*/
void Skills::target_stealing( NXWCLIENT ps, pTarget t )
{
	pChar thief = ps->currChar();
	VALIDATEPC(thief);
	uint32_t target_serial = t->getClicked();

	AMXEXECSVTARGET( thief->getSerial(),AMXT_SKITARGS,skStealing,AMX_BEFORE);

	//steal a char
	if ( isCharSerial(target_serial) )
	{
		Skills::target_randomSteal(ps,t);
        	return;
	}

	const pItem pi = cSerializable::findItemBySerial( target_serial );
	if ( ! pi ) return;

	//steal a pickpocket, a steal training dummy
	if( pi->getId() == 0x1E2D || pi->getId() == 0x1E2C )
	{
		Skills::PickPocketTarget(ps);
        	return;
	}

	//no stealing for items on layers other than 0 (equipped!) , newbie items, and items not being in containers allowed !
	if ( pi->layer!=0 || pi->isNewbie() || pi->isInWorld() )
	{
       	thief->sysmsg("You cannot steal that.");
       	return;
	}

	pChar victim = pi->getPackOwner();
	VALIDATEPC(victim);

	if (victim->npcaitype == NPCAI_PLAYERVENDOR)
	{
		thief->sysmsg("You cannot steal from player vendors.");
       	return;
	}

	if ( (thief->getSerial() == victim->getSerial()) || (thief->getSerial()==victim->getOwnerSerial32()) )
	{
		thief->sysmsg("You catch yourself red handed.");
		return;
	}

	if (thief->distFrom( victim ) > 1)
	{
		thief->sysmsg("You are too far away to steal that item.");
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
		thief->sysmsg("That is too heavy.");
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
		VALIDATEPI(pack);

		pi->setContainer( pack );

		thief->sysmsg("You successfully steal the item.");
		pi->Refresh();

		result=+200;
		//all_items(s); why all item?
	}
	else
	{
		thief->sysmsg("You failed to steal the item.");
		result=-200;
		//Only onhide when player is caught!
	}

	if ( rand()%1000 <= ( thief->skill[skStealing] + result )  )
		return;
	
	thief->unHide();
	thief->sysmsg("You have been caught!");
	thief->increaseKarma( - nSettings::Skills::getStealKarmaLoss() );
	thief->modifyFame( - nSettings::Skills::getStealFameLoss() );

	if ( victim->IsInnocent() && thief->attackerserial != victim->getSerial() && Guilds->Compare(thief,victim)==0)
		//!\todo should be investigated
		thief->setCrimGrey(ServerScp::g_nStealWillCriminal); //Blue and not attacker and not same guild


	std::string itmname;
	
	if ( pi->getCurrentName() != "#" )
		itmname = pi->getCurrentName();
	else
		itmname = pi->getName();
	
	if ( victim->npc )
		if( victim->HasHumanBody() )
			victim->talkAll("Guards!! A thief is amoung us!", false);
	else
		victim->sysmsg("You notice %s trying to steal %s from you!", thief->getCurrentName().c_str(), itmname.c_str());

	char *temp;
	asprintf(&temp,"You notice %s trying to steal %s from %s!", thief->getCurrentName().c_str(), itmname.c_str(), victim->getCurrentName().c_str());

	//send to all player temp = thief are stealing victim if are more intelligent and a bonus of luck :D
	NxwSocketWrapper sw;
	sw.fillOnline( thief, true );
	for( sw.rewind(); !sw.isEmpty(); sw++ ) {

		NXWCLIENT ps_i=sw.getClient();
		if(ps_i==NULL ) continue;

		pChar pc_i=ps_i->currChar();
		if ( pc_i )
			if( (rand()%10+10==17) || ( (rand()%2==1) && (pc_i->in>=thief->in)))
				pc_i->sysmsg(temp);
	}
	free(temp);

	AMXEXECSVTARGET( thief->getSerial(),AMXT_SKITARGS,skStealing,AMX_AFTER);
}

/*!
\brief Steal from a pickpocket, a stealing training dummy
\author Ripper, updated by Endymion
\param ps the client
*/
void Skills::PickPocketTarget(NXWCLIENT ps)
{
	if( ps == 0 ) return;
	pChar Me = ps->currChar();
	VALIDATEPC(Me);

	if (Me->skill[skStealing] < 300)
	// check if under 30 in stealing
	{
		Me->checkSkill( skStealing, 0, 1000);
		// check their skill
		Me->playSFX(0x0249);
		// rustling sound..dont know if right but it works :)
	}
	else
        Me->sysmsg("You learn nothing from practicing here");
        	// if over 30 Stealing..dont learn.
}

/*!
\brief Steal random
\author Unknow, updated by Endymion
\param ps the client
\todo add string because it's locked contanier into translate
*/
void Skills::target_randomSteal( NXWCLIENT ps, pTarget t )
{

	pChar thief=ps->currChar();
	VALIDATEPC(thief);
	pChar victim = cSerializable::findCharBySerial( t->getClicked() );
	VALIDATEPC(victim);


	if (thief->getSerial() == victim->getSerial() || thief->getSerial()==victim->getOwnerSerial32())
	{
		thief->sysmsg("You catch yourself red handed.");
		return;
	}

	if (victim->npcaitype == NPCAI_PLAYERVENDOR)
	{
		thief->sysmsg("You cannot steal from player vendors.");
		return;
	}

	if (victim->IsGMorCounselor() )
	{
		thief->sysmsg( "You can't steal from gods.");
		return;
	}

	pItem pack= victim->getBackpack();
	if ( !pack)
	{
		thief->sysmsg("bad luck, your victim doesn't have a backpack");
		return;
	}

	thief->sysmsg("You reach into %s's pack to steal something ...", victim->getCurrentName().c_str() );

	if ( !thief->hasInRange(victim, 1) )
	{
		thief->sysmsg( "... and realise you're too far away.");
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

	if( pi==NULL ) {
		thief->sysmsg("... and discover your victim doesn't have any posessions");
		return;
	}


	//Endy can't be not valid after this -^ loop, else error
	if ( ! pi ) return;

	if( pi->isNewbie() )
	{//newbie
		thief->sysmsg("... and fail because it is of no value to you.");
		return;
	}

	if(pi->isSecureContainer())
	{
		thief->sysmsg("... and fail because it's a locked container.");
		return;
	}

	if ( thief->checkSkill( skStealing,0,999) )
	{
		// 0 stealing 2 stones, 10  3 stones, 99.9 12 stones, 100 17 stones !!!
		int cansteal = thief->skill[skStealing] > 999 ? 1700 : thief->skill[skStealing] + 200;

		if ( pi->getWeightActual() > cansteam )
			thief->sysmsg("... and fail because it is too heavy.");
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
			if ( ! thiefpack ) return;
			pi->setContainer( thiefpack );
			thief->sysmsg("... and you succeed.");
			pi->Refresh();
			//all_items(s);
		}
	}
	else
		thief->sysmsg(".. and fail because you're not good enough.");

	if ( thief->skill[skStealing] >= rand()%1001 )
		return;
	
	thief->unHide();
	thief->sysmsg("You have been caught!");
	thief->IncreaseKarma( - nSettings::Skills::getStealKarmaLoss() );
	thief->modifyFame( - nSettings::Skills::getStealFameLoss() );

	if (victim->IsInnocent() && thief->attackerserial!=victim->getSerial() && Guilds->Compare(thief,victim)==0)//AntiChrist
		thief->setCrimGrey(ServerScp::g_nStealWillCriminal);//Blue and not attacker and not guild

	std::string itmname = "";
	if ( pi->getCurrentName() != "#" )
		itmname = pi->getCurrentName();
	else
		itmname = pi->getName();

	if ( victim->npc)
		victim->talkAll( "Guards!! A thief is amoung us!",0);
	else
		victim->sysmsg("You notice %s trying to steal %s from you!", thief->getCurrentName().c_str(), itmname.c_str());

	char *temp;
	asprintf(&temp,"You notice %s trying to steal %s from %s!", thief->getCurrentName().c_str(), itmname.c_str(), victim->getCurrentName().c_str());
	
	//send to all player temp = thief are stealing victim if are more intelligent and a bonus of luck :D
	NxwSocketWrapper sw;
	sw.fillOnline( thief, true );
	for( sw.rewind(); !sw.isEmpty(); sw++ ) {

		NXWCLIENT ps_i=sw.getClient();
		if( ps_i==NULL ) continue;

		pChar pc_i=ps_i->currChar();
		if ( pc_i )
			if( (rand()%10+10==17) || ( (rand()%2==1) && (pc_i->in>=thief->in)))
				sysmessage(ps_i->toInt(),temp);
	}
	
	free(temp);
}


/*!
\brief lockpicking skill
\author Unknow, rewrite by Endymion
\since 0.53
\param ps the client
*/
void Skills::target_lockpick( NXWCLIENT ps, pTarget t )
{

	pChar pc = ps->currChar();
	pContainer chest = dynamic_cast<pContainer>(cSerializable::findBySerial( t->getClicked() ));
	pItem pick = cSerializable::findBySerial(t->buffer[0]);
	
	if ( ! pc || ! pi || ! pick )
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
		pc->sysmsg("You are too far away!");
		return;
	}

	if (chest->magic==4)
	{
		return;
	}

	if(!chest->isSecureContainer())
	{
		pc->sysmsg("That is not locked.");
		return;
	}

	if( ! chest->getKeyCode() )
	{
		pc->sysmsg("That cannot be unlocked without a key.");
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
		soundeffect3(chest, 0x0241);
		pc->sysmsg("You manage to pick the lock.");
	} else {
		if((rand()%100)>50)
		{
			pc->sysmsg( "You broke your lockpick!");
			pick->ReduceAmount(1);
		}
		else
			pc->sysmsg( "You fail to open the lock.");
	}

	AMXEXECSVTARGET( pc->getSerial(),AMXT_SKITARGS,skLockPicking,AMX_AFTER);
}
