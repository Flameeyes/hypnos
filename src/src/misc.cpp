/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "misc.h"

/*!
\brief direction from character a to position x,y
\return int direction
\param pc first character
\param x X-coordinate
\param y Y-coordinate
*/
int chardirxyz(pChar pc, int x, int y)
{
	if ( !pc ) return INVALID;

	int dir,xdif,ydif;

	xdif = x - pc->getPosition().x;
	ydif = y - pc->getPosition().y;

	if ((xdif==0)&&(ydif<0)) dir=0;
	else if ((xdif>0)&&(ydif<0)) dir=1;
	else if ((xdif>0)&&(ydif==0)) dir=2;
	else if ((xdif>0)&&(ydif>0)) dir=3;
	else if ((xdif==0)&&(ydif>0)) dir=4;
	else if ((xdif<0)&&(ydif>0)) dir=5;
	else if ((xdif<0)&&(ydif==0)) dir=6;
	else if ((xdif<0)&&(ydif<0)) dir=7;
	else dir=INVALID;

	return dir;
}


int fielddir(pChar pc, int x, int y, int z)
{
//WARNING: unreferenced formal parameter z

	if ( !pc ) return 0;

	int dir=chardirxyz(pc, x, y);
	switch (dir)
	{
	case 0:
	case 4:
		return 0;
	case 2:
	case 6:
		return 1;
	case 1:
	case 3:
	case 5:
	case 7:
	case INVALID:
		switch(pc->dir) //crashfix, LB
		{
		case 0:
		case 4:
			return 0;

		case 2:
		case 6:
			return 1;

		case 1:
		case 3:
		case 5:
		case 7:
			return 1;

		default:
			LogError("Switch fallout. misc.cpp, fielddir()\n"); //Morrolan
			return 0;
		}
	default:
		LogError("Switch fallout. misc.cpp, fielddir()\n"); //Morrolan
		return 0;
	}
}

/*!
\brief makes an npc attacking someone
\author Luxor
\param pc the npc attacker
\param pc_target the victim
*/
void npcattacktarget(pChar pc, pChar pc_target)
{
	if (
		! pc || ! pc_target ||
		! pc->npc ||
		pc->isDead() || pc_target->isDead() ||
		pc == pc_target ||
		!pc->losFrom(pc_target)
	) return;
	
	pFunctionHandle evt;
	evt = pc->getEvent(cChar::evtChrOnBeginAttack);
	if (evt) {
		tVariantVector params = tVariantVector(2);
		params[0] = pc->getSerial(); params[1] = pc_target->getSerial();
		evt->setParams(params);
		evt->execute();
		if( evt->isBypassed() )
			return;
	}

	evt = pc->getEvent(cChar::evtChrOnBeginDefence);
	if (evt) {
		tVariantVector params = tVariantVector(2);
		params[0] = pc->getSerial(); params[1] = pc_target->getSerial();
		evt->setParams(params);
		evt->execute();
		if( evt->isBypassed() )
			return;
	}

	pc->playMonsterSound(SND_STARTATTACK);

	pc->targserial = pc_target->getSerial();
	pc->attackerserial = pc_target->getSerial();
	pc->SetAttackFirst();

	if ( !pc->war )
		pc->toggleCombat();
	pc->setNpcMoveTime();

	pChar pc_target_targ = cSerializable::findCharBySerial(pc_target->targserial);
	if ( !pc_target_targ || pc_target_targ->dead || pc_target->distFrom(pc_target_targ) > 15 ) {
		if (!pc_target->npc && pc_target->war) {
			pc_target->targserial = pc->getSerial();
			pc_target->attackerserial = pc->getSerial();
		} else if (pc_target->npc) {
			if ( !pc_target->war )
				pc_target->toggleCombat();

			pc_target->targserial = pc->getSerial();
			pc_target->attackerserial = pc->getSerial();
			pc_target->setNpcMoveTime();
		}
	}

	pc->emoteall( "You see %s attacking %s!", 1, pc->getCurrentName().c_str(), pc_target->getCurrentName().c_str() );

}

