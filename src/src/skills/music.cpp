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

#include "logsystem.h"
#include "settings.h"
#include "networking/cclient.h"
#include "objects/cpc.h"
#include "objects/cbody.h"
#include "ojbects/citem/ccontainer.h"
#include "skills/skills.h"
#include "skills/music.h"

void nSkills::target_enticement2(pClient client, pTarget t )
{
	pChar pc = client->currChar();
	pChar pc_ftarg = dynamic_cast<pChar>( t->getClicked() );
	if ( ! pc || ! pc_ftarg ) return;

	pItem inst = client->currChar()->getBody()->getBackpack()->getInstrument();
	if ( ! inst )
	{
		client->sysmessage("You do not have an instrument to play on!");
		return;
	}

	if (! ( pc->checkSkill( skEnticement, 0, 1000) && pc->checkSkill( skMusicianship, 0, 1000) ) )
	{
		client->sysmessage("Your music fails to attract them.");
		nSkills::PlayInstrumentPoor(client, inst);
		return;
	}
	
	pChar pc_target = cSerializable::findCharBySerial( t->buffer[0] );
	if ( ! pc_target ) return;
	pc_target->ftargserial = pc_ftarg->getSerial();
	pc_target->npcWander = cNPC::WANDER_FOLLOW;
	client->sysmessage("You play your hypnotic music, luring them near your target.");
	nSkills::PlayInstrumentWell(client, inst);
}

void nSkills::target_enticement1(pClient client, pTarget t )
{
	pChar current=client->currChar();
	pChar pc = dynamic_cast<pChar>( t->getClicked() );
	if ( !current || !pc ) return;

	pItem inst = client->currChar()->getBody()->getBackpack()->getInstrument();
	if (!inst)
	{
		client->sysmessage("You do not have an instrument to play on!");
		return;
	}
	
	if ( pc->isInvul() || pc->shopkeeper )
	{
		client->sysmessage(" You cant entice that npc!");
		return;
	}
	
	if (pc->InGuardedArea())
	{
		client->sysmessage(" You cant do that in town!");
		return;
	}
	
	if ( typeid(pc) == typeid(cNPC) )
	{
		client->sysmessage("You cannot entice other players.");
		return;
	}
		
	pTarget targ= clientInfo[s]->newTarget( new cCharTarget() );
	targ->code_callback = nSkills::target_enticement2;
	targ->buffer[0]= pc->getSerial();
	targ->send( ps );
	client->sysmessage("You play your music, luring them near. Whom do you wish them to follow?");
	PlayInstrumentWell(client, inst);
}

void nSkills::target_provocation2(pClient client, pTarget t )
{
	pChar Victim2 = dynamic_cast<pChar>( t->getClicked() );
	pChar Victim1 = cSerializable::findCharBySerial( t->buffer[0] );
	if( !Victim1 || !Victim2 ) return;

	pChar Player = client->currChar();
	if(!Player) return;

	sLocation charpos= Player->getBody()->getPosition();

	if (Victim2->InGuardedArea())
	{
		client->sysmessage("You cant do that in town!");
		return;
	}
	if (Victim1 == Victim2)
	{
		client->sysmessage("Silly bard! You can't get something to attack itself.");
		return;
	}

	pItem inst = client->currChar()->getBody()->getBackpack()->getInstrument();
	if (!inst)
	{
		client->sysmessage("You do not have an instrument to play on!");
		return;
	}
	
	if ( ! Player->checkSkill( skMusicianship, 0, 1000) )
	{
		nSkills::PlayInstrumentPoor(client, inst);
		client->sysmessage("You play rather poorly and to no effect.");
		return;
	}
	
	nSkills::PlayInstrumentWell(client, inst);
	if (Player->checkSkill( skProvocation, 0, 1000))
	{
		if (Player->InGuardedArea() && nSettings::Server::hasInstantGuards() ) //Luxor
			npcs::SpawnGuard(Player, Player, charpos + (1,0,0)); //ripper
		client->sysmessage("Your music succeeds as you start a fight.");
	}
	else
	{
		client->sysmessage("Your music fails to incite enough anger.");
		Victim2 = Player;		// make the targeted one attack the Player
	}

	Victim1->fight(Victim2);
	Victim1->SetAttackFirst();

	Victim2->fight(Victim1);
	Victim2->ResetAttackFirst();

	char *temp;
	asprintf(&temp, "You see %s attacking %s", Victim1->getCurrentName().c_str(), Victim2->getCurrentName().c_str());

	NxwSocketWrapper sw;
	sw.fillOnline( Victim1, false );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient i = sw.getClient();
		if( !i ) continue;
		
		i->sysmessage(temp);
	}
	free(temp);
}

