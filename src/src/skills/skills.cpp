/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "skills/skills.h"
#include "common_libs.h"
#include "settings.h"
#include "backend/strconstants.h"
#include "extras/jails.h"
#include <fstream>

//int goldsmithing;
//1=iron, 2=golden, 3=agapite, 4=shadow, 5=mythril, 6=bronze, 7=verite, 8=merkite, 9=copper, 10=silver
int ingottype=0;//will hold number of ingot type to be deleted

inline void SetSkillDelay(pChar pc)
{
	SetTimerSec(&pc->skilldelay, nSettings::Skills::getSkillDelay());
}

//! Default constructor for nSkills::sSkillInfo struct
nSkills::sSkillInfo::sSkillInfo()
{
	st = 0;
	dx = 0;
	in = 0;
	flags = 0x03;
	advanceIndex = 0;
	madeword = strNull;
}

/*!
\brief Loads the skills from skills.xml

This function loads the skills' infos from skills.xml datafile, and stores them
into nSkill::infos array.
*/
void nSkills::loadSkills()
{
	ConOut("Loading skills information...\t\t");
	
	std::ifstream xmlfile("config/skills.xml");
	if ( ! xmlfile )
	{
		ConOut("[ Failed ]\n");
		LogCritical("Unable to open skills.xml file.");
		return;
	}
	
	try {
		MXML::Document doc(xmlfile);
		
		MXML::Node *n = doc.main()->child();
		if ( ! n ) return;
		
		do {
			if ( n->name() != "skill" )
			{
				LogWarning("Unknown node %s in skills.xml, ignoring", n->name().c_str() );
				continue;
			}
			
			try {
				Skill sk = nStrConstants::skill(n->getAttribute("name"));
				if ( sk == skInvalid )
				{
					LogWarning("Unknown skill name in skills.xml, ignoring");
					continue;
				}
				
				if ( n->hasAttribute("str") )
					infos[sk].str = tVariant(n->getAttribute("str")).toUInt16();
				else infos[sk].str = 0;
				
				if ( n->hasAttribute("dex") )
					infos[sk].dex = tVariant(n->getAttribute("dex")).toUInt16();
				else infos[sk].dex = 0;
				
				if ( n->hasAttribute("int") )
					infos[sk].in_ = tVariant(n->getAttribute("int")).toUInt16();
				else infos[sk].in_ = 0;
				
				if ( n->hasAttribute("unhideOnUse") && n->getAttribute("unhideOnUse") != "true" )
					infos[sk].flags &= ~sSkillInfo::flagUnhideOnUse;
				else infos[sk].flags |= sSkillInfo::flagUnhideOnUse;
				
				if ( n->hasAttribute("unhideOnFail") && n->getAttribute("unhideOnFail") != "true" )
					infos[sk].flags &= ~sSkillInfo::flagUnhideOnFail;
				else infos[sk].flags |= sSkillInfo::flagUnhideOnFail;
				
				infos[sk].advances.clear();
				
				MXML::Node *p = n->child();
				
				if ( ! p )
					continue;
				
				do {
					try {
						sSkillAdvance adv;
						adv.base = p->getAttribute("base");
						adv.success = p->getAttribute("success");
						adv.failure = p->getAttribute("failure");
						infos[sk].advances.push_back(adv);
					} catch ( MXML::NotFoundError e ) {
						LogWarning("Incomplete node in skills.xml, ignoring");
						continue;
					}
				} while ( (p = p->next()) );
				
			} catch ( MXML::NotFoundError e ) {
				LogWarning("Incomplete node in skills.xml, ignoring");
				continue;
			}
		} while((n = n->next()));
		
		ConOut("[   OK   ]\n");
	} catch ( MXML::MalformedError e) {
		ConOut("[ Failed ]\n");
		LogCritical("skills.xml file not well formed.");
	}
	
	loadSkillVars();
}

