/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "targeting.h"
#include "magic.h"
#include "tmpeff.h"
#include "particles.h"
#include "sndpkg.h"
#include "npcai.h"
#include "data.h"
#include "map.h"
#include "inlines.h"
#include "extras/jails.h"

namespace magic {

sSpell spellsData[MAX_SPELLS];
std::map< std::string, SpellId > speechMap;

/*!
\brief Gets the direction for a field magic
\param pc Caster character
\param p Point where the field should be summoned
*/
static bool fielddir(pChar pc, sPoint p)
{
	if ( !pc ) return false;

	int dir = pc->getDirFromXY(loc);
	switch (dir)
	{
	case 0:
	case 4:
		return false;
	case 2:
	case 6:
		return true;
	case 1:
	case 3:
	case 5:
	case 7:
	case INVALID:
		switch(pc->dir) //crashfix, LB
		{
		case 0:
		case 4:
			return false;

		case 2:
		case 6:
			return true;

		case 1:
		case 3:
		case 5:
		case 7:
			return true;

		default:
			LogError("Switch fallout. misc.cpp, fielddir()\n"); //Morrolan
			return false;
		}
	default:
		LogError("Switch fallout. misc.cpp, fielddir()\n"); //Morrolan
		return false;
	}
}

/*!
\author Luxor
*/
uint32_t getCastingTime( SpellId spell )
{
	return ( ( (spellsData[spell].delay/10) * SECS ) + getclock() );
}

/*!
\author Luxor
*/
bool checkMagicalSpeech( pChar pc, char* speech )
{
	if(!pc) return false;

	pPC pc_tmp;
	pClient client = (pc_tmp = dynamic_cast<pPC>(pc))? pc_tmp->getClient() : NULL;
	if(!client) return false;

	strupr( speech );
	std::string sSpeech( speech );
	std::map< std::string, SpellId >::iterator it( speechMap.find( sSpeech ) );
	if ( it == speechMap.end() )
		return false;
	SpellId spell = it->second;
	if ( !pc->knowsSpell( spell ) ) {
		client->sysmessage("You don't know that spell yet.");
		return true;
	}
	beginCasting( spell, client, CASTINGTYPE_SPELL );
	return true;
}

/*!
\brief Load spells data from file
\author Xanatar
*/
void loadSpellsFromScript()
{
	cScpIterator* iter = NULL;
	char script1[1024], script2[1024];

	for (register int curspell = 0; curspell < MAX_SPELLS; curspell++) {
		// in XSS script, numeration starts from 1 :[
		spellsData[curspell].attackSpell = false;
		spellsData[curspell].areasize = INVALID;
		sprintf(script1, "SECTION SPELL %s", g_szSpellName[curspell]);
		safedelete(iter);
		iter = Scripts::Spells->getNewIterator(script1);
		if (iter==NULL) continue;

		do
		{
			iter->parseLine(script1, script2);
			if (!strcmp("ENABLED", script1)) 		spellsData[curspell].enabled = true;
			else if (!strcmp("DISABLED", script1))	spellsData[curspell].enabled = false;
			else if (!strcmp("CIRCLE", script1)) 	spellsData[curspell].circle = str2num(script2);
			else if (!strcmp("MANA", script1))		spellsData[curspell].mana = str2num(script2);
			else if (!strcmp("HISKILL", script1))	spellsData[curspell].hiskill = str2num(script2);
			else if (!strcmp("LOSKILL", script1))	spellsData[curspell].loskill = str2num(script2);
			else if (!strcmp("SCLO", script1))		spellsData[curspell].sclo = str2num(script2);
			else if (!strcmp("SCHI", script1))		spellsData[curspell].schi = str2num(script2);
			else if (!strcmp("LODAMAGE", script1))	spellsData[curspell].lodamage = str2num(script2);
			else if (!strcmp("HIDAMAGE", script1))	spellsData[curspell].hidamage = str2num(script2);
			else if (!strcmp("DAMAGETYPE", script1)) spellsData[curspell].damagetype = static_cast<DamageType>(str2num(script2));
			else if (!strcmp("ATTACKSPELL", script1))spellsData[curspell].attackSpell = true;
			else if (!strcmp("ALWAYSFLAG", script1)) spellsData[curspell].alwaysflag = str2num(script2);
			else if (!strcmp("AREASIZE", script1))	spellsData[curspell].areasize = str2num(script2);
			else if (!strcmp("MANTRA", script1)) 	{ spellsData[curspell].mantra += script2;
				//Luxor: speech cast
				strupr( script2 );
				speechMap.insert( pair< std::string, SpellId >( std::string( script2 ), static_cast<SpellId>(curspell) ) );
			}
			else if (!strcmp("ACTION", script1)) 	spellsData[curspell].action = hex2num(script2);
			else if (!strcmp("DELAY", script1))		spellsData[curspell].delay = str2num(script2);
			else if (!strcmp("ASH", script1))		spellsData[curspell].reagents.ash = str2num(script2);
			else if (!strcmp("DRAKE", script1))		spellsData[curspell].reagents.drake = str2num(script2);
			else if (!strcmp("GARLIC", script1)) 	spellsData[curspell].reagents.garlic = str2num(script2);
			else if (!strcmp("GINSING", script1))	spellsData[curspell].reagents.ginseng = str2num(script2);
			else if (!strcmp("MOSS", script1))		spellsData[curspell].reagents.moss = str2num(script2);
			else if (!strcmp("PEARL", script1))		spellsData[curspell].reagents.pearl = str2num(script2);
			else if (!strcmp("SHADE", script1))		spellsData[curspell].reagents.shade = str2num(script2);
			else if (!strcmp("SILK", script1))		spellsData[curspell].reagents.silk = str2num(script2);
			else if (!strcmp("TARG", script1))		spellsData[curspell].strToSay += script2;
			else if (!strcmp("REFLECTABLE", script1))	spellsData[curspell].reflect = true;
			else if (!strcmp("UNREFLECTABLE", script1))	  spellsData[curspell].reflect = false;
			else if (!strcmp("RUNIC", script1))		spellsData[curspell].runic = str2num(script2)!=0;
		}
		while (script1[0]!='}');
	}
	safedelete(iter);
}


/*!
\author Luxor
\brief Checks for gate collisions
*/
bool checkGateCollision( pChar pc )
{
	if(!pc) return false;

        if ( pc->npc )
		return false;

	pItem pgate = NULL;

        sLocation charpos = pc->getPosition();

	NxwItemWrapper si;
	// WIntermute: Only check items beneath the feet or neighbouring items may be triggered first
	si.fillItemsNearXYZ( charpos, 0, false );
	for ( si.rewind(); !si.isEmpty(); si++ ) {
		pgate = si.getItem();
		if ( !pgate )
			return false;

		if ( pgate->type != 51 )
			pgate = NULL;
	}

        if ( !pgate )
		return false;

	sLocation gatepos = pgate->getPosition();

	if ( sPoint(charpos) != sPoint(gatepos) || uint32_t(charpos.z - gatepos.z) > 2 )
		return false;

        pChar pnpc = NULL;
        NxwCharWrapper sc;
        sc.fillOwnedNpcs( pc, false, true );
        for ( sc.rewind(); !sc.isEmpty(); sc++ ) {
		if ( !(pnpc=sc.getChar()) )
			continue;

		pnpc->MoveTo( pgate->morex, pgate->morey, pgate->morez );
	}

	pc->MoveTo( pgate->morex, pgate->morey, pgate->morez );
	pc->teleport();
	pc->playSFX( 0x01FE );
	staticFX(pc, 0x372A, 0x09, 0x06 );

	return true;
}

/*!
\brief Check if a spell can be casted (considering town limits)
\author Xanatar
\param spellnum the spell number
\param pa attacker (can be NULL)
\param pd defender
\param spellflags spell flags
\param param spell param
\param areaspell Is an spell area?
*/
static inline bool checkTownLimits(SpellId spellnum, pChar pa, pChar pd, uint16_t spellflags, int param, bool areaspell = false)
{
	if(!pd) return false;

	if ((spellsData[spellnum].attackSpell)&&(SrvParms->guardsactive)&&(region[pd->region].priv&rgnFlagGuarded))
	{
		if (spellflags&SPELLFLAG_IGNORETOWNLIMITS) return false;
		if (areaspell) return false; // do *NOT* change order of these lines!! :]
		if ((pd->npc)&&(pd->npcaitype==NPCAI_EVIL)) return false;
		if (pa==pd) return false;
		if (pa) {
			if (Guilds->Compare(pa,pd)!=0) return false;
		}
		if (pd->IsCriminal()) return false;
		if (pd->IsGrey()) return false;
		return true;
	}
	return false;
}


/*!
\brief Checks if char has enough mana
\author Xanathar
\param pc the player who want to cast
\param num spell id
\retval true The player has enough mana
\retval false The player hasn't enough mana (and was sysmessaged)
*/
static inline bool checkMana(pChar pc, SpellId num)
{
	if(!pc) return false;

	pPC pc_tmp;
	pClient client = (pc_tmp = dynamic_cast<pPC>(pc))? pc_tmp->getClient() : NULL;

	if ( pc->dontUseMana() ) return true;

	if (pc->mn >= spellsData[num].mana) return true;

	client->sysmessage("You have insufficient mana to cast that spell.");
	return false;
}


/*!
\brief Subtracts mana from char
\author Xanatar
\param pc caster
*/
static inline void subtractMana(pChar pc, SpellId spellnumber)
{
	if ( ! pc ) return;

	if ( pc->dontUseMana() ) return;

	if (pc->mn >= spellsData[spellnumber].mana) pc->mn -= spellsData[spellnumber].mana;
	else pc->mn = 0;

	pc->updateMana();//AntiChrist - bugfix
}


/*!
\brief Checks (recursively) magic reflection and eventually changes attacker/defender roles
\author Xanatar
\param[in,out] pa attacker
\param[in,out] pd defender
\retval true The roles of \c pa and \c pd are inverted
\retval false The roles aren't inverted
*/
static bool checkReflection(pChar &pa, pChar &pd)
{
	if(!pa) return false;
	if(!pd) return false;

	if ( pd->hasReflection() ) {
		pd->setReflection(false);
		staticFX(pd, 0x373A, 0, 15);
		qswap(pa, pd);
		return !checkReflection(pa, pd);
	}
	return false;
}


/*!
\brief Checks if a spell is a field
\author Xanathar
\param spell Spell to check
\retval true The spell is a field type
\retval false The spell isn't a field type
*/
static inline bool isFieldSpell(SpellId spell)
{
	switch (spell) {
		case spellFireField:
		case spellPoisonField:
		case spellParalyzeFIELD:
		case SPELL_ENERGYFIELD:
		case spellWallStone:
			return true;
		default:
			return false;
	}
}


/*!
\brief Checks if a spell is a box cast
\author Xanathar
\param spell Spell to check
\retval true The spell is a box spell
\retval false The spell is an area spell
*/
static inline bool isBoxSpell(SpellId spell)
{
	return !(spellsData[spell].areasize == 0);
}

/*!
\brief Check if defender can resist a spell
\author Xanatar
\param pa attacker
\param pd defender
\param spellnumber spell to check
*/
static bool checkResist(pChar pa, pChar pd, SpellId spellnumber)
{	// This function uses informations found at http://uo.stratics.com !

	if(!pd) return false;

	pPC tmp;
	pClient client = (tmp = dynamic_cast<pPC>(pd))? tmp->getClient() : NULL;

	int circle = (spellnumber) / 8;
	// just to give skill a chance to raise :)
	pd->checkSkill( skMagicResistance, 80*circle, 1000, !isFieldSpell(spellnumber));

	int resist = pd->skill[skMagicResistance] / 10;
	int resistchance = resist / 5;

	if (pa) {
		int magery = pa->skill[skMagery]/10;
		int secondresist = (resist) - ( (magery/2) + (circle*5));
		resistchance = qmax(resistchance, secondresist);
	}

	if (chance(resistchance)) {
		client->sysmessage("You feel yourself resisting magical energy!");
		return true;
	} else return false;
}



/*!
\author Luxor
\brief Plays the spell effect
*/
static void spellFX(SpellId spellnum, pChar pcaster = NULL, pChar pctarget = NULL, pItem pitarget = NULL )
{
	pChar pcfrom = pcaster;
	pChar pcto = pctarget;
	if ( !pcfrom && !pcto )
		return;
	if ( !pcfrom && pcto )
		pcfrom = pcto;
	if ( pcfrom && !pcto )
		pcto = pcfrom;

	ParticleFx spfx, mpfx;

	mpfx.initWithSpellMoveEffect(spellnum);
	spfx.initWithSpellStatEffect(spellnum);

	switch( spellnum )
	{
		case spellClumsy:
			pcto->playSFX( 0x1DF );
			staticFX(pcto, 0x374A, 0, 10, &spfx );
			break;
		case spellCreateFood:
			pcfrom->playSFX( 0x1E2 );
			break;
		case spellFeebleMind:
			pcto->playSFX( 0x1E4 );
			staticFX(pcto, 0x374A, 0, 10, &spfx );
			break;
		case spellHeal:
			staticFX(pcto, 0x376A, 0, 10, &spfx );
			pcto->playSFX( 0x1F2 );
			break;
		case spellMagicArrow:
			movingFX(pcfrom, pcto, 0x36E4, 5, 0, true, &mpfx );
			pcfrom->playSFX( 0x1E5 );
			break;
		case spellNightSight:
			pcfrom->playSFX( 0x1E3 );
			break;
		case spellReactiveArmour:
			pcfrom->playSFX( 0x211 );
			staticFX(pcfrom, 0x373A, 0, 10, &spfx );
			break;
		case spellWeaken:
			pcto->playSFX( 0x1E6 );
			staticFX(pcto, 0x374A, 0, 10, &spfx );
			break;
		case spellAgility:
			pcto->playSFX( 0x1E7 );
			staticFX(pcto, 0x375A, 0, 10, &spfx );
			break;
		case spellCunning:
			pcto->playSFX( 0x1EB );
			staticFX(pcto, 0x375A, 0, 10, &spfx );
			break;
		case spellCure:
			pcto->playSFX( 0x1E0 );
			staticFX(pcto, 0x376A, 0, 10, &spfx );
			break;
		case spellHarm:
			pcto->playSFX( 0x1F1 );
			staticFX(pcto, 0x374A, 0, 10, &spfx );
			break;
		case spellProtection:
			pcto->playSFX( 0x1ED );
			staticFX(pcto, 0x373A, 0, 10, &spfx );
			break;
		case spellStrength:
			pcto->playSFX( 0x1EE );
			staticFX(pcto, 0x375A, 0, 10, &spfx );
			break;
		case spellBless:
			pcto->playSFX( 0x1EA );
			staticFX(pcto, 0x375A, 0, 10, &spfx );
			break;
		case spellFireball:
			if ( pcfrom->skill[skMagery] < 500 )	// First level fireball
				pcfrom->playSFX( 0x15E );
			else if ( pcfrom->skill[skMagery] < 800 )	// Second level fireball
				pcfrom->playSFX( 0x15F );
			else					// Third level fireball
				pcfrom->playSFX( 0x1F3 );
			movingFX(pcfrom, pcto, 0x36D5, 5, 0, true, &mpfx );
			break;
		case spellPoison:
			pcto->playSFX( 0xF5 );
			staticFX(pcto, 0x374A, 0, 10, &spfx );
			break;
		case spellTelekinesys:
			pcfrom->playSFX( 0x1F5 );
			break;
		case spellTeleport:
			pcfrom->playSFX( 0x1FE );
			staticFX(pcfrom, 0x3727, 0, 10, &spfx );
			break;
		case spellWallStone:
			pcfrom->playSFX( 0x1F6 );
			break;
		case spellArchCure:
			pcto->playSFX( 0x1E8 );
			staticFX(pcto, 0x376A, 0, 10, &spfx );
			break;
		case spellArchProtection:
			pcto->playSFX( 0x1F7 );
			staticFX(pcto, 0x373A, 0, 10, &spfx );
			break;
		case spellCurse:
			pcto->playSFX( 0x1E1 );
			staticFX(pcto, 0x374A, 0, 10, &spfx );
			break;
		case spellFireField:
		case spellPoisonField:
			pcfrom->playSFX( 0x20C );
			break;
		case spellGreatHeal:
			pcto->playSFX( 0x202 );
			staticFX(pcto, 0x376A, 0, 10, &spfx );
			break;
		case spellLightning:
			if ( pcfrom->skill[skMagery] < 500 )	// First level lightning
				pcto->playSFX( 0x28 );
			else if ( pcfrom->skill[skMagery] < 800 )	// Second level lightning
				pcto->playSFX( 0x29 );
			else					// Third level lightning
				pcto->playSFX( 0x206 );
			boltFX(pcto, false );
			break;
		case spellManaDrain:
			pcto->playSFX( 0x1F8 );
			break;
		case spellRecall:
			pcfrom->playSFX( 0x1FC );
			break;
		case spellBladeSpirit:
			pcfrom->playSFX( 0x212 );
			break;
		case spellDispelField:
			pcfrom->playSFX( 0x201 );
			break;
		case spellIncognito:
		case SPELL_POLYMORPH:
			pcfrom->playSFX( 0x20F );
			break;
		case spellReflection:
			pcfrom->playSFX( 0x1E9 );
			staticFX(pcfrom, 0x375A, 0, 10, &spfx );
			break;
		case spellMindBlast:
			pcto->playSFX( 0x213 );
			staticFX(pcto, 0x374A, 0, 10, &spfx );
			break;
		case spellParalyze:
			pcto->playSFX( 0x204 );
			staticFX(pcto, 0x374A, 0, 10, &spfx );
			break;
		case spellSummon:
			pcfrom->playSFX( 0x215 );
			break;
		case SPELL_DISPEL:
			pcto->playSFX( 0x1E1 );
			break;
		case SPELL_ENERGYBOLT:
			pcfrom->playSFX( 0x20A );
			movingFX(pcfrom, pcto, 0x379F, 5, 0, true, &mpfx );
			break;
		case SPELL_EXPLOSION:
			if ( pcfrom->skill[skMagery] < 800 )	// First level explosion
				pcto->playSFX( 0x11D );
			else					// Second level explosion
				pcto->playSFX( 0x207 );
			staticFX(pcto, 0x36B0, 0, 10, &spfx );
			break;
		case SPELL_INVISIBILITY:
			pcfrom->playSFX( 0x203 );
			break;
		case SPELL_MARK:
			pcfrom->playSFX( 0x1FA );
			break;
		case SPELL_MASSCURSE:
			pcto->playSFX( 0x1FB );
			staticFX(pcto, 0x374A, 0, 10, &spfx );
			break;
		case spellParalyzeFIELD:
			pcfrom->playSFX( 0x20B );
			break;
		case SPELL_REVEAL:
			pcfrom->playSFX( 0x1FD );
			break;
		case SPELL_CHAINLIGHTNING:
			pcto->playSFX( 0x206 );
			boltFX(pcto, false );
			break;
		case SPELL_ENERGYFIELD:
			pcfrom->playSFX( 0x210 );
			break;
		case SPELL_FLAMESTRIKE:
			staticFX(pcto, 0x3709, 0, 10, &spfx );
			pcto->playSFX( 0x208 );
			break;
		case SPELL_GATE:
			pcfrom->playSFX( 0x20D );
			break;
		case SPELL_MANAVAMPIRE:
			pcto->playSFX( 0x1F9 );
			movingFX(pcto, pcfrom, 0x36F4, 5, 0, true, &mpfx );
			break;
		case SPELL_MASSDISPEL:
			pcto->playSFX( 0x209 );
			break;
		case SPELL_METEORSWARM:
			pcto->playSFX( 0x11B );
			staticFX(pcto, 0x36B0, 0, 10, &spfx );
			break;
		case SPELL_EARTHQUAKE:
			pcto->playSFX( 0x20D );
			if ( pcto->HasHumanBody() && !pcto->isMounting() )
				pcto->playAction( (rand()%2 == 1) ? 0x15 : 0x16 );
			break;
		case SPELL_ENERGYVORTEX:
			pcfrom->playSFX( 0x212 );
			break;
		case SPELL_RESURRECTION:
			pcfrom->playSFX( 0x214 );
			break;
		case spellSummon_AIR:
		case spellSummon_EARTH:
		case spellSummon_FIRE:
		case spellSummon_WATER:
			pcfrom->playSFX( 0x217 );
			break;
		case spellSummon_DEAMON:
			pcfrom->playSFX( 0x216 );
			break;
		default:
			break;
	}
}


/*!
\brief Inflicts magic damage from a spell
\author Xanatar
\param pa attacker
\param pd defender
\param spellnum Spell casted
\param spellflags spell flags
\param param optional parameter for some spellflags
*/
static void damage(pChar pa, pChar pd, SpellId spellnum, uint16_t spellflags = 0, int param = 0)
{
	if(!pd) return false;

	pChar p_realattacker = pa;
	pChar p_realdefender = pd;

	// check resistances :)
	if ((pa)&&(!(spellflags&SPELLFLAG_DONTREFLECT)))
		if (spellsData[spellnum].reflect)
			checkReflection(pa, pd);

	// early return for invulz
	if (pd->IsInvul()) return;

	// early return if in town and target should not be damaged
	if (checkTownLimits(spellnum, pa, pd, spellflags, param)) return;

	// check resistance for damage resistance
	bool bResists = checkResist(pa, pd, spellnum);
	if (spellflags&SPELLFLAG_IGNORERESISTANCE) bResists = false;

	// calculate basic spell damage
	double damage = static_cast<double>(RandomNum(spellsData[spellnum].lodamage, spellsData[spellnum].hidamage));
	if (spellflags&SPELLFLAG_PARAMISDAMAGE) damage = static_cast<double>(param);

	// calculates evint/resist modifier
	double resist = static_cast<double>(pd->skill[skMagicResistance]) / 10.0;
	double evint = resist; //no bonus/malus if no attacker

	if (pa) {
		evint = static_cast<double>(pd->skill[skEvaluatingIntelligence]) / 10.0;
	}

	if (bResists) damage/=2.0;

	double mod;

	if (resist > evint) {
		mod = 1.0 + (evint - resist) / 200.0;
	} else if (resist == evint || pd->nxwflags[0] & cChar::flagSpellProtection) { //Luxor
		mod = 1.0 + (evint - resist) / 300.0;
	} else {
		mod = 1.0 + (evint - resist) / 500.0;
	}

	if ( mod < 0.1 ) // Luxor
		mod = 0.1;
	damage *= mod;
	int amount = static_cast<int>(damage);

	// now we have the damage to be done :]

	if (pd->npc) amount *= 4;		   // double damage against non-players

	if ( p_realattacker ) p_realattacker->attackStuff(p_realdefender);

	StatType stattodamage = STAT_HP;
	if ((spellnum==spellManaDrain)||(spellnum==SPELL_MANAVAMPIRE)) stattodamage = STAT_MANA;
	pd->damage(amount, spellsData[spellnum].damagetype, stattodamage);
}


/*!
\brief Check presence of reagents
*/
bool checkReagents(pChar pc, sReagents reagents)
{
	if(!pc) return false;

	pPC pc_tmp;
	pClient client = (pc_tmp = dynamic_cast<pPC>(pc))? pc_tmp->getClient() : NULL;

	sReagents fail;

//	if( pc->IsGM() ) return true;

	if ( pc->dontUseReagents() ) return true;
	if (dynamic_cast<pNPC>(pc)) return true;

	fail.ash=fail.drake=fail.garlic=fail.moss=fail.pearl=fail.shade=fail.silk = 0;

	if (reagents.ash!=0 && pc->CountItems(0x0F8C)<reagents.ash) fail.ash=1;
	if (reagents.drake!=0 && pc->CountItems(0x0F86)<reagents.drake) fail.drake=1;
	if (reagents.garlic!=0 && pc->CountItems(0x0F84)<reagents.garlic) fail.garlic=1;
	if (reagents.ginseng!=0 && pc->CountItems(0x0F85)<reagents.ginseng) fail.ginseng=1;
	if (reagents.moss!=0 && pc->CountItems(0x0F7B)<reagents.moss) fail.moss=1;
	if (reagents.pearl!=0 && pc->CountItems(0x0F7A)<reagents.pearl) fail.pearl=1;
	if (reagents.shade!=0 && pc->CountItems(0x0F88)<reagents.shade) fail.shade=1;
	if (reagents.silk!=0 && pc->CountItems(0x0F8D)<reagents.silk) fail.silk=1;

	std::string str("You do not have enough reagents to cast that spell.[ ");

	if (fail.ash)	  str += "Sa ";
	if (fail.drake)   str += "Mr ";
	if (fail.garlic)  str += "Ga ";
	if (fail.ginseng) str += "Gi ";
	if (fail.moss)	  str += "Bm ";
	if (fail.pearl)   str += "Bp ";
	if (fail.shade)   str += "Ns ";
	if (fail.silk)	  str += "Ss ";
	str += "]";

	int failure = fail.ash+fail.drake+fail.garlic+fail.moss+fail.pearl+fail.shade+fail.silk;

	if (failure!=0) {
		client->sysmessage(const_cast<char*>(str.c_str()));
		return false;
	}

	return true;
}


/*!
\brief Plays failure effects
\author Xanatar
\param pc Little happy caster
*/
void spellFailFX(pChar pc)
{
	if ( ! pc ) return;
	if ((pc->spell < 0)||( pc->spell>89)) return;

	staticFX(pc, 0x3735, 0, 30);
	pc->playSFX(0x005C);
	pc->emote(pc->getSocket(), "The spell fizzles.",1);
}


/*!
\brief Casts an area attack spell like eartquake or chain lightning
\author Xanatar
\param epi Epicenter of the spell
\param spellnum Spell casted
\param pcaster Caster (can be NULL)
*/
void castAreaAttackSpell (sPoint epi, SpellId spellnum, pChar pcaster)
{
	NxwCharWrapper sc;
	uint32_t range = VISRANGE -2;
	if ( spellnum == SPELL_EXPLOSION )
		range = 4;

	sc.fillCharsNearXYZ( epi.x, epi.y, range );

	int damagetobedone = RandomNum(spellsData[spellnum].lodamage, spellsData[spellnum].hidamage);
	int divider = (sc.size() / 4) + 1;
	if (divider!=0) damagetobedone /= divider;

	if ( pcaster )
	{
		if ( spellnum == SPELL_EARTHQUAKE )
			pcaster->playSFX( 0x20D );
		
		if (checkTownLimits(spellnum, pcaster, pcaster, 0, 0, true))
			return;
	}

	for( sc.rewind(); !sc.isEmpty(); sc++ )
	{
		pChar pd = sc.getChar();
		if ( ! pd ) return;
			
		if ( (spellnum == SPELL_EARTHQUAKE || spellnum == SPELL_CHAINLIGHTNING)
			&& ( pd == pcaster ) )
				return;
		
		if ( spellnum == SPELL_EARTHQUAKE && pd->isMounting() )
				pd->unmountHorse();
		
		spellFX(spellnum, pcaster, pd);
		damage(pcaster, pd, spellnum, SPELLFLAG_PARAMISDAMAGE, damagetobedone);
	}
}


/*!
\brief Returns target type
\author Xanatar
\param spellnum Spell casted
\return target type
*/
static inline int spellTargetType(SpellId spellnum)
{
	switch(spellnum) {
		case spellFireball:
		case spellClumsy:
		case spellFeebleMind:
		case spellWeaken:
		case spellParalyze:
		case SPELL_DISPEL:
		case spellCurse:
		case spellPoison:
		case spellCunning:
		case spellAgility:
		case spellStrength:
		case spellBless:
		case spellHeal:
		case spellGreatHeal:
		case spellCure:
		case spellArchCure:
		case SPELL_RESURRECTION:
		case spellMagicArrow:
		case SPELL_FLAMESTRIKE:
		case SPELL_EXPLOSION:
		case spellLightning:
		case SPELL_ENERGYBOLT:
		case spellHarm:
		case spellMindBlast:
		case spellManaDrain:
		case SPELL_MANAVAMPIRE:
			return TARGTYPE_CHAR;

		case spellReactiveArmour:
		case spellProtection:
		case spellArchProtection:
		case spellIncognito:
		case spellReflection:
		case spellNightSight:
		case SPELL_INVISIBILITY:
		case spellSummon:
		case spellSummon_AIR:
		case spellSummon_DEAMON:
		case spellSummon_EARTH:
		case spellSummon_FIRE:
		case spellSummon_WATER:
		case SPELL_EARTHQUAKE:
		case spellCreateFood:
		case SPELL_POLYMORPH:
			return TARGTYPE_NONE;

		case spellTrap:
		case spellUntrap:
			return TARGTYPE_CONTAINER;

		case spellLock:
		case spellUnlock:
			return TARGTYPE_CONTAINERORDOOR;

		case SPELL_GATE:
		case SPELL_MARK:
		case spellRecall:
			return TARGTYPE_RUNE;

		case spellBladeSpirit:
		case SPELL_ENERGYVORTEX:
		case spellWallStone:
		case spellTeleport:
		case SPELL_MASSDISPEL:
		case SPELL_MASSCURSE:
		case SPELL_REVEAL:
		case spellFireField:
		case spellDispelField:
		case spellPoisonField:
		case spellParalyzeFIELD:
		case SPELL_ENERGYFIELD:
		case SPELL_CHAINLIGHTNING:
		case SPELL_METEORSWARM:
			return TARGTYPE_XYZ;

		default:
			return TARGTYPE_NONE;
	}
} PURE;


/*!
\brief Returns if spell requires a target
\author Xanatar
\param spellnum Spell to cast
*/
bool spellRequiresTarget(SpellId spellnum)
{
	return (spellTargetType(spellnum)!=TARGTYPE_NONE);
}


/*!
\brief Check if the target is too far from the caster
\author Luxor
\param caster Little wannabe caster
\param target a poor guinea pig ;]
*/
static bool checkDistance(pChar caster, pChar target)
{
	if(!caster) return false;
	if(!target) return false;

	pPC tmp;
	pClient caster_client;

	caster_client = (tmp = dynamic_cast<pPC>(caster))? tmp ->getClient() : NULL;

	if (caster->distFrom(target) > 15) {
		caster_client->sysmessage("You are too far away from the target.");
		return false;
	}
	if ( target->IsHidden() ) {
		caster_client->sysmessage("You cannot see your target.");
		return false;
	}
	return true;
}


/*!
\brief Check if there's something between the caster and the target
\author Luxor
\param caster a Little poor boy
\param destpos position of a little crazy girl ;]
*/
static bool checkLos(pChar caster, sLocation destpos)
{
	if(!caster) return false;

	pPC tmp = NULL;
	pClient caster_client = NULL;

	caster_client = (tmp = dynamic_cast<pPC>(caster))? tmp->getClient() : NULL;

        if (!line_of_sight(INVALID, caster->getPosition(), destpos, INVALID)) {
		caster_client->sysmessage("There is something between you and your target that makes the casting impossible.");
		return false;
	}
	return true;
}

/*!
\brief Checks if target is correct
\author Xanathar & Luxor
\param spellnum Spell number
\param t Target location
*/
bool checkRequiredTargetType(SpellId spellnum, TargetLocation& t)
{
	// 0:none,1:xyz,2:item,3:char,4:container or door,6:rune,5:container
	pItem pi = t.getItem();
	sLocation loc = t.getLocation();

	switch(spellTargetType(spellnum)) {
		case TARGTYPE_NONE :
			return true;
		case TARGTYPE_CONTAINER:
			return pi && dynamic_cast<pContainer>(pi);
		case TARGTYPE_CONTAINERORDOOR:
			return pi && ( dynamic_cast<pContainer>(pi) || pi->toDoor() );
		case TARGTYPE_XYZ :
			return loc != sPoint(0,0);
		case TARGTYPE_CHAR:
			return t.getChar();
		case TARGTYPE_ITEM:
			return pi;
		case TARGTYPE_RUNE:
			return pi && pi->toRune();
		default:
			return pi; //!\todo need to be changed
	}
}


/*!
\brief Delete reagents
\author Xanatar
\param pc caster
\param reags reagents
*/
void consumeReagents( pChar pc, sReagents reags )
{
	if ( ! pc ) return;
	if ( pc->dontUseReagents() ) return;
	pc->delItems(0x0F7A, reags.pearl);
	pc->delItems(0x0F7B, reags.moss);
	pc->delItems(0x0F84, reags.garlic);
	pc->delItems(0x0F85, reags.ginseng);
	pc->delItems(0x0F86, reags.drake);
	pc->delItems(0x0F88, reags.shade);
	pc->delItems(0x0F8C, reags.ash);
	pc->delItems(0x0F8D, reags.silk);
}


/*!
\brief Get spell number from scroll itemID
\author Xanatar
\param id Scroll ItemID
\return spell number
*/
SpellId spellNumberFromScrollId(int id)
{
	if (id==0x1F2D) 		return spellReactiveArmour;			// Reactive Armor
	if (id>=0x1F2E && id<=0x1F33)	return static_cast<SpellId>(id-0x1F2D-1);	// first circle without weaken
	if (id>=0x1F34 && id<=0x1F6C)	return static_cast<SpellId>(id-0x1F2D);		// 2 to 8 circle spell scrolls plus weaken
	return spellInvalid;
}


/*!
\author Xanathar & Luxor
\brief Casting function for stat pumping spells
\param spellnumber Spell to cast
\param dest target position
\param pa attacker mage
*/
static void castStatPumper(SpellId spellnumber, TargetLocation& dest, pChar pa, int flags, int param)
{
	int bonus = 10; //default
	pChar pd = NULL;
	int duration = 60; // one minute default

	if (pa) {
		int skilltouse = skMagery;
		if (flags&SPELLFLAG_PARAMISSKILLTOUSE) skilltouse = param;
		bonus = pa->skill[skilltouse] / 50;
		duration = pa->skill[skilltouse] / 10;
	}

	if (flags&SPELLFLAG_PARAMISBONUS) bonus = param;
	if (flags&SPELLFLAG_PARAMISDURATION) duration = param;

	if ((pd = dest.getChar())==NULL) return;

	pChar p_realAttacker = pa;
	pChar p_realDefender = pd;


	if ((pa!=NULL)&&(!(flags&SPELLFLAG_DONTREFLECT)))
		if (spellsData[spellnumber].reflect) checkReflection(pa, pd);

	if (pd == NULL) return; //paranoia >:]

	// early return for invulz : no bonus, no malus
	if ((spellsData[spellnumber].attackSpell)&&(pd->IsInvul())) return;

	// check resistance for damage resistance
	bool bResists = checkResist(pa, pd, spellnumber);
	if (flags&SPELLFLAG_IGNORERESISTANCE) bResists = false;

	// early return if in town and target should not be damaged
	if (checkTownLimits(spellnumber, pa, pd, flags, param)) return;

	if ((spellsData[spellnumber].attackSpell)&&(pa!=NULL)&&(!(flags&SPELLFLAG_DONTCRIMINAL)))
		p_realAttacker->attackStuff(p_realDefender);

	if ((!spellsData[spellnumber].attackSpell)&&(pa!=NULL)&&(!(flags&SPELLFLAG_DONTCRIMINAL)))
		p_realAttacker->helpStuff(p_realDefender);

	if ((spellsData[spellnumber].attackSpell)&&(bResists)) return;

	switch (spellnumber)
	{
		case spellClumsy:
			duration = ( ( pa->skill[skEvaluatingIntelligence] / 50 ) + 1 ) * 6;
			tempfx::add(pa, pd, tempfx::spellClumsy, bonus, 0, 0, duration);
			break;
		case spellFeebleMind:
			duration = ( ( pa->skill[skEvaluatingIntelligence] / 50 ) + 1 ) * 6;
			tempfx::add(pa, pd, tempfx::spellFeebleMind, bonus, 0, 0, duration);
			break;
		case spellWeaken:
			duration = ( ( pa->skill[skEvaluatingIntelligence] / 50 ) + 1 ) * 6;
			tempfx::add(pa, pd, tempfx::spellWeaken, bonus, 0, 0, duration);
			break;
		case spellCurse:
			duration = int( pa->skill[skMagery] * 0.12 );
			tempfx::add(pa, pd, tempfx::spellCurse, bonus, bonus, bonus, duration);
			break;
		case spellCunning:
			duration = ( ( pa->skill[skEvaluatingIntelligence] / 50 ) + 1 ) * 12;
			tempfx::add(pa, pd, tempfx::spellCunning, bonus, 0, 0, duration);
			break;
		case spellAgility:
			duration = ( ( pa->skill[skEvaluatingIntelligence] / 50 ) + 1 ) * 12;
			tempfx::add(pa, pd, tempfx::spellAgility, bonus, 0, 0, duration);
			break;
		case spellStrength:
			duration = ( ( pa->skill[skEvaluatingIntelligence] / 50 ) + 1 ) * 12;
			tempfx::add(pa, pd, tempfx::spellStrength, bonus, 0, 0, duration);
			break;
		case spellBless:
			duration = ( ( pa->skill[skEvaluatingIntelligence] / 50 ) + 1 ) * 12;
			tempfx::add(pa, pd, tempfx::spellBless, bonus, bonus, bonus, duration);
			break;
		default :
			ErrOut("Switch fallout. magic::castStatPumper\n");
	}
}


/*!
\brief Summon a NPC for the master ;]
\author Xanatar and Luxor
\param owner Master of NPC
\param npctype NPC type
\param duration How long NPC will be "under pressure"
\param bTamed Tamed or uncontrollable one

	Changes:  
		Luxor: added code for uncontrollable npcs. added code for xyz target.
*/
pChar summon (pChar owner, int npctype, int duration, bool bTamed, sLocation pos)
{
	if(!owner) return NULL;

	if ( pos == sLocation(0,0,0) || ! isValidCoord(pos) )
		pos = owner->getPosition();
	
	pChar pc = npcs::addNpc(npctype, pos);
	if(!pc) return NULL;

	if (bTamed) {
		pc->setOwner(owner);
		pc->setTamed(true);
	} else {
		pc->npcaitype = NPCAI_MADNESS; //Blade spirit, E-Vortex
	}
	pc->summontimer = getclock() + duration * SECS;
	return pc;
}


/*!
\brief Cast a Field
\param pc HarryPotter wannabe
\param pos position of mighty caster
\param spellnumber spell to cast
*/
void castFieldSpell( pChar pc, sPosition pos, int spellnumber)
{
	if ( ! pc ) return;
	int j = 0, fieldLen = 4;
	int fx[5], fy[5]; // bugfix LB, was fx[4] ...
	uint16_t id;

	if (pc!=NULL) j=fielddir(pc, pos); // lb bugfix, socket char# confusion

	if (j)
	{	fx[0]=fx[1]=fx[2]=fx[3]=fx[4]=pos.x; fy[0]=pos.y; fy[1]=pos.y+1; fy[2]=pos.y-1; fy[3]=pos.y+2; fy[4]=pos.y-2;}
	else
	{	fy[0]=fy[1]=fy[2]=fy[3]=fy[4]=pos.y; fx[0]=pos.x; fx[1]=pos.x+1; fx[2]=pos.x-1; fx[3]=pos.x+2; fx[4]=pos.x-2;}	// end else


	switch(spellnumber)
	{
		case spellWallStone:
			id = 0x0080; fieldLen = 2; break;
		case spellFireField:
			id = (j) ? 0x3996 : 0x398c; break;
		case spellPoisonField:
			id = (j) ? 0x3920 : 0x3915; break;
		case spellParalyzeFIELD:
			id = (j) ? 0x3979 : 0x3967; break;
		case SPELL_ENERGYFIELD:
			id = (j) ? 0x3956 : 0x3946; break;
		default:
			ErrOut("Switch fallout in magic::castFieldSpell\n");
			return;
	}

	int R;
	if (pc!=NULL) R = pc->region;

	if ((R<=INVALID)||(R>255)) R = 255;


	if ((region[R].priv&rgnFlagGuarded)&&(SrvParms->guardsactive)) return;

	for( j=0; j<=fieldLen; j++ )
	{
		int8_t nz=getHeight( sLocation( fx[j], fy[j], pos.z ) );
		pItem pi = cItem::addByID(id, 1, "#", 0, sLocation(fx[j], fy[j], nz));

		if (pi)
		{
			pi->setDecay();
			pi->setDispellable();
			pi->setDecayTime( (getclock()+(int(pc->skill[skMagery]/20)+4)*SECS) );
			pi->morex=pc->skill[skMagery]; // remember casters magery skill for damage, LB
			pi->dir=29;
			pi->magic=2;
			pi->refresh();
		}
	}
}


#define CHECKDISTANCE(A,B) if(!checkDistance(A,B) || !A->losFrom(B)) return; //Luxor


/*!
\brief the function which really does spell effects :]
\author Xanatar and Luxor
\param spellnumber Spell to Cast
\param dest Target location
\param src caster
\param flags spell flags
\param param spell param
*/
static void applySpell(SpellId spellnumber, TargetLocation& dest, pChar src, int flags, int param)
{
	if ( ! src ) return;
	
	pPC pc_tmp;
	pClient client = (pc_tmp = dynamic_cast<pPC>(src))? pc_tmp->getClient() : NULL;

	pChar pd = dest.getChar();
	pItem pi = dest.getItem();
	sLocation loc = dest.getLocation();

        sLocation srcpos= src->getPosition();

	//
        //Luxor:	Line of sight check
	//
	//Sparhawk:	Don't check los for spells on items in containers
	//
	if( ( ( pi != 0 ) && pi->isInWorld() ) || pd != 0 )
		if (!checkLos(src, loc))
			return;

	int nSkill = skMagery;
	int nValue = INVALID;
	int nTime = INVALID;

	if (flags&SPELLFLAG_PARAMISSKILLTOUSE) nSkill = param;
	if (flags&SPELLFLAG_PARAMISDAMAGE) nValue = param;
	if (flags&SPELLFLAG_PARAMISDURATION) nTime = param;



	switch (spellnumber)
	{
		case spellCunning:
		case spellAgility:
		case spellStrength:
		case spellBless:
			if (pd) {
				CHECKDISTANCE(src, pd);
				spellFX(spellnumber, src, pd);
				castStatPumper(spellnumber, dest, src, flags|SPELLFLAG_IGNORERESISTANCE, param);
			}
			break;
		case spellClumsy:
		case spellFeebleMind:
		case spellWeaken:
		case spellCurse:
			if (pd) {
				CHECKDISTANCE(src, pd);
				spellFX(spellnumber, src, pd);
				castStatPumper(spellnumber, dest, src, flags|SPELLFLAG_IGNORERESISTANCE, param);
			}
			break;
		case spellParalyze:
			if (pd) {
				CHECKDISTANCE(src, pd);
				spellFX(spellnumber, src, pd);
				tempfx::add(src, pd, tempfx::spellParalyze);
			}
			break;
		case spellPoison:
			if (pd) {
				CHECKDISTANCE(src, pd);
				spellFX(spellnumber, src, pd);
				if ( src->skill[skMagery] < 700 )
					pd->applyPoison(poisonWeak);
				else if ( src->skill[skMagery] < 900 )
					pd->applyPoison(poisonNormal);
				else
					pd->applyPoison(poisonGreater);
			}
			break;
		case spellMagicArrow:
		case spellFireball:
		case SPELL_FLAMESTRIKE:
		case spellLightning:
		case SPELL_EXPLOSION:
		case SPELL_ENERGYBOLT:
		case spellHarm:
		case SPELL_METEORSWARM:
		case SPELL_CHAINLIGHTNING:
		case SPELL_EARTHQUAKE:
			if (pd) {
				if (spellsData[spellnumber].areasize<=0 && (spellnumber!=SPELL_EXPLOSION || src->skill[skMagery] < 800)) //Luxor
				{
					CHECKDISTANCE(src, pd);
					spellFX(spellnumber, src, pd);
					damage(src, pd, spellnumber, flags, param);
				}
				else
				{
					if ( spellnumber == SPELL_EARTHQUAKE ) {  //Luxor
						pos = sPoint(srcpos);
					} else if ( spellnumber == SPELL_EXPLOSION ) {
						pos = sPoint(pd->getPosition());
					}
					castAreaAttackSpell(pos, spellnumber, src);
				}
			}
			break;

		case spellMindBlast:
			if (pd) {
				CHECKDISTANCE(src, pd);
				spellFX(spellnumber, src, pd);
				param = ( (src->in + 10) - pd->in ) / 2;
				damage(src, pd, spellnumber, flags|SPELLFLAG_PARAMISDAMAGE, param);
			}
			break;

		case spellManaDrain:
			if (pd) {
				CHECKDISTANCE(src, pd);
				spellFX(spellnumber, src, pd);
				damage(src, pd, spellnumber, flags, param);
			}
			break;

		case SPELL_MANAVAMPIRE:
			if (pd) {
				CHECKDISTANCE(src, pd);
				spellFX(spellnumber, src, pd);
				int manabogus = pd->mn;
				damage(src, pd, spellnumber, flags, param);
				if (src) {
					manabogus -= pd->mn;
					src->mn+=manabogus/2;
					if (src->mn > src->in) src->mn = src->in;
					src->updateMana();
				}
			}
			break;

		case spellLock:
			if (pi) {
				if( pi->toContainer() && !pi->toSecureContainer() )
				{
					//!\todo What the fuck is that?!?
					switch(pi->type)
					{
						case ITYPE_CONTAINER: pi->type=ITYPE_LOCKED_ITEM_SPAWNER; break;
						case ITYPE_UNLOCKED_CONTAINER: pi->type=ITYPE_LOCKED_CONTAINER; break;
					}
					if (src)
					{
						src->playSFX( 0x1F4 ); //Luxor
						client->sysmessage("It's locked!");
					}
				}
				else
					if (client)
						client->sysmessage("You cannot lock this!!!");
			}
			break;

		case spellUnlock:
			if ( pi && (pi->more1.more == 0) && (pi->isSecureContainer()) ) {
				switch(pi->type)
				{
					case ITYPE_LOCKED_ITEM_SPAWNER: pi->type=ITYPE_CONTAINER; break;
					case ITYPE_LOCKED_CONTAINER: pi->type=ITYPE_UNLOCKED_CONTAINER; break;
				}
				if (src) {
					src->playSFX( 0x1FF ); //Luxor
					client->sysmessage("You unlocked it!");
				}
			} else 
				client->sysmessage("You cannot unlock this!!!");
			break;

		case spellTrap:
			if (pi) {
				if(( pi->type==ITYPE_DOOR || pi->type==ITYPE_CONTAINER || pi->type==ITYPE_LOCKED_ITEM_SPAWNER ||
				     pi->type==ITYPE_LOCKED_CONTAINER || pi->type==ITYPE_UNLOCKED_CONTAINER) && pi->getId()!=0x0E75 )
				{
					pi->more2.moreb1=1;
					if (nValue!=-1) {
                    				pi->more2.moreb2=nValue/2;
						pi->more2.moreb3=nValue;
					} else if (src!=NULL) {
						pi->more2.moreb2=src->skill[nSkill]/20;
						pi->more2.moreb3=src->skill[nSkill]/10;
						src->playSFX( 0x1E9 ); //Luxor
						client->sysmessage("It's trapped!");
					} else {
						pi->more2.moreb2=13;
						pi->more2.moreb3=26;
					}
				} else if (client) client->sysmessage("You cannot trap this!!!");
			}
			break;

		case spellUntrap:
			if (pi) {
				if((  pi->type==ITYPE_DOOR || pi->type==ITYPE_CONTAINER || pi->type==ITYPE_LOCKED_ITEM_SPAWNER ||
				      pi->type==ITYPE_LOCKED_CONTAINER || pi->type==ITYPE_UNLOCKED_CONTAINER))
				{
					if(pi->more2.moreb1==1) {
						pi->more2.moreb1=0;
						pi->more2.moreb2=0;
						pi->more2.moreb3=0;
						src->playSFX( 0x1F0 ); //Luxor
						client->sysmessage("You successfully untrap this item!");
					} else if (client) client->sysmessage("This item doesn't seem to be trapped!");
				} else if (client) client->sysmessage("This item cannot be untrapped!");
			}
			break;

		case spellReactiveArmour:
			if (nTime==INVALID) nTime = src->skill[nSkill]/15;
			spellFX(spellnumber, src, src);
			tempfx::add(src,src, tempfx::SPELL_REACTARMOR, 0, 0, 0, nTime);
			break;

		case SPELL_DISPEL:	//Luxor
			if ( pd && pd->summontimer > 0 ) { //Only if it's a summoned creature
				pd->emoteall( "%s begins disappearing", true, pd->getCurrentName().c_str() );
				//3 seconds left
				if ( pd->summontimer > (getclock() + 3*SECS) )
					pd->summontimer = getclock() + 3*SECS;
				spellFX( spellnumber, pd );
			}
			break;

		case SPELL_MASSDISPEL: //Luxor
			{
			if ( pd ) {
				pos = sPoint(pd->getPosition());
			} else if ( pi ) {
				pos = sPoint(pi->getPosition());
			}
			NxwCharWrapper sc;
			pChar pc_curr;
			sc.fillCharsNearXYZ( loc.x, loc.y, src->skill[skMagery] / 100, true, false );
			for ( sc.rewind(); !sc.isEmpty(); sc++ ) {
				pc_curr = sc.getChar();
				if ( !pc_curr )
					continue;
				if ( pc_curr->summontimer > 0 ) {
					pc_curr->emoteall( "%s begins disappearing", true, pc_curr->getCurrentName().c_str() );
					if ( pc_curr->summontimer > (getclock() + 3*SECS) )
						pc_curr->summontimer = getclock() + 3*SECS;
				}
				spellFX( SPELL_DISPEL, pc_curr );
			}
			}
			break;

		case spellTelekinesys: // Luxor
			tempfx::add( src, src, tempfx::spellTelekinesys, 0, 0, 0, 10 );
			spellFX( spellnumber, src );
			break;

		case SPELL_POLYMORPH: //Luxor
			{
			P_MENU menu = Menus.insertMenu( new cPolymorphMenu( src ) );
			if(!menu) return;

			menu->show( src );
			}
			break;

		case SPELL_GATE: //Luxor
			if ( src && pi ) {
				if ( pi->type == ITYPE_RUNE ) {
					if ((pi->morex < 10)&&(pi->morey < 10)) {
						client->sysmessage("The rune is not marked yet.");
					} else {
						pItem pgate = item::CreateFromScript( "$item_a_blue_moongate" );
						if(!pgate) return;

						pgate->MoveTo( srcpos );
						pgate->morex = pi->morex;
						pgate->morey = pi->morey;
						pgate->morez = pi->morez;
						pgate->type = 51;
						pgate->setDecay( true );
						pgate->setDecayTime( getclock() + 30*SECS );
						pgate->Refresh();

						pItem pgate2 = item::CreateFromScript( "$item_a_blue_moongate" );
						if(!pgate2) return;

						pgate2->MoveTo( pi->morex, pi->morey, pi->morez );
						pgate2->morex = srcpos.x;
						pgate2->morey = srcpos.y;
						pgate2->morez = srcpos.z;
						pgate2->type = 51;
						pgate2->setDecay( true );
						pgate2->setDecayTime( getclock() + 30*SECS );
						pgate2->Refresh();

						spellFX( spellnumber, src );
					}
				} else
					client->sysmessage("That is not a rune!!");
			}
			break;

		case SPELL_MASSCURSE:
			{

			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( loc.x, loc.y, src->skill[skMagery] / 100, true);

			for( sc.rewind(); !sc.isEmpty(); sc++ ) {
				pChar pd = sc.getChar();
				if ( pd && pd->getSerial()!=src->getSerial()) {
					spellFX(spellnumber, src, pd);
					castStatPumper(spellCurse, dest, src, flags, param);
				}
			}
			break;

		case SPELL_REVEAL: //Luxor
			{
			spellFX(spellnumber, src);
			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( loc.x, loc.y, src->skill[skMagery] / 100, true);
			for( sc.rewind(); !sc.isEmpty(); sc++ ) {
				pChar pd = sc.getChar();
				if ( pd && pd->IsHidden() && !checkResist(src, pd, SPELL_REVEAL)) {
					if ( pd->IsHiddenBySpell() )
						pd->delTempfx( tempfx::SPELL_INVISIBILITY );
					else
						pd->unHide();
				}
			}
			}
			break;

		case spellProtection:
			spellFX(spellnumber, src, pd);
			if (nTime==INVALID) nTime = src->skill[nSkill]/15;
			if (nValue==INVALID) nValue = src->skill[nSkill]/10;
			tempfx::add(src,src, tempfx::spellProtection, nValue, 0, 0, nTime);
			break;

		case spellArchProtection:
			{
			if (src) {
				if (nTime==INVALID) nTime = 12;
				if (nValue==INVALID) nValue = 80;
			} else {
				if (nTime==INVALID) nTime = src->skill[nSkill]/15;
				if (nValue==INVALID) nValue = src->skill[nSkill]/10;
			}
			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( loc.x, loc.y, src->skill[skMagery] / 100, true);

			for( sc.rewind(); !sc.isEmpty(); sc++ ) {
				pChar pd = sc.getChar();
				if( pd ) {
					tempfx::add(pd,pd, tempfx::spellProtection, nValue, 0, 0, nTime);
					spellFX(spellnumber, src, pd);
				}
			}
			}
			break;

		case spellIncognito:
			spellFX(spellnumber, src, pd);
			if ( !pd && src ) pd = src;
			if (pd) {
				if (nTime==INVALID) nTime = 90;
				tempfx::add(pd,pd, tempfx::spellIncognito, 0,0,0, nTime);
			}
			break;

		case spellReflection:
			spellFX(spellnumber, src, pd);
			if ( !pd && src ) pd = src;
			if (pd) pd->setReflection(true);
			break;

		case SPELL_INVISIBILITY:
			spellFX(spellnumber, src, pd);
			if ( src->isHiddenBySpell() ) {
				src->delTempfx( tempfx::SPELL_INVISIBILITY );
			} else {
				if (nTime==INVALID) nTime = 90;
				src->hideBySpell(nTime);
			}
			break;

		case spellHeal:
		case spellGreatHeal:
			if ( !pd ) pd = src;
			if (pd) {
                                CHECKDISTANCE(src, pd);
                                spellFX(spellnumber, src, pd);
				if (pd->isHolyDamaged()) {
					damage(src, pd, spellnumber, flags|SPELLFLAG_DONTCRIMINAL, param);
				} else {
					if (nValue==INVALID) {
						(spellnumber==spellHeal) ? nValue = (1+src->skill[nSkill]/100) : nValue = (src->skill[nSkill]/30);
					/*} else if ((nValue==INVALID)&&(src==NULL)) {
						nValue = (spellnumber==spellHeal) ? 5 : 15;*/
					}
					pd->hp = min(pd->hp+nValue, pd->getStrength());
					src->helpStuff(pd);
					pd->updateHp();
				} // if !should damage
			} //pd!=NULL
			break;

		case spellCure:
			if (pd==NULL) pd = src;
                        CHECKDISTANCE(src, pd);
                        if (pd!=NULL) pd->curePoison();
			spellFX(spellnumber, src, pd);
			break;

		case spellArchCure: {
			CHECKDISTANCE(src, pd);
            		spellFX(spellnumber, src, pd);
			NxwCharWrapper sc;
			sc.fillCharsNearXYZ( loc.x, loc.y, src->skill[skMagery] / 100, true);

			for( sc.rewind(); !sc.isEmpty(); sc++ ) {
				pChar pd = sc.getChar();
				if( pd )
					pd->curePoison();
			}
			}
			break;

		case SPELL_RESURRECTION:
			if (pd==NULL) pd = src;
                        CHECKDISTANCE(src, pd);
                        if (pd!=NULL) {
				spellFX(spellnumber, src, pd);
				if (pd->isHolyDamaged()) {
					damage(src, pd, spellnumber, flags|SPELLFLAG_DONTCRIMINAL, param);
				} else {
					if ((pd->dead)&&(pd->IsOnline())) pd->resurrect();
					else if ( !pd->dead && client ) client->sysmessage("That player isn't dead!");
					else if ( !pd->IsOnline() && client ) client->sysmessage("That player isn't online!");
				}
			}
			break;

		case spellNightSight:
			if (pd==NULL) pd = src;
                        CHECKDISTANCE(src, pd);
                        if (pd!=NULL) {
				//CHECKDISTANCE(src, pd);
				spellFX(spellnumber, src, pd);
				if ((nTime==INVALID)) {
					if (src!=NULL) nTime = src->skill[nSkill] / 2;
					else nTime = 300; // 5' default
				}
				tempfx::add(src, pd, tempfx::SPELL_LIGHT, 0,0,0, nTime);
			}
			break;


		case spellFireField:
		case spellPoisonField:
		case spellParalyzeFIELD:
		case SPELL_ENERGYFIELD:
		case spellWallStone:
			spellFX(spellnumber, src, src);
			castFieldSpell( src, loc, spellnumber);
			break;

		case spellDispelField:
			if ( pi!=NULL )
			{
				if( pi->isDispellable() )
				{
					spellFX(spellnumber, src, pd);
					pi->Delete();
				}
			}
			break;

		case spellSummon:
			if (src!=NULL) { // Luxor
				P_MENU menu = Menus.insertMenu( new cSummonCreatureMenu( src ) );
				if(!menu) return;

				menu->show( src );
			}
			break;

		case spellSummon_AIR:
			if (src!=NULL) {
				spellFX(spellnumber, src, pd);
				nTime = (nTime==INVALID) ? (int)(src->skill[nSkill] * 0.4) : nTime;
				summon (src, xss::getIntFromDefine("$npc_summoned_air_elemental"), nTime, true, loc);
			}
			break;

		case spellSummon_DEAMON:
			if (src!=NULL) {
				spellFX(spellnumber, src, pd);
				if (nTime==INVALID) nTime = int(src->skill[nSkill] * 0.4);
				summon (src, xss::getIntFromDefine("$npc_summoned_deamon"), nTime, true, loc);
			}
			break;

		case spellSummon_EARTH:
			if (src!=NULL) {
				spellFX(spellnumber, src, pd);
				if (nTime==INVALID) nTime = int(src->skill[nSkill] * 0.4);
    			summon (src, xss::getIntFromDefine("$npc_summoned_earth_elemental"), nTime, true, loc);
			}
			break;

		case spellSummon_FIRE:
			if (src!=NULL) {
				spellFX(spellnumber, src, pd);
				if (nTime==INVALID) nTime = int(src->skill[nSkill] * 0.4);
    			summon (src, xss::getIntFromDefine("$npc_summoned_fire_elemental"), nTime, true, loc);
			}
			break;
		
		case spellSummon_WATER:
			if (src!=NULL) {
				spellFX(spellnumber, src, pd);
				if (nTime==INVALID) nTime = int(src->skill[nSkill] * 0.4);
				summon (src, xss::getIntFromDefine("$npc_summoned_water_elemental"), nTime, true, loc);
			}
			break;
		
		case spellBladeSpirit:
			if (src!=NULL) {
				spellFX(spellnumber, src, pd);
				if (nTime==INVALID) nTime = int(src->skill[nSkill] * 0.4);
				summon (src, xss::getIntFromDefine("$npc_summoned_blade_spirit"), nTime, false, loc);
			}
			break;
		
		case SPELL_ENERGYVORTEX:
			if (src!=NULL) {
				spellFX(spellnumber, src, pd);
				if (nTime==INVALID) nTime = int(src->skill[nSkill] * 0.4);
				summon (src, xss::getIntFromDefine("$npc_summoned_energy_vortex"), nTime, false, loc);
			}
			break;

		case SPELL_MARK:
			if ((src!=NULL)&&(pi!=NULL)) {
				if (pi->type==ITYPE_RUNE) {
					pi->morex = srcpos.x;
					pi->morey = srcpos.y;
					pi->morez = srcpos.z;
					client->sysmessage("Recall rune marked.");
					spellFX(spellnumber, src, pd);
				} else {
					client->sysmessage("That is not a rune!!");
				}// if a rune
			}
			break;

		case spellRecall:
			if ((src!=NULL)&&(pi!=NULL)) {
				if (src->isOverWeight()) {
					client->sysmessage("You're too heavy!");
				} else {
					if (pi->type==ITYPE_RUNE) {
						if ((pi->morex < 10)&&(pi->morey < 10)) {
							client->sysmessage("The rune is not marked yet.");
						} else {
							src->MoveTo( pi->morex, pi->morey, pi->morez );
							src->teleport();
							spellFX(spellnumber, src, pd);
						} // if rune marked ok
					} else {
						client->sysmessage("That is not a rune!!");
					}// if a rune
				} // if not overweight
			} // if src & pi valids
			break;

		case spellCreateFood:
			{ // Luxor
			P_MENU menu = Menus.insertMenu( new cCreateFoodMenu( src ) );
			if(!menu) return;

			menu->show( src );
			}
			break;

		case spellTeleport:
			//Luxor: now a mage cannot teleport to water
			/*bool isWater = false;
			map_st map;
			data::seekMap(loc.x, loc.y, map);
			switch(map.id)
			{
				//water tiles:
				case 0x00A8:
				case 0x00A9:
				case 0x00AA:
				case 0x00Ab:
				case 0x0136:
				case 0x0137:
				case 0x3FF0:
				case 0x3FF1:
				case 0x3FF2:
				case 0x2FF3:
					isWater = true;
					break;
				default:
					break;
			}
			land_st land;
			data::seekLand(map.id, land);
			if (land.flags&TILEFLAG_WET) isWater = true;*/
			//if (!isWater) {
				src->MoveTo(loc);
	                        src->teleport();
                        	spellFX(spellnumber, src, pd);
			//}
		break;
	}
	if (spellsData[spellnumber].attackSpell) src->attackStuff(pd);	//Luxor
}


/*!
\brief Cast a spell (direct action)
\author Xanatar
\param spellnumber Spell to cast
\param dest Target destination
\param src the caster
\param flags spell flags
\param param param
*/
void castSpell(SpellId spellnumber, TargetLocation& dest, pChar src, int flags, int param)
{

	if (!checkRequiredTargetType(spellnumber, dest)) return;

	if ( ! src ) return;

	// initial checks and unhide/unfreeze/disturbmed
	if (src->dead) return;
	src->unHide();
	src->disturbMed();
	src->spell=spellnumber;
	if ( src->spelltype !=CASTINGTYPE_ITEM && src->spelltype !=CASTINGTYPE_NOMANAITEM )
	{
		if ( src->skill[skMagery] < 900 ) //Luxor
			src->talkAll((char*)spellsData[src->spell].mantra.c_str(), false);
		else
			src->talkAllRunic((char*)spellsData[src->spell].mantra.c_str(), false);
		if (src->isMounting()) { //Luxor
			src->playAction(0x1B); // General Lee
		} else {
			src->playAction(spellsData[src->spell].action);
		}
	}
	// do the event :]

	pFunctionHandle evt = src->getEvent(cChar::evtChrOnCastSpell);
	if ( evt )
	{
		tVariantVector params = tVariantVector(4);
		params[0] = src->getSerial(); params[1] = spellnumber;
		params[2] = src->spelltype; params[3] = INVALID;
		evt->setParams(params);
		evt->execute();
		if ( evt->isBypassed() )
			return;
	}
	
	// check mana, don't bother the rest if no mana
	if ((!(flags&SPELLFLAG_NOUSEMANA)) && (!checkMana(src, spellnumber))) return;

	// check regs and consume them
	if (!(flags&SPELLFLAG_DONTREQREAGENTS)) {
		if (!checkReagents(src, spellsData[spellnumber].reagents)) return;
		consumeReagents( src, spellsData[spellnumber].reagents );
	}

	// if a skill needs to be checked.. check it :]
	// Sparhawk let's check this for pc's and npc's both
	//if ((!src->npc)&&(!(flags&SPELLFLAG_DONTCHECKSKILL))) {
	if ( !( flags & SPELLFLAG_DONTCHECKSKILL ) )
	{
		int skilltobechecked = skMagery;
		if ( flags&SPELLFLAG_PARAMISSKILLTOUSE )
			skilltobechecked = param;
		int loskill, hiskill;
		if ( ( flags & SPELLFLAG_BONUSCHANCE ) && ( SrvParms->cutscrollreq ) )
		{
			loskill=spellsData[spellnumber].sclo;
			hiskill=spellsData[spellnumber].schi;
		}
		else
		{
			loskill=spellsData[spellnumber].loskill;
			hiskill=spellsData[spellnumber].hiskill;
		}
		if (!src->checkSkill(static_cast<Skill>(skilltobechecked), loskill, hiskill)) {
			spellFailFX(src);
			return;
		}
	}

	// and now consume that fucking mana
	if (!(flags&SPELLFLAG_NOUSEMANA)) subtractMana(src, spellnumber);

	applySpell(spellnumber, dest, src, flags, param);
}


/*!
\brief Prepares spell casting
\author Xanatar
\param num Spell to cast
\param s Client of the caster
\param type Type of casting
*/
bool beginCasting (SpellId num, pClient s, CastingType type)
{
	if (!s) return true;

	// override for spellcasting (?)
	pPC pc = s->currChar();
	if(!pc) retun false;

	if ( pc->isDead() ) return false;

	// caster jailed ?
	if ( nJails::isJailed(pc) && !pc->isGM() )
	{
		s->sysmessage("You are in jail and cannot cast spells");
		return false;
	}

	// spell disabled ?
	if( spellsData[num].enabled != true )
	{
		s->sysmessage("Unseen forces make thou unable to cast that spell.");
		return false;
	}

	if ( pc->isHiddenBySpell() ) {	//Luxor: cannot do magic gestures if under invisible spell
		pc->sysmessage("You cannot cast by invisible.");
		return false;
	}
	if ((type!=CASTINGTYPE_ITEM)&&(type!=CASTINGTYPE_NOMANAITEM)&&(!pc->CanDoGestures())) {
		pc->sysmessage("You cannot cast with a weapon equipped.");
		return false;
	}

	pFunctionHandle evt = pc->getEvent(cChar::evtChrOnCastSpell);
	if ( evt )
	{
		tVariantVector params = tVariantVector(4);
		params[0] = pc->getSerial(); params[1] = num;
		params[2] = type; params[3] = INVALID;
		evt->setParams(params);
		evt->execute();
		if ( evt->isBypassed() )
			return;
	}
	
	pc->unHide();
	pc->disturbMed();

	if (type==CASTINGTYPE_SPELL && (!checkReagents(pc, spellsData[num].reagents))) return false;

	if ((type != CASTINGTYPE_ITEM) && (!checkMana(pc, num))) return false;

	pc->spelltype = type;
	pc->spell = num;
	pc->casting = 1;
	pc->nextact = 1;
	pc->spellaction = spellsData[num].action;

	if ((type==CASTINGTYPE_SPELL)&&(!pc->IsGM()))
		pc->spelltime = getCastingTime( num );
	else
		pc->spelltime = 0;

	return true;
}


/*!
\brief Constructor
\author Luxor
*/
cPolymorphMenu::cPolymorphMenu( pChar pc ) : cIconListMenu()
{
	if ( ! pc ) return;
	if ( pc->getTempfx( tempfx::SPELL_POLYMORPH ) != NULL )
		addIcon( 0x2106, 0, pc->getOldId(), std::string("Undo polymorph") );
	addIcon( 0x20CF, 0, 0xd3, std::string("Black Bear") );
	addIcon( 0x20DB, 0, 0xd4, std::string("Grizzly Bear") );
	addIcon( 0x20E1, 0, 0xd5, std::string("Polar Bear") );
	addIcon( 0x20D1, 0, 0xd0, std::string("Chicken") );
	addIcon( 0x20D3, 0, 0x9, std::string("Daemon") );
	addIcon( 0x20D8, 0, 0x2, std::string("Ettin") );
	addIcon( 0x20D9, 0, 0x4, std::string("Gargoyle") );
	addIcon( 0x20D6, 0, 0xc, std::string("Dragon") );
	addIcon( 0x20C9, 0, 0x1d, std::string("Gorilla") );
	addIcon( 0x20CA, 0, 0x23, std::string("Lizardman") );
	addIcon( 0x20DF, 0, 0x1, std::string("Ogre") );
	addIcon( 0x20D0, 0, 0xd7, std::string("Rat") );
	addIcon( 0x20D4, 0, 0xed, std::string("Deer") );
	addIcon( 0x20D7, 0, 0xe, std::string("Earth Elemental") );
	addIcon( 0x20E7, 0, 0x32, std::string("Skeleton") );
	addIcon( 0x2100, 0, 0x3a, std::string("Wisp") );
	addIcon( 0x211F, 0, 0xc8, std::string("Horse") );
	addIcon( 0x210B, 0, 0x10, std::string("Water Elemental") );

	question = std::string( "Choose a creature" );
}

/*!
\author Luxor
*/
void cPolymorphMenu::handleButton( pClient ps, cClientPacket* pkg  )
{
	pChar pc = ps->currChar();
	if ( ! pc ) return;

	cPacketResponseToDialog* p = (cPacketResponseToDialog*)pkg;
	std::map<uint32_t, int32_t>::iterator iter( iconData.find( p->index.get()-1 ) );
	uint16_t data = ( iter!=iconData.end() )? iter->second : INVALID;

	pc->delTempfx( tempfx::spellStrength );
	pc->delTempfx( tempfx::spellCunning );
	pc->delTempfx( tempfx::spellAgility );
	pc->delTempfx( tempfx::spellFeebleMind );
	pc->delTempfx( tempfx::spellClumsy );
	pc->delTempfx( tempfx::spellCurse );
	pc->delTempfx( tempfx::spellBless);
	pc->delTempfx( tempfx::spellWeaken );


	if ( pc->getTempfx( tempfx::SPELL_POLYMORPH ) != NULL && data == pc->getOldId() ) {
		pc->delTempfx( tempfx::SPELL_POLYMORPH );
		return;
	}

	pc->delTempfx( tempfx::SPELL_POLYMORPH );
	pc->addTempfx( *pc, tempfx::SPELL_POLYMORPH, data );

	switch( data )
	{
		case 0xd3:
		case 0xd4:
		case 0xd5:
			pc->addTempfx( *pc, tempfx::spellStrength, 40, 0, 0, polyduration );
			pc->addTempfx( *pc, tempfx::spellCurse, 0, 15, 20, polyduration );
			break;
		case 0xd0:
			pc->delTempfx( tempfx::spellWeaken );
			pc->addTempfx( *pc, tempfx::spellWeaken, 20, 0, 0, polyduration );
			break;
		case 0x9:
			pc->addTempfx( *pc, tempfx::spellBless, 10, 10, 10, polyduration );
			pc->addTempfx( *pc, tempfx::spellTelekinesys, 0, 0, 0, polyduration );
			break;
		case 0x2:
		case 0x4:
		case 0x1d:
		case 0x23:
		case 0x1:
		case 0x10:
			pc->addTempfx( *pc, tempfx::spellStrength, 30, 0, 0, polyduration );
			break;
		case 0xe:
			pc->addTempfx( *pc, tempfx::spellStrength, 50, 0, 0, polyduration );
			break;
		case 0xc8:
			pc->addTempfx( *pc, tempfx::spellAgility, 50, 0, 0, polyduration );
			break;
		case 0x3a:
			pc->addTempfx( *pc, tempfx::spellCunning, 40, 0, 0, polyduration );
			break;
		case 0x32:
			pc->addTempfx( *pc, tempfx::spellStrength, 10, 0, 0, polyduration );
			pc->addTempfx( *pc, tempfx::spellFeebleMind, 20, 0, 0, polyduration );
			break;
	}

	spellFX( SPELL_POLYMORPH, pc, pc );
}


/*!
\brief Constructor
\author Luxor
\since 0.82
*/
cCreateFoodMenu::cCreateFoodMenu( pChar pc ) : cIconListMenu()
{
	if ( ! pc ) return;

	addIcon( 0x9D0, 0, xss::getIntFromDefine("$item_apples"), std::string("Apple") );
	addIcon( 0x103C, 0, xss::getIntFromDefine("$item_bread_loaves"), std::string("Bread") );
	addIcon( 0x97C, 0, xss::getIntFromDefine("$item_wedges_of_cheese"), std::string("Cheese") );
	addIcon( 0x9F2, 0, xss::getIntFromDefine("$item_cuts_of_ribs"), std::string("Cut of ribs") );
	addIcon( 0x97B, 0, xss::getIntFromDefine("$item_fish_steaks"), std::string("Fish steak") );
	addIcon( 0x9D1, 0, xss::getIntFromDefine("$item_grape_bunches"), std::string("Grape bunch") );
	addIcon( 0x9C9, 0, xss::getIntFromDefine("$item_hams"), std::string("Ham") );
	addIcon( 0x9EA, 0, xss::getIntFromDefine("$item_muffins"), std::string("Muffin") );
	addIcon( 0x9D2, 0, xss::getIntFromDefine("$item_peaches"), std::string("Peach") );
	addIcon( 0x9C0, 0, xss::getIntFromDefine("$item_sausages"), std::string("Sausage") );

	question = std::string( "Choose a food type" );
}

/*!
\author Luxor
*/
void cCreateFoodMenu::handleButton( pClient ps, cClientPacket* pkg  )
{
	pChar pc = ps->currChar();
	if ( ! pc ) return;

	cPacketResponseToDialog* p = (cPacketResponseToDialog*)pkg;
	std::map<uint32_t, int32_t>::iterator iter( iconData.find( p->index.get()-1 ) );
	uint16_t data = ( iter!=iconData.end() )? iter->second : INVALID;

	item::CreateFromScript( data, pc->getBackpack() );
	spellFX( spellCreateFood, pc, pc );
}

/*!
\brief Constructor
\author Luxor
\since 0.82
*/
cSummonCreatureMenu::cSummonCreatureMenu( pChar pc ) : cIconListMenu()
{
	if ( ! pc ) return;

	addIcon( 0x211E, 0, xss::getIntFromDefine("$npc_a_brown_bear"), std::string("Bear") );
	addIcon( 0x211A, 0, xss::getIntFromDefine("$npc_forest_bird"), std::string("Bird") );
	addIcon( 0x20EF, 0, xss::getIntFromDefine("$npc_a_bull"), std::string("Bull") );
	addIcon( 0x211B, 0, xss::getIntFromDefine("$npc_a_cat"), std::string("Cat") );
	addIcon( 0x20D1, 0, xss::getIntFromDefine("$npc_a_chicken"), std::string("Chicken") );
	addIcon( 0x2102, 0, xss::getIntFromDefine("$npc_a_cougar"), std::string("Cougar") );
	addIcon( 0x2103, 0, xss::getIntFromDefine("$npc_a_cow"), std::string("Cow") );
	addIcon( 0x20D5, 0, xss::getIntFromDefine("$npc_a_dog"), std::string("Dog") );
	addIcon( 0x20F5, 0, xss::getIntFromDefine("$npc_a_gorilla"), std::string("Gorilla") );
	addIcon( 0x2124, 0, xss::getIntFromDefine("$npc_a_horse"), std::string("Horse") );
	addIcon( 0x20F6, 0, xss::getIntFromDefine("$npc_a_llama"), std::string("Lama") );
	addIcon( 0x2125, 0, xss::getIntFromDefine("$npc_a_rabbit"), std::string("Rabbit") );
	addIcon( 0x2108, 0, xss::getIntFromDefine("$npc_a_sheep"), std::string("Sheep") );

	question = std::string( "Choose a creature" );
}

/*!
\author Luxor
*/
void cSummonCreatureMenu::handleButton( pClient ps, cClientPacket* pkg  )
{
	pChar pc = ps->currChar();
	if ( ! pc ) return;

	cPacketResponseToDialog* p = (cPacketResponseToDialog*)pkg;
	std::map<uint32_t, int32_t>::iterator iter( iconData.find( p->index.get()-1 ) );
	uint16_t data = ( iter!=iconData.end() )? iter->second : INVALID;

	sLocation pos = pc->getPosition();
	pChar pc_monster = npcs::addNpc( data, pos.x, pos.y, pos.z );
	if(!pc_monster) return;

	pc_monster->setOwner( pc );
	pc_monster->tamed = true;
	pc_monster->summontimer = getclock() + uint32_t(pc->skill[skMagery] * 0.4) * SECS;
	pc_monster->MoveTo(pos);
	pc_monster->teleport();
	spellFX( spellSummon, pc, pc );
}


} // namespace
