/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Begging methods implementation
*/

TIMERVAL Begging::timer;
uint32_t Begging::range;
std::string Begging::text[3];

void Begging::initialize()
{
	timer = 300;
	range = 3;
	text[0] = "Could thou spare a few coins?";
	text[1] = "Hey buddy can you spare some gold?";
	text[2] = "I have a family to feed, think of the children.";
}

void Begging::target(NXWCLIENT ps, pTarget t)
{
	pChar pcc = ps->currChar();
	if ( ! pcc )
		return;
	pChar pc = t->getClicked()->toChar();
	if ( ! pc )
		return;
	
	// TODO! Convertire a interfaccia eventi migliorata
	// AMXEXECSVTARGET( pcc->getSerial(),AMXT_SKITARGS,skBegging,AMX_BEFORE);
	
	if(pc->IsOnline())
	{
		sysmessage(s,tr("Maybe you should just ask."));
		return;
	}
	
	if( pc->distFrom(pcc) >= Begging::range)
	{
		sysmessage(s,tr("You are not close enough to beg."));
		return;
        }

	// Not used on human
	if( !pc->getBody()->isHuman() || !pc->getBody()->getIntelligence() )
	{
		sysmessage(s, "That would be foolish.");
		return;
	}

	if (pc->begging_timer >= getClock())
	{
		pc->talk(s,"Annoy someone else!",1);
		return;
	}
	
	pcc->talkAll( Begging::text[rand()%3], 0); // npcemoteall?
	
	if (!pcc->checkSkill( skiBegging, 0, 1000))
	{
		sysmessage(s,"They seem to ignore your begging plees.");
		return;
	}
	
	SetTimerSec(&pc->begging_timer,Begging::timer);
	
	uint16_t n = pc->getBody()->getSkill(skillBegging) / 50;	
	n < 1 && n = 1;
	
	uint16_t pregold = rand()%x + RandomNum(1,4);
	pregold > 25 && pregold = 25;
	
	uint16_t gold = 0;
	
	// pre-calculate the random amout of gold that is "targeted"
	pContainer pack = pc->getBackpack();
	
	if (pack)
	{
		gold = pack->countItems(0x0EED, 0, true);
		gold < pregold && pregold = gold;
		
		pack->removeItems(y, 0x0EED);
	}
	
	if (!gold)
	{
		pc->talk(s,"Sorry, I'm poor myself",1);
		return;
	}
	
	pc->talkAll("Ohh thou lookest so poor, Here is some gold I hope this will assist thee.",0); // zippy
	pcc->addGold(gold);
	sysmessage(s,"Some gold is placed in your pack.");

	// TODO need to update to new event system
//	AMXEXECSVTARGET( pcc->getSerial(),AMXT_SKITARGS,skBegging,AMX_AFTER);
}