/*!
\brief Loads the 'madewords' for the skills

This function loads into the \ref infos vector the data about the
'madeword' to use in the identification of the item's crafter.
\see Made words
*/
static inline void nSkills::loadSkillVars()
{
	// Note: lore, knowledge and combat skills can't made anything!	
	infos[skAnimalLore].madeword = strNull;
	infos[skItemID].madeword = strNull;
	infos[skArmsLore].madeword = strNull;
	infos[skParrying].madeword = strNull;
	infos[skBegging].madeword = strNull;
	infos[skDetectingHidden].madeword = strNull;
	infos[skEnticement].madeword = strNull;
	infos[skEvaluatingIntelligence].madeword = strNull;
	infos[skForensics].madeword = strNull;
	infos[skHerding].madeword = strNull;
	infos[skHiding].madeword = strNull;
	infos[skProvocation].madeword = strNull;
	infos[skLockPicking].madeword = strNull;
	infos[skMagicResistance].madeword = strNull;
	infos[skTactics].madeword = strNull;
	infos[skSnooping].madeword = strNull;
	infos[skArchery].madeword = strNull;
	infos[skSpiritSpeak].madeword = strNull;
	infos[skStealing].madeword = strNull;
	infos[skTasteID].madeword = strNull;
	infos[skTracking].madeword = strNull;
	infos[skVeterinary].madeword = strNull;
	infos[skSwordsmanship].madeword = strNull;
	infos[skMacefighting].madeword = strNull;
	infos[skFencing].madeword = strNull;
	infos[skWrestling].madeword = strNull;
	infos[skLumberjacking].madeword = strNull;
	infos[skStealth].madeword = strNull;
	infos[skRemoveTraps].madeword = strNull;
	
	infos[skAlchemy].madeword = strMixed;
	infos[skAnatomy].madeword = strMade;
	infos[skBlacksmithing].madeword = strForged;
	infos[skBowcraft].madeword = strBowcrafted;
	infos[skCamping].madeword = strMade;
	infos[skCarpentry].madeword = strMade;
	infos[skCartography].madeword = strWrote;
	infos[skCooking].madeword = strCooked;
	infos[skHealing].madeword = strMade;
	infos[skFishing].madeword = strFished;
	infos[skInscription].madeword = strWrote;
	infos[skMagery].madeword = strEvoked;
	infos[skMusicianship].madeword = strPlayed;
	infos[skPoisoning].madeword = strMixed;
	infos[skTailoring].madeword = strSewn;
	infos[skTaming].madeword = strTamed;   
	infos[skTinkering].madeword = strMade;
	infos[skMining].madeword = strSmelted;
	infos[skMeditation].madeword = strEvoked;
}

/*!
\author Luxor
\brief Implements the hiding skill
\param client client requesting hiding
*/
void Skills::Hide(pClient client)
{
	if ( ! client )
		return;

	pChar pc=cSerializable::findCharBySerial(currchar[s]);
	if ( ! pc ) return;

	NxwCharWrapper sc;
	pChar pj = NULL;
	sc.fillCharsNearXYZ( pc->getPosition(), 4 );
	for( sc.rewind(); !sc.isEmpty(); sc++ ) {
		pj = sc.getChar();
		if ( pj && pj->getSerial() != pc->getSerial() && !pj->IsHidden() && pc->losFrom(pj) ) {
			client->sysmessage("There is someone nearby who prevents you to hide.");
			return;
		}
	}

	pc->hideBySkill();
}

/*!
\author AntiChrist
\brief Stealths a char
\param client client requesting stealthing
*/
void Skills::Stealth(pClient client)
{
	pChar pc = NULL;
	if ( ! client || ! (pc = client->currChar()) ) //Luxor
		return;

	if ( pc->isMounting() && ! nSettings::Skills::canStealthOnHorse() ) {
		client->sysmessage("You can't stealth on horse!");
		return;
	}
	
	if (pc->hidden==0) {
		client->sysmessage("You must hide first.");
		return;
	}
	
	if (pc->skill[HIDING]<800)
	{
		client->sysmessage("You are not hidden well enough. Become better at hiding.");
		pc->stealth = INVALID;
		pc->unHide();
		return;
	}
	
	//XAN : more osi-like
	
	int def = pc->calcDef(0);
	int min = 0, max = 1000;
	if (def <= 5) { min = 0; max = 650; }
	if (def == 6) { min = 450; max = 700; }
	if (def == 7) { min = 500; max = 750; }
	if (def == 8) { min = 550; max = 800; }
	if (def == 9) { min = 600; max = 850; }
	if (def == 10) { min = 650; max = 900; }
	if (def == 11) { min = 650; max = 900; }
	if (def == 12) { min = 700; max = 950; }
	if (def == 13) { min = 700; max = 950; }
	if (def == 14) { min = 750; max = 1000; }
	if (def >= 15) { min = 800; max = 1100; }
	
	if (ServerScp::g_nStealthArLimit==0) {
		def = 0;
		min = 0;
		max = 1000;
	}
	// do not invert the two parts of the || operator otherwise
	// it stops raising skills when training with plate-armor ! :)
	if ((!pc->checkSkill(skStealth, min, max))||(def>20))
	{
		pc->setHidden(htUnhidden);
		pc->stealth = INVALID;
		pc->teleport( teleSendWornItems );
		return;
	}
	
	client->sysmessage("You can move %i steps unseen.", (nSettings::Skills::getMaxStealthSteps()*pc->skill[skStealth])/1000 );
	pc->stealth = 0; //AntiChrist -- init. steps already done
	pc->hideBySkill();
}