void nSkills::target_provocation1(pClient client, pTarget t )
{
	pChar current = client->currChar();
	pChar pc = dynamic_cast<pChar>( t->getClicked() );

	if( !current || !pc ) return;

	pItem inst = client->currChar()->getBody()->getBackpack()->getInstrument();
	if (!inst)
	{
		client->sysmessage("You do not have an instrument to play on!");
		return;
	}

	if ( typeid(pc) == typeid(cNPC) )
	{
		client->sysmessage("You cannot provoke other players.");
		return;
	}
	
	pTarget targ=clientInfo[s]->newTarget( new cCharTarget() );
	targ->code_callback=target_provocation2;
	targ->buffer[0]=pc->getSerial();
	targ->send(ps);
	client->sysmessage("You play your music, inciting anger, and your target begins to look furious. Whom do you wish it to attack?");
	PlayInstrumentWell(client, inst);
}

/*!
\brief Execute Peacemaking skill
\param client Client who's performing the skill

This function does all the code for the peacemaking skill, calling the
functions PlayInstrumentPoor() or PlayInstrumentWell() in case of failure or
success.
*/
void nSkills::PeaceMaking(pClient client)
{
	pChar pc = NULL;
	if ( !client || ! (pc = client->currChar()) ) //Luxor
		return;

	pItem inst = client->currChar()->getBody()->getBackpack()->getInstrument();
	if( !inst )
	{
		client->sysmessage("You do not have an instrument to play on!");
		return;
	}
	
	if ( !pc->checkSkill( skPeacemaking, 0, 1000) || !pc->checkSkill( skMusicianship, 0, 1000) )
	{
		nSkills::PlayInstrumentPoor(client, inst);
		client->sysmessage("You attempt to calm everyone, but fail.");
		return;
	}

	nSkills::PlayInstrumentWell(client, inst);
	client->sysmessage("You play your hypnotic music, stopping the battle.");

	NxwCharWrapper sc;
	sc.fillCharsNearXYZ( pc->getBody()->getPosition(), VISRANGE, true, false );
	for( sc.rewind(); !sc.isEmpty(); sc++ )
	{
		pPC pcm = dynamic_cast<pPC>(sc.getChar());
		if ( !pcm || ! pcm->inWarMode() || pc == pcm )
			continue;
		
	
		pcm->getClient()->sysmessage("You hear some lovely music, and forget about fighting.");

		pcm->toggleCombat();
		pcm->targserial = INVALID;
		pcm->attackerserial = INVALID;
		pcm->ResetAttackFirst();
	}
}

/*!
\brief Plays the 'well' sound effect for the instrument
\param client Client who's playing (so the one who effectly play the SFX)
\param pi Instrument to play
\note This function must be called with valid pointers, so they are asserted
*/
void nSkills::PlayInstrumentWell(pClient client, pItem pi)
{
	pChar pc = NULL;
	assert(client);
	assert(pc = client->currChar());
	assert(pi);

	switch(pi->getId())
	{
	case 0x0E9C:    pc->playSFX(0x0038);  break;
	case 0x0E9D:
	case 0x0E9E:    pc->playSFX(0x0052);  break;
	case 0x0EB1:
	case 0x0EB2:    pc->playSFX(0x0045);  break;
	case 0x0EB3:
	case 0x0EB4:    pc->playSFX(0x004C);  break;
	default:
		LogError("nSkills::PlayInstrumentPoor() called for a non-instrument");
	}
}

/*!
\brief Plays the 'poor' sound effect for the instrument
\param client Client who's playing (so the one who effectly play the SFX)
\param pi Instrument to play
\note This function must be called with valid pointers, so they are asserted
*/
void nSkills::PlayInstrumentPoor(pClient client, pItem pi)
{
	pChar pc = NULL;
	assert(client);
	assert(pc = client->currChar());
	assert(pi);

	switch(pi->getId())
	{
	case 0x0E9C:    pc->playSFX( 0x0039);  break;
	case 0x0E9D:
	case 0x0E9E:    pc->playSFX( 0x0053);  break;
	case 0x0EB1:
	case 0x0EB2:    pc->playSFX( 0x0046);  break;
	case 0x0EB3:
	case 0x0EB4:    pc->playSFX( 0x004D);  break;
	default:
		LogError("nSkills::PlayInstrumentPoor() called for a non-instrument");
	}
}