void usepotion(pChar pc, pItem pi)
{
	int x;

	if ( ! pc ) return;

	pPC tmp;
	pClient client = (tmp = dynamic_cast<pPC>(pc))? tmp->getClient() : NULL;

	switch(pi->morey)
	{
	case 1: // Agility Potion
		staticFX(pc, 0x373A, 0, 15);
		switch(pi->morez)
		{
		case 1:
			tempfx::add(pc, pc, tempfx::SPELL_AGILITY, 5+RandomNum(1,10), 0, 0, 120);
			if(client) client->sysmessage("You feel more agile!");
			break;
		case 2:
			tempfx::add(pc, pc, tempfx::SPELL_AGILITY, 10+RandomNum(1,20), 0, 0, 120);
			if(client) client->sysmessage("You feel much more agile!");
			break;
		default:
			ErrOut("Switch fallout. hypnos.cpp, usepotion()\n");
			return;
		}
		pc->playSFX(0x01E7);
		if (s!=INVALID)
			pc->updateStats(2);
		break;

	case 2: // Cure Potion
		if ( pc->getBody()->getPoisoned() == poisonNone )
			if(client) client->sysmessage("The potion had no effect.");
		else
		{
			switch(pi->morez)
			{
			case 1:
				x=RandomNum(1,100);
				if (pc->getBody()->getPoisoned() == poisonWeak && x<81) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonNormal && x<41) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonGreater && x<21) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonDeadly && x< 6) pc->getBody()->setPoisoned(poisonNone);
				break;
			case 2:
				x=RandomNum(1,100);
				if (pc->getBody()->getPoisoned() == poisonWeak) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonNormal && x<81) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonGreater && x<41) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonDeadly && x<21) pc->getBody()->setPoisoned(poisonNone);
				break;
			case 3:
				x=RandomNum(1,100);
				if (pc->getBody()->getPoisoned() == poisonWeak) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonNormal) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonGreater && x<81) pc->getBody()->setPoisoned(poisonNone);
				if (pc->getBody()->getPoisoned() == poisonDeadly && x<61) pc->getBody()->setPoisoned(poisonNone);
				break;
			default:
				ErrOut("Switch fallout. hypnos.cpp, usepotion()\n"); //Morrolan
				return;
			}
			if (pc->poisoned)
				if(client) client->sysmessage("The potion was not able to cure this poison.");
			else
			{
				staticFX(pc, 0x373A, 0, 15);
				pc->playSFX(0x01E0); //cure sound - SpaceDog
				if(client) client->sysmessage("The poison was cured.");
			}
		}
		impowncreate(s,pc,1); //Lb, makes the green bar blue or the blue bar blue !
		break;

	case 3: {// Explosion Potion
		if (region[pc->region].priv&0x01) // Ripper 11-14-99
		{
			if(client) client->sysmessage(" You cant use that in town!");
			return;
		}
		if(client) client->sysmessage("Now would be a good time to throw it!");
		tempfx::add(pc, pc, tempfx::EXPLOTIONMSG, 0, 1, 3);
		tempfx::add(pc, pc, tempfx::EXPLOTIONMSG, 0, 2, 2);
		tempfx::add(pc, pc, tempfx::EXPLOTIONMSG, 0, 3, 1);
		tempfx::add(pc, pi, tempfx::EXPLOTIONEXP, 0, 4, 0);

		pTarget targ= clientInfo[s]->newTarget( new cTarget() );
		targ->code_callback=target_expPotion;
		targ->buffer[0]= pi->getSerial();
		targ->send( getClientFromSocket(s) );
		client->sysmessage( "*throw*" );
		return;
	}
	case 4: // Heal Potion
		switch(pi->morez)
		{
		case 1:
			pc->hp=qmin(pc->hp+5+RandomNum(1,5)+pc->skill[17]/100,pc->getStrength());
			if(client) client->sysmessage("You feel better!");
			break;
		case 2:
			pc->hp=qmin(pc->hp+15+RandomNum(1,10)+pc->skill[17]/50,pc->getStrength());
			if(client) client->sysmessage("You feel more healty!");
			break;
		case 3:
			pc->hp=qmin(pc->hp+20+RandomNum(1,20)+pc->skill[17]/40, pc->getStrength());
			if(client) client->sysmessage("You feel much more healty!");
			break;

		default:
			ErrOut("Switch fallout. hypnos.cpp, usepotion()\n"); //Morrolan
			return;
		}

		if (client)
			pc->updateStats(0);

		staticFX(pc, 0x376A, 9, 6); // Sparkle effect
		pc->playSFX(0x01F2); //Healing Sound - SpaceDog
		break;

	case 5: // Night Sight Potion
		staticFX(pc, 0x376A, 9, 6);
		tempfx::add(pc, pc, tempfx::SPELL_LIGHT, 0, 0, 0,(720*secondsperuominute*SECS));
		pc->playSFX(0x01E3);
		break;

	case 6: // Poison Potion
		if(pc->poisoned < (PoisonType)pi->morez)
			pc->poisoned=(PoisonType)pi->morez;
		if(pi->morez>4)
			pi->morez=4;
		pc->poisonwearofftime=getclock()+(SECS*SrvParms->poisontimer); // lb, poison wear off timer setting
		impowncreate(s,pc,1); //Lb, sends the green bar !
		pc->playSFX(0x0246); //poison sound - SpaceDog
		if(client) client->sysmessage("You poisoned yourself! *sigh*"); //message -SpaceDog
		break;

	case 7: // Refresh Potion
		switch(pi->morez)
		{
			case 1:
				pc->stm=qmin(pc->stm+20+RandomNum(1,10), pc->dx);
				if(client) client->sysmessage("You feel more energetic!");
				break;

			case 2:
				pc->stm=qmin(pc->stm+40+RandomNum(1,30), pc->dx);
				if(client) client->sysmessage("You feel much more energetic!");
				break;

			default:
				ErrOut("Switch fallout. hypnos.cpp, usepotion()\n"); //Morrolan
				return;
		}
		if (s!=INVALID)
			pc->updateStats(2);
		
		staticFX(pc, 0x376A, 9, 6);
		pc->playSFX(0x01F2); //Healing Sound
		break;

	case 8: // Strength Potion
		staticFX(pc, 0x373A, 0, 15);
		switch(pi->morez)
		{
		case 1:
			tempfx::add(pc, pc, tempfx::SPELL_STRENGHT, 5+RandomNum(1,10), 0, 0, 120);
			if(client) client->sysmessage("You feel more strong!");
			break;
		case 2:
			tempfx::add(pc, pc, tempfx::SPELL_STRENGHT, 10+RandomNum(1,20), 0, 0, 120);
			if(client) client->sysmessage("You feel much more strong!");
			break;
		default:
			ErrOut("Switch fallout. hypnos.cpp, usepotion()\n");
			return;
		}
		pc->playSFX(0x01EE);
		break;

	case 9: // Mana Potion
		switch(pi->morez)
		{
		case 1:
			pc->mn=qmin(pc->mn+10+pi->morex/100, (unsigned)pc->in);
			break;

		case 2:
			pc->mn=qmin(pc->mn+20+pi->morex/50, (unsigned)pc->in);
			break;

		default:
			ErrOut("Switch fallout. hypnos.cpp, usepotion()\n");
			return;
		}
		if (client)
			pc->updateStats(1);
		staticFX(pc, 0x376A, 9, 6); // Sparkle effect
		pc->playSFX(0x01E7); //agility sound - SpaceDog
		break;

	case 10: //LB's LSD potion, 5'th november 1999
		if (pi->getId()!=0x1841) return; // only works with an special flask
		if (client) return;
		if( clientInfo[s]->lsd )
		{
			if(client) client->sysmessage("no,no,no,cant you get enough ?");
			return;
		}
		tempfx::add(pc, pc, tempfx::LSD, 60+RandomNum(1,120), 0, 0); // trigger effect
		staticFX(pc, 0x376A, 9, 6); // Sparkle effect
		pc->playSFX(0x00F8, true); // lsd sound :)
		break;

	default:
		ErrOut("Switch fallout. hypnos.cpp, usepotion()\n"); //Morrolan
		return;
	}

	pc->playSFX(0x0030);
	if (pc->HasHumanBody() && !pc->isMounting())
		pc->playAction(0x22);

	pi->ReduceAmount( 1 );

	if (pi->morey!=3)
	{
		int lsd=pi->morey; // save morey before overwritten

		pi = new cItem(cItem::nextSerial());

		pi->setId( 0x0F0E );

		if (lsd==10) // empty Lsd potions
		{
			pi->setId( 0x183d );
		}

		pi->pileable=1;

		pItem pack=pc->getBackpack();
		if (pack) {
			pack->AddItem( pi );
		}
		else {
			pi->MoveTo( pc->getPosition() );
			pi->setDecay();
			pi->Refresh();
		}
	}
}