bool Skills::AdvanceSkill(pChar pc, int sk, char skillused)
{
	if ( sk < 0 || sk >= skTrueSkills ) //Luxor
		return 0;

	if ( ! pc ) return 0;
	
	int ges=0, d=0;
	uint8_t lockstate;
	int skillcap = SrvParms->skillcap;
	uint32_t incval;
	int atrophy_candidates[ALLSKILLS+1];

	pFunctionHandle evt = pc->getEvent(cChar::evtChrOnGetSkillCap);
	if ( evt )
	{
		tVariantVector params = tVariantVector(4);
		params[0] = pc->getSerial(); params[1] = pc->getClient();
		evt->setParams(params);
		tVariant ret = evt->execute();
		skillcap = ret.toSInt32();
	}
	
	lockstate = pc->lockSkill[sk];
	if (pc->isGM()) lockstate=0;
	// for gms no skill cap exists, also ALL skill will be interperted as up, no matter how they are set
	
	if (lockstate==2 || lockstate==1) return 0;// skill is locked -> forget it
	
	// also NOthing happens if you train a skill marked for atrophy !!!
	// skills only fall if others raise, ( osi quote ), so my interpretion
	// is that those marked as falling cant fall if you use them directly
	// exception: if you are gm its ignored!
	
	int c=0;

	for (register int b = 0; b < (ALLSKILLS+1); b++)
	{
		if (pc->lockSkill[b]==1 && pc->baseskill[b]!=0) // only count atrophy candidtes if they are above 0 !!!
		{
			atrophy_candidates[c] = b;
			c++;
		}
	}

	if (!pc->isGM())
	{
		for (register int a = 0; a < ALLSKILLS; a++)
			ges+=pc->baseskill[a];
		
		ges=ges/10;
	
		if (ges>skillcap && c==0) // skill capped and no skill is marked as fall down.
		{
			client->sysmessage("You have reached the skill-cap of %i and no skill can fall!", skillcap);
			return 0;
		}
	
	} else ges=0;

	//
	// Skill advance parameter
	//
	// 1. If the race system is active get them from there
	// 2. Else use the standard server tables
	//
	if( Race::isRaceSystemActive() )
	{
		if ( skillused )
		{
			Race* r = Race::getRace( (uint32_t) pc->race );
			if( r!=NULL )
				incval = r->getSkillAdvanceSuccess( (uint32_t) sk, (uint32_t) pc->baseskill[sk] );
			else
				incval=0;
			//SDbgOut("Race advance success for skill %d with base %d is %d\n", sk, pc->baseskill[sk], incval * 10 );
		}
		else
		{
			Race* r = Race::getRace( (uint32_t) pc->race );
			if( r!=NULL )
				incval = r->getSkillAdvanceFailure( (uint32_t) sk, (uint32_t) pc->baseskill[sk] );
			else
				incval =0;

			//SDbgOut("Race advance failure for skill %d with base %d is %d\n", sk, pc->baseskill[sk], incval * 10 );
		}
		incval *= 10;

	} else {
		int i = 0;
		int loopexit=0;
		while ( (wpadvance[1+i+infos[sk].advance_index].skill == sk &&
			wpadvance[1+i+infos[sk].advance_index].base  <= pc->baseskill[sk] ) &&
			(++loopexit < MAXLOOPS) ) ++i;
		
		if(skillused)
			incval=(wpadvance[i+infos[sk].advance_index].success);
		else
			incval=(wpadvance[i+infos[sk].advance_index].failure);
		
		incval *= 10;
	}

	bool retval = incval > (rand()%SrvParms->skilladvancemodifier);
	
	pFunctionHandle evt = pc->getEvent(cChar::evtChrOnAdvanceSkill);
	if ( evt )
	{
		tVariantVector params = tVariantVector(4);
		params[0] = pc->getSerial(); params[1] = sk;
		params[2] = skillused; params[3] = retval;
		evt->setParams(params);
		tVariant ret = evt->execute();
		retval = ret.toBoolean();
		if ( evt->isBypassed() )
			return;
	}
	
	if (retval)
	{
		pc->baseskill[sk]++;
		// no atrophy for gm's !!
		if (ges>skillcap) // atrophy only if cap is reached !!!
		// if we are above the skill cap -> we have to let the atrophy candidates fall
		// important: we have to let 2 skills fall, or we'll never go down to cap
		// (especially if we are far above the cap from previous verisons)
		{
			if (c==1)
			{
				d = (pc->baseskill[atrophy_candidates[0]]>=2)? 2 : 1;
				// avoid value below 0 (=65535 cause unsigned)
				if (d==1 && pc->baseskill[atrophy_candidates[0]]==0)
					d=0; // should never happen ...
				
				pc->baseskill[atrophy_candidates[0]]-=d;
				Skills::updateSkillLevel(pc, atrophy_candidates[0]);
				// we HAVE to correct the skill-value
				
				pc->updateSkill(atrophy_candidates[0]);
				// and send changed skill values packet so that client can re-draw correctly
				// this is very important cauz this is ONLY done for the calling skill value automatically .
			} else {
				for( int vol=0; vol<2; vol++ )
				{
					d = ( c != 0)? rand()%c : 0;
	
					if (pc->baseskill[atrophy_candidates[d]]>=1)
					{
						pc->baseskill[atrophy_candidates[d]]--;
						Skills::updateSkillLevel(pc, atrophy_candidates[d]);
						pc->updateSkill(atrophy_candidates[d]);
					}
				}
			}
		}
		if (ServerScp::g_nStatsAdvanceSystem == 0)
				Skills::AdvanceStats(pc, sk);
	}
	if (ServerScp::g_nStatsAdvanceSystem == 1)
		Skills::AdvanceStats(pc, sk);
	
	return retval;
}

