/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Musicianship, Provocation, Enticement, and so on
*/

#include "skills/skills.h"

void Skills::target_enticement2(pClient client, pTarget t )
{
	pChar pc=ps->currChar();
	pChar pc_ftarg = dynamic_cast<pChar>( t->getClicked() );
	if ( ! pc || ! pc_ftarg ) return;

	pItem inst = Skills::getInstrument(client);
	if ( ! inst )
	{
		client->sysmessage("You do not have an instrument to play on!");
		return;
	}

	if (pc->checkSkill( skEnticement, 0, 1000) && pc->checkSkill( skMusicianship, 0, 1000) )
	{
		pChar pc_target = cSerializable::findCharBySerial( t->buffer[0] );
		if ( ! pc_target ) return;
		pc_target->ftargserial = pc_ftarg->getSerial();
		pc_target->npcWander = WANDER_FOLLOW;
		client->sysmessage("You play your hypnotic music, luring them near your target.");
		Skills::PlayInstrumentWell(client, inst);
	}
	else
	{
		client->sysmessage("Your music fails to attract them.");
		Skills::PlayInstrumentPoor(client, inst);
	}
}

void Skills::target_enticement1(pClient client, pTarget t )
{

	pChar current=ps->currChar();
	VALIDATEPC(current);

	pChar pc = cSerializable::findCharBySerial( t->getClicked() );
	if ( ! pc ) return;

	pClient client = ps->toInt();

	ITEM inst = Skills::GetInstrument(s);
	if (inst==INVALID)
	{
		sysmessage(s, "You do not have an instrument to play on!");
		return;
	}
	if ( pc->IsInvul() || pc->shopkeeper )
	{
		sysmessage(s," You cant entice that npc!");
		return;
	}
	if (pc->InGuardedArea())
	{
		sysmessage(s," You cant do that in town!");
		return;
	}
	if (!pc->npc)
		sysmessage(s, "You cannot entice other players.");
	else
	{
		pTarget targ= clientInfo[s]->newTarget( new cCharTarget() );
		targ->code_callback = Skills::target_enticement2;
		targ->buffer[0]= pc->getSerial();
		targ->send( ps );
		ps->sysmsg("You play your music, luring them near. Whom do you wish them to follow?");
		PlayInstrumentWell(s, inst);
	}
}

void target_provocation2(pClient client, pTarget t )
{
	pChar Victim2 = cSerializable::findCharBySerial( t->getClicked() );
	VALIDATEPC(Victim2);

	pChar Player = ps->currChar();
	VALIDATEPC(Player);
	Location charpos= Player->getPosition();

	pChar Victim1 = cSerializable::findCharBySerial( t->buffer[0] );
	VALIDATEPC(Victim1);

	pClient client =ps->toInt();

	if (Victim2->InGuardedArea())
	{
		sysmessage(s,"You cant do that in town!");
		return;
	}
	if (Victim1->isSameAs(Victim2))
	{
		sysmessage(s, "Silly bard! You can't get something to attack itself.");
		return;
	}

	int inst = Skills::GetInstrument(s);
	if (inst==INVALID)
	{
		sysmessage(s, "You do not have an instrument to play on!");
		return;
	}
	if (Player->checkSkill( skMusicianship, 0, 1000))
	{
		Skills::PlayInstrumentWell(s, inst);
		if (Player->checkSkill( skProvocation, 0, 1000))
		{
			if (Player->InGuardedArea() && ServerScp::g_nInstantGuard == 1) //Luxor
				npcs::SpawnGuard(Player, Player, charpos.x+1, charpos.y, charpos.z); //ripper
			sysmessage(s, "Your music succeeds as you start a fight.");
		}
		else
		{
			sysmessage(s, "Your music fails to incite enough anger.");
			Victim2 = Player;		// make the targeted one attack the Player
		}

		Victim1->fight(Victim2);
		Victim1->SetAttackFirst();

		Victim2->fight(Victim1);
		Victim2->ResetAttackFirst();

		char temp[500];
		sprintf(temp, "* You see %s attacking %s *", Victim1->getCurrentName().c_str(), Victim2->getCurrentName().c_str());

		NxwSocketWrapper sw;
		sw.fillOnline( Victim1, false );
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			pClient i = sw.getClient();
			if( !i ) continue;
			
			itemmessage(i, temp, Victim1->getSerial());
		}
	}
	else
	{
		Skills::PlayInstrumentPoor(s, inst);
		sysmessage(s, "You play rather poorly and to no effect.");
	}
}

void Skills::target_provocation1(pClient client, pTarget t )
{
	pChar current=ps->currChar();
	VALIDATEPC(current);

	pChar pc = cSerializable::findCharBySerial( t->getClicked() );
	if ( ! pc ) return;

	pClient client =ps->toInt();

	int inst = Skills::GetInstrument(s);
	if (inst==INVALID)
	{
		sysmessage(s, "You do not have an instrument to play on!");
		return;
	}

	if (!pc->npc)
		sysmessage(s, "You cannot provoke other players.");
	else
	{
		pTarget targ=clientInfo[s]->newTarget( new cCharTarget() );
		targ->code_callback=target_provocation2;
		targ->buffer[0]=pc->getSerial();
		targ->send( ps );
		ps->sysmsg( "You play your music, inciting anger, and your target begins to look furious. Whom do you wish it to attack?");
		PlayInstrumentWell(s, inst);
	}
}