/*!
\todo backport into cChar
*/
void callguards( pChar caller )
{
	if ( !caller )
		return;

	if( !(region[caller->region].priv&0x01 ) || !SrvParms->guardsactive || !TIMEOUT( caller->antiguardstimer ) || caller->dead )
		return;

	caller->antiguardstimer=getclock()+(SECS*10);

	/*
	Sparhawk:
	1. when instant guard is set and offender nearby caller spawn guard near caller and leave attacking to checkAI
	2. when instant guard is not set and offender nearby caller walk toward caller and leave attacking to checkAI
	*/
	bool offenders = false;
	vector < pChar > guards;

	NxwCharWrapper sc;
	sc.fillCharsNearXYZ( caller->getPosition(), VISRANGE, true, false  );
	for( sc.rewind(); !sc.isEmpty(); sc++ ) {

		pChar character=sc.getChar();
		if(	! character ||
			caller == character ||
			caller->distFrom(character) > 15 ||
			character->isDead() ||
			character->isHidden() )
				continue;
			
		if ((!character->IsInnocent() || character->npcaitype == NPCAI_EVIL) && !character->IsHidden() )
			offenders = true;
		else
			if ((character->npcaitype == NPCAI_TELEPORTGUARD || character->npcaitype == NPCAI_GUARD) && !character->war && character->npcWander != WANDER_FOLLOW)
				guards.push_back( character );
	}
	
	if ( ! offenders ) return;
	
	if ( guards.empty() && nSettings::Server::hasInstantGuards() )
	{
		pNPC guard = npcs::AddNPCxyz( caller->getSocket(), region[caller->region].guardnum[(rand()%10)+1], caller->getPosition());

		if ( guard )
		{
			guard->npcaitype=NPCAI_TELEPORTGUARD;
			guard->npcWander=WANDER_FREELY_CIRCLE;
			guard->setNpcMoveTime();
			guard->summontimer = getclock() + SECS * 25 ;

			guard->playSFX( 0x01FE );
			staticFX(guard, 0x372A, 9, 6);

			guard->teleport();
			guard->talkAll("Don't fear, help is near", false );
		}
	}
	else
	{
		pChar guard;
		while( !guards.empty() )
		{
			guard = guards.back();
			guard->oldnpcWander = guard->npcWander;
			guard->npcWander = WANDER_FOLLOW;
			guard->ftargserial = caller->getSerial();
			guard->antiguardstimer=getclock()+(SECS*10); // Sparhawk this should become server configurable
			guard->talkAll("Don't fear, help is on the way", false );
			//guard->antispamtimer = getclock()+SECS*5;
			guards.pop_back();
		}
	}
}