/*!
\author Duke
\brief Little helper function for cSkills::AdvanceStats()

finds the appropriate line for the used skill in advance table
and uses the value of that skill (!) to increase the stat
and cuts it down to 100 if necessary

\param sk skill identifier
\param stat stat identifier
\param pc pointer to character to advance the stats to
\todo document missing paramteres
*/
static int AdvanceOneStat(uint32_t sk, int i, char stat, bool *update, int type, pChar pc)
{
	if ( sk < 0 || sk >= skTrueSkills ) //Luxor
		return 0;

	int loopexit=0, limit=1000;
	*update = false;
	int32_t tmp;

	int stat2update1, stat2update2;
	int stat2update;

	switch( stat )
	{
		case 'S': tmp= pc->st3;	break;
		case 'D': tmp= pc->dx3;	break;
		case 'I': tmp= pc->in3;	break;
	}

	if( Race::isRaceSystemActive() )
		stat2update1 = Race::getRace( (uint32_t) pc->race )->getSkillAdvanceSuccess( sk, tmp*10 );

	while ((wpadvance[i+1].skill==sk) &&     // if NEXT line is for same skill and is not higher than our stat then proceed to it !
     		(wpadvance[i+1].base<=(tmp*10)) && (++loopexit < MAXLOOPS) )
  	      	i++;

	stat2update2 = wpadvance[i].success;         // gather small increases

	//SDbgOut("AdvanceOneStat() skill %d base %d succes %d %d\n", sk, (*stat)*10, stat2update1, stat2update2);

	if( Race::isRaceSystemActive() )
		stat2update = stat2update1;
	else
		stat2update = stat2update2;

	switch( stat )
	{
		case 'S':	pc->st2+= stat2update;
					tmp= pc->st2;
					break;

		case 'D':	pc->dx2+= stat2update;
					tmp= pc->dx2;
					break;

		case 'I':	pc->in2+= stat2update;
					tmp= pc->in2;
					break;
	}

	if ( tmp >= 1000)           // until they reach 1000
	{
		switch( stat )
		{
			case 'S':	pc->st2-= 1000;	if (pc->st2 < 0) pc->st2 = 0; tmp= pc->st3; break;
			case 'D':	pc->dx2-= 1000; if (pc->dx2 < 0) pc->dx2 = 0; tmp= pc->dx3; break;
			case 'I':	pc->in2-= 1000;	if (pc->in2 < 0) pc->in2 = 0; tmp= pc->in3; break;
		}
 //       *stat2 -= 1000;                     // then change it


		pFunctionHandle evt = pc->getEvent(cChar::evtChrOnAdvanceStat);
		if ( evt )
		{
			tVariantVector params = tVariantVector(4);
			params[0] = pc->getSerial(); params[1] = type;
			params[2] = sk; params[3] = tmp;
			evt->setParams(params);
			evt->execute();
			if ( evt->isBypassed() )
				return false;
		}
		
		if( Race::isRaceSystemActive() )
		{
			switch( type )
			{
				case STATCAP_CAP:
					limit = Race::getRace( (uint32_t) pc->race )->getStatCap();
					SDbgOut("AdvanceOneStat() race %d %s statcap %d\n", pc->race, Race::getName( pc->race )->c_str(), limit );
					break;
				case STATCAP_STR:
					limit = Race::getRace( (uint32_t) pc->race )->getStrCap();
					SDbgOut("AdvanceOneStat() race %d %s strcap %d\n", pc->race, Race::getName( pc->race )->c_str(), limit );
					break;
				case STATCAP_DEX:
					limit = Race::getRace( (uint32_t) pc->race )->getDexCap();
					SDbgOut("AdvanceOneStat() race %d %s dexcap %d\n", pc->race, Race::getName( pc->race )->c_str(), limit );
					break;
				case STATCAP_INT:
					limit = Race::getRace( (uint32_t) pc->race )->getIntCap();
					SDbgOut("AdvanceOneStat() race %d %s intcap %d\n", pc->race, Race::getName( pc->race )->c_str(), limit );
					break;
			}
		} else {
			limit = 100;
		}

		pFunctionHandle evt = pc->getEvent(cChar::evtChrOnGetStatCap);
		if ( evt )
		{
			tVariantVector params = tVariantVector(3);
			params[0] = pc->getSerial(); params[1] = type;
			params[2] = limit;
			evt->setParams(params);
			tVariant ret = evt->execute();
			limit = ret.toSInt();
		}

		switch( stat )
		{
			case 'S':	pc->modifyStrength(1);
						pc->st3++;
						break;

			case 'D':	pc->dx++;
						pc->dx3++;
						break;

			case 'I':	pc->in++;
						pc->in3++;
						break;
		}
		*update=true;
	}

	if( !pc->isGM() )
	{
		switch( stat )
		{
		case 'S':
			if(pc->st3 > limit)
			{
				pc->st3=limit;
				pc->setStrength(limit);
			}
			break;

		case 'D':
			if(pc->dx3 > limit)
			{
				pc->dx3=limit;
				pc->dx= limit;
			}
			break;

		case 'I':
		if(pc->in3 > limit)
			{
				pc->in3=limit;
				pc->in= limit;
			}
			break;
		}
		*update= true;
	}
	return *update;
}

/*!
\brief Advance STR, DEX and INT after use of a skill
\param pc crafter character
\param sk skill identifier

checks if STR+DEX+INT are higher than statcap from server.cfg
gives all three stats the chance (from skills.scp & server.cfg) to rise
and reduces the two other stats if necessary
*/
void Skills::AdvanceStats(pChar pc, int sk)
{
	if ( sk < 0 || sk >= skTrueSkills ) //Luxor
		return;

	if ( ! pc ) return;

    	// Begin: Determine statcap
    	// 1. get statcap as defined in server.cfg
	// 2. get statcap from racesystem
	// 3. get statcap from char event
	//
	int statcap = SrvParms->statcap;

	if( Race::isRaceSystemActive() )
		statcap = Race::getRace( pc->race )->getStatCap();



	pFunctionHandle evt = pc->getEvent(cChar::evtChrOnGetStatCap);
	if ( evt )
	{
		tVariantVector params = tVariantVector(3);
		params[0] = pc->getSerial(); params[1] = STATCAP_CAP;
		params[2] = statcap;
		evt->setParams(params);
		tVariant ret = evt->execute();
		statcap = ret.toSInt();
	}

	// End: Determine statcap

	bool atCap = (pc->st3 + pc->dx3 + pc->in3) > statcap;
	
	int i = infos[sk].advance_index;
	int mod	= SrvParms->statsadvancemodifier;
//  int	*pi; // ptr to stat to be decreased
	bool update = false;

	if ( pc->statGainedToday <= nSettings::Skills::getStatDailyLimit() )
	{
		bool strCheck = ( Race::isRaceSystemActive() ? Race::getRace( pc->race )->getSkillAdvanceStrength( sk ) : infos[sk].st ) > (uint32_t)(rand() % mod);
    	bool dexCheck = ( Race::isRaceSystemActive() ? Race::getRace( pc->race )->getSkillAdvanceDexterity( sk ) : infos[sk].dx ) > (uint32_t)(rand() % mod);
    	bool intCheck = ( Race::isRaceSystemActive() ? Race::getRace( pc->race )->getSkillAdvanceIntelligence( sk ) : infos[sk].in ) > (uint32_t)(rand() % mod);

       	if ( strCheck )
       		if ( AdvanceOneStat( sk, i, 'S', &update, STATCAP_STR, pc ) && atCap && !pc->isGM() )
			{
				if( rand()%2 ) {
					pc->dx3--;
					pc->dx--;
				}
				else {
					pc->in3--;
					pc->in--;
				}
			}

		if ( dexCheck )
	       	if ( AdvanceOneStat(sk, i, 'D', &update, STATCAP_DEX, pc ) && atCap && !pc->isGM() )
			{
				if( rand()%2 ) {
					pc->st3--;
					pc->modifyStrength(-1);
				}
				else {
					pc->in3--;
					pc->in--;
				}
			}

	    if ( intCheck )
	       	if ( AdvanceOneStat(sk, i, 'I', &update, STATCAP_INT, pc ) && atCap && !pc->isGM() )
			{
				if( rand()%2 ) {
					pc->st3--;
					pc->modifyStrength(-1);
				}
				else {
					pc->dx3--;
					pc->dx--;
				}
			}



		if ( update )
		{

			pClient clientocket = pc->getSocket();

			++pc->statGainedToday;

			if ( socket != INVALID )
                		client->statusWindow(pc,true);  //!< \todo check second argument


			for ( i = 0;  i < ALLSKILLS; i++ )
				updateSkillLevel(pc,i );     // update client's skill window

			if ( atCap && !pc->isGM() )
				client->sysmessage("You have reached the stat-cap of %i!", statcap );

		}
	}
}

/*!
\brief Spirit speack time on a base of 30 seconds + skill[SPIRITSPEAK]/50 + INT
\param client client to the character
*/
void Skills::SpiritSpeak(pClient client)
{
	if ( ! client ) //Luxor
		return;
	pChar pc = client->currChar();
	//  Unsure if spirit speaking should they attempt again?
	//  Suggestion: If they attempt the skill and the timer is !0 do not have it raise the skill
	
	if(!pc->checkSkill(SPIRITSPEAK, 0, 1000))
	{
		client->sysmessage("You fail your attempt at contacting the netherworld.");
		return;
	}
	
	pc->impAction(0x11);   // I heard there is no action...but I decided to add one
	pc->playSFX(0x024A);   // only get the sound if you are successful
	client->sysmessage("You establish a connection to the netherworld.");
	SetTimerSec(&(pc->spiritspeaktimer),spiritspeak_data.spiritspeaktimer+pc->in);
}

/*!
\brief Skill is clicked on the skill list
\param client Client of the character that used skill
\param x skill identifier
*/
void Skills::SkillUse(pClient client, int x)
{
	if ( ! client || x >= skTrueSkills ) return;
	
	pChar pc = client->currChar();
	if ( ! pc ) return;

	if( (pc->skilldelay>getclock()) && (!pc->isGM()) )
	{
		client->sysmessage("You must wait a few moments before using another skill.");
		return;
	}

	if ( nJails::isJailed(pc) )
	{
		client->sysmessage("you are in jail and cant gain skills here!");
		return;
	}

	if ( pc->isDead() )
	{
		client->sysmessage("You cannot do that as a ghost.");
		return;
	}

	if (pc->IsHiddenBySpell())
		return; //Luxor: cannot use skills if under invisible spell

	//<Luxor> 7 dec 2001
	if (infos[x].unhide_onuse)
		pc->unHide();
	//</Luxor>


	if( pc->casting )
	{
		client->sysmessage("You can't do that while you are casting" );
		return;
	}

	pc->disturbMed(); // Meditation

	AMXEXECSV( pc->getSerial(),AMXT_SKILLS, x, AMX_BEFORE);

	bool setSkillDelay = true;

#if 0
	if( Race::isRaceSystemActive() && !(Race::getRace( pc->race )->getCanUseSkill( (uint32_t) x )) )
	{
		client->sysmessage("Your race cannot use that skill");
		setSkillDelay = false;
		AMXEXECSV( pc->getSerial(),AMXT_SKILLS, x, AMX_AFTER);
		return;
	}
#endif
	
	pTarget targ=NULL;
	switch(x)
	{
	case skArmsLore:
		targ=clientInfo[s]->newTarget( new cItemTarget() );
		targ->code_callback=target_armsLore;
		targ->send( ps );
		client->sysmessage( "What item do you wish to get information about?");
		break;

	case skAnatomy:
		break;

	case skItemID:
		targ=clientInfo[s]->newTarget( new cItemTarget() );
		targ->code_callback=Skills::target_itemId;
		targ->send( ps );
		client->sysmessage( "What do you wish to appraise and identify?");
		break;

	case skEvaluatingIntelligence:
		break;

	case skTaming:
		targ=clientInfo[s]->newTarget( new cCharTarget() );
		targ->code_callback=target_tame;
		targ->send( ps );
		client->sysmessage( "Tame which animal?");
		break;

	case skHiding:
		Skills::Hide(s);
		break;

	case skStealth:
		Skills::Stealth(s);
		break;

	case skDetectingHidden:
		targ=clientInfo[s]->newTarget( new cLocationTarget() );
		targ->code_callback=target_detectHidden;
		targ->send( ps );
		client->sysmessage( "Where do you wish to search for hidden characters?");
		break;

	case skPeacemaking:
		Skills::PeaceMaking(s);
		break;

	case skProvocation:
		targ=clientInfo[s]->newTarget( new cCharTarget() );
		targ->code_callback=target_provocation1;
		targ->send( ps );
		client->sysmessage( "Whom do you wish to incite?");
		break;

	case skEnticement:
		targ=clientInfo[s]->newTarget( new cCharTarget() );
		targ->code_callback=target_enticement1;
		targ->send( ps );
		client->sysmessage( "Whom do you wish to entice?");
		break;

	case skSpiritSpeak:
		Skills::SpiritSpeak(s);
		break;

	case skStealing:
		targ=clientInfo[s]->newTarget( new cObjectTarget() );
		targ->code_callback=target_stealing;
		targ->send( ps );
		client->sysmessage( "What do you wish to steal?");
		break;

	case skInscription:
		break;

	case skTracking:
		break;

	case skBegging:
		targ=clientInfo[s]->newTarget( new cCharTarget() );
		targ->code_callback = Begging::target;
		targ->send( ps );
		client->sysmessage( "Whom do you wish to annoy?");
		break;

	case skAnimalLore:
		targ=clientInfo[s]->newTarget( new cCharTarget() );
		targ->code_callback=Skills::target_animalLore;
		targ->send( ps );
		client->sysmessage( "What animal do you wish to get information about?");
		break;

	case skForensics:
		targ=clientInfo[s]->newTarget( new cItemTarget() );
		targ->code_callback=Skills::target_forensics;
		targ->send( ps );
		client->sysmessage( "What corpse do you want to examine?");
		break;

	case skPoisoning:
		targ=clientInfo[s]->newTarget( new cItemTarget() );
		targ->code_callback=Skills::target_poisoning;
		targ->send( ps );
		client->sysmessage( "What poison do you want to apply?");
		break;

	case skTasteID:
		break;

	case skMeditation:  //Morrolan - Meditation
		Skills::Meditation(s);
		break;

	case skRemoveTraps:
		targ=clientInfo[s]->newTarget( new cItemTarget() );
		targ->code_callback=target_removeTraps;
		targ->send( ps );
		client->sysmessage( "What do you want to untrap?");
		break;

	case skCartography:
		Skills::Cartography(s);
		break;

	default:
		client->sysmessage("That skill has not been implemented yet.");
		setSkillDelay = false;
		break;
	}
	//<Luxor>: Complete skills handling by AMX
	AmxFunction::g_prgOverride->CallFn( AmxFunction::g_prgOverride->getFnOrdinal(AMX_SKILLS_MAIN), s, x);

	if ( setSkillDelay )
		SetSkillDelay(pc);

	AMXEXECSV( pc->getSerial(),AMXT_SKILLS, x, AMX_AFTER);
}


/*!
\brief Calculate the skill of this character based on the characters baseskill and stats
\param pc pointer to the character
\param s skill identifier
*/
void Skills::updateSkillLevel(pChar pc, int s)
{
	if ( ! pc ) return;
	if ( s < 0 || s >= skTrueSkills ) //Luxor
		return;


    int temp = (((infos[s].st * pc->getBody()->getStrength()) / 100 +
        (infos[s].dx * pc->getBody()->getDexterity()) / 100 +
        (infos[s].in * pc->getBody()->getIntelligende()) / 100)
        *(1000-pc->baseskill[s]))/1000+pc->baseskill[s];


    pc->skill[s] = qmax(static_cast<short>(pc->baseskill[s]), static_cast<short>(temp));


}

void Skills::TDummy(pClient client)
{
	if ( ! client ) return;
	pPC pc = client->currChar();
	if ( ! pc ) return;

	int hit;

	pItem pWeapon = pc->getWeapon();

	if (pWeapon)
	{
		if (pc->getWeapon()->IsBowType())
		{
			client->sysmessage("Practice archery on archery buttes !");
			return;
		}
	}

	int skillused = pc->getCombatSkill();

	if (pc->isMounting())
		pc->combatOnHorse();
	else
		pc->combatOnFoot();

	hit=rand()%3;

	switch(hit)
	{
		case 0: pc->playSFX(0x013B);
			break;

		case 1: pc->playSFX(0x013C);
			break;

		case 2: pc->playSFX(0x013D);
			break;

		default:
			LogError("switch reached default");
			return;
	}

	pItem pj = cSerializable::findItemBySerial( LongFromCharPtr(buffer[s] +1) & 0x7FFFFFFF );

	if (pj)
	{
		if (pj->getId()==0x1070)
			pj->setId( 0x1071 );

		if (pj->getId()==0x1074)
			pj->setId( 0x1075 );

		tempfx::add(pc, pj, tempfx::TRAINDUMMY, 0, 0, 0); //Luxor
		pj->Refresh();
	}

	if(pc->skill[skillused] < 300)
	{
		pc->checkSkill( static_cast<Skill>(skillused), 0, 1000);

		if(pc->skill[skTactics] < 300)
			pc->checkSkill(skTactics, 0, 250);  //Dupois - Increase tactics but only by a fraction of the normal rate
	}
	else
		client->sysmessage("You feel you would gain no more from using that.");

}

void Skills::AButte(pClient client, pItem pButte)
{
	if ( ! client ) return;
	pPC pc = client->currChar();
	if ( ! pc ) return;

	int v1;
	if(pButte->getId()==0x100A)
	{ // East Facing Butte
		if ( pButte->getPosition().x > pc->getPosition().x || pButte->getPosition().y != pc->getPosition().y )
			v1= INVALID;
		else
			v1= pc->getPosition().x - pButte->getPosition().x;
	}
	else
	{ // South Facing Butte
		if ( pButte->getPosition().y > pc->getPosition().y || pButte->getPosition().x != pc->getPosition().x )
			v1= INVALID;
		else
			v1= pc->getPosition().y - pButte->getPosition().y;
	}

	int arrowsquant=0;
	if(v1==1)
	{
	
		if(pButte->more1>0)
		{
			pItem pi = item::CreateFromScript( "$item_arrow", pc->getBackpack(), pButte->more1/2 );
			if ( ! pi ) return;
			pi->Refresh();
		}

		if(pButte->more2>0)
		{
			pItem pi = item::CreateFromScript( "$item_crossbow_bolt", pc->getBackpack(), pButte->more2/2 );
			if ( ! pi ) return;
			pi->Refresh();
		}
	
		int i=0;
		if(pButte->more1>0) i++;
		if(pButte->more2>0) i+=2;

		switch(i)
		{
			case 0:
				client->sysmessage("This target is empty");
				break;
			case 1:
				client->sysmessage("You pull %d arrows from the target",pButte->more1/2);
				break;
			case 2:
				client->sysmessage("You pull %d bolts from the target",pButte->more2/2);
				break;
			case 3:
				client->sysmessage("You pull %d arrows and %d bolts from the target",pButte->more1,pButte->more2/2);
				break;
			default:
				LogError("switch reached default");
				return;
		}
		pButte->more1=0;
		pButte->more2=0;
	}

	if ( v1 < 5 || v1 > 8 )
	{
		client->sysmessage( "You cant use that from here.");
		return;
	}
	
	if (!pc->getWeapon()->IsBowType())
	{
		client->sysmessage( "You need to equip a bow to use this.");
		return;
	}
	if ((pButte->more1+pButte->more2)>99)
	{
		client->sysmessage( "You should empty the butte first!");
		return;
	}
	if (pc->getWeapon()->IsBow())
		arrowsquant=pc->getAmount(0x0F3F); // Sabrewulf
	else
		arrowsquant=pc->getAmount(0x1BFB);

	if (arrowsquant==0)
	{
		client->sysmessage( "You have nothing to fire!");
		return;
	}

	if (pc->getWeapon()->IsBow())
	{
		pc->delItems( 0x0F3F, 1);
		pButte->more1++;
		//add moving effect here to item, not character
	} else {
		pc->delItems( 0x1BFB, 1);
		pButte->more2++;
		//add moving effect here to item, not character
	}

	if (pc->isMounting())
		pc->combatOnHorse();
	else
		pc->combatOnFoot();

	if( pc->skill[skArchery] < 350 )
		pc->checkSkill( skArchery, 0, 1000 );
	else
		client->sysmessage( "You learn nothing from practicing here");

	switch( ( pc->skill[skArchery]+ ( (rand()%200) -100) ) /100 )
	{
	case -1:
	case 0:
	case 1:
		client->sysmessage( "You miss the target");
		pc->playSFX(0x0238);
		break;
	case 2:
	case 3:
		client->sysmessage( "You hit the outer ring!");
		pc->playSFX(0x0234);
		break;
	case 4:
	case 5:
	case 6:
		client->sysmessage( "You hit the middle ring!");
		pc->playSFX(0x0234);
		break;
	case 7:
	case 8:
	case 9:
		client->sysmessage( "You hit the inner ring!");
		pc->playSFX(0x0234);
		break;
	case 10:
	case 11:
		client->sysmessage( "You hit the bullseye!!");
		pc->playSFX(0x0234);
		break;
	}
}

/*!
\author Luxor
\brief Implements Meditation skill
\param client Client who requested the meditation
*/
void Skills::Meditation (pClient client)
{
	if ( ! client ) return;
	pPC pc = client->currChar();
	if ( ! pc ) return;

	pc->setMeditating(false);

	if ( pc->war ) {
		client->sysmessage( "Your mind is too busy with the war thoughts.");
		return;
	}

	if ( Skills::GetAntiMagicalArmorDefence(pc) > 15 ) {
		client->sysmessage( "Regenerative forces cannot penetrate your armor.");
		return;
	}

	pWeapon pi = pc->getWeapon();
	if ( (pi && !pi->IsStave()) || pc->getShield() ) {
		client->sysmessage( "You cannot meditate with a weapon or shield equipped!");
		return;
	}

	if ( pc->mn == pc->in ) {
		client->sysmessage( "You are at peace.");
		return;
	}


	//
	// Meditation check
	//
	if ( !pc->checkSkill(skMeditation, 0, 1000) ) {
		client->sysmessage( "You cannot focus your concentration.");
		return;
	}

	client->sysmessage("You enter a meditative trance.");
	pc->setMeditating(true);
	pc->playSFX(0x00F9);
}

/*!
\author AntiChrist
\param client client of the persecuter

If you are a ghost and attack a player, you can PERSECUTE him
and his mana decreases each time you try to persecute him
decrease=3+(your int/10)
*/
void Skills::Persecute (pClient client)
{
	if ( ! client ) return;
	pPC pc = client->currChar();

	pChar pc_targ = pc->getTarget();
	if ( ! pc_targ ) return;

	if (pc_targ->isGM()) return;
	
	int decrease=(pc->in/10)+3;
	
	if( pc->skilldelay > getclock() && !pc->isGM() )
	{
		client->sysmessage("You are unable to persecute him now...rest a little...");
		return;
	}
	
	if ( (rand()%20 + pc->in) <= 45 )
	{
		client->sysmessage("Your mind is not strong enough to disturb the enemy.");
		return;
	}
	
	if( pc_targ->mn <= decrease )
		pc_targ->mn = 0;
	else
		pc_targ->mn-=decrease;//decrease mana
	pc_targ->updateMana();//update
	client->sysmessage("Your spiritual forces disturb the enemy!");
	
	if ( dynamic_cast<pPC>(pc_targ) )
		(dynamic_cast<pPC>(pc_targ))->getClient()->sysmessage("A damned soul is disturbing your mind!");
	
	SetSkillDelay(pc);

	pc_targ->emoteall("%s is persecuted by a ghost!!", true, pc_targ->getCurrentName().c_str());
}

int Skills::GetAntiMagicalArmorDefence(pChar pc)
{
	if(!pc) return 0;

	int ar = 0;
	if (pc->HasHumanBody())
	{
		NxwItemWrapper si;
		si.fillItemWeared( pc, false, true, true );
		for( si.rewind(); !si.isEmpty(); si++ )
		{
			pItem pi=si.getItem();
			if( pi && pi->layer>1 && pi->layer < 25)
			{
				if (!(strstr(pi->getCurrentName().c_str(), "leather") || strstr(pi->getCurrentName().c_str(), "magic") ||
				strstr(pi->getCurrentName().c_str(), "boot")|| strstr(pi->getCurrentName().c_str(), "mask")))
					ar += pi->def;
			}
		}
	}
	return ar;
}
/*!
\brief Builds the cartography menu
\param client Client of the crafter

Function is called when clicked on the \em Cartography button

\todo Complete write
*/
void Skills::Cartography(pClient client)
{
	if ( ! client ) //Luxor
		return;

	pChar pc = client->currChar();
	if ( ! pc ) return;

	if ( ! pc->getBody()->getBackpack()->getEmptyMap(true) )
	{
		client->sysmessage("You don't have an empty map to draw on");
		return;
	}
	
	//itemmake[s].has = 1;
	//Skills::MakeMenu(s, 1200, skCartography);
}
