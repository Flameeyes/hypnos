/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.hpp"
#include "magic.hpp"
#include "misc.hpp"
#include "inlines.hpp"

namespace npcs {

static int spherespells[256][256];

static void npcMagicAttack(pChar pc_att, pChar pc_def)
{
	if ( ! pc_att || ! pc_def )
		return;
	
	int spattackbit, spattacks, currenttime = getClockmSecs();
	// early return if prerequisites for spellcasting aren't true!
	// dirty,but helps losing some KG of code later :)

	if ((pc_att->spatimer > (uint32_t)currenttime)) return;

	//note : if magicsphere!=0, spattack has a different meaning!!
	spattacks = numbitsset( pc_att->spattack );

	if (pc_def->dead || pc_att->distFrom(pc_def)>=10 || spattacks <= 0 || pc_att->spattack==0) return;

	pc_att->spatimer=currenttime+(pc_att->spadelay*SECS); //LB bugkilling


	if ( pc_def->summontimer && pc_att->baseskill[skMagery] > 700 ) {
		pc_att->facexy( pc_def->getPosition().x, pc_def->getPosition().y );
		pc_att->beginCasting(pc_def, magic::spellDispel);
		return;
	}
	// We're here.. let's spellcast ;)
	if (pc_att->magicsphere!=0) npcCastSpell(pc_att, pc_def);
	else
	{
		spattackbit=rand()%(spattacks) + 1;
		pc_att->playAction(6);

		switch(whichbit(pc_att->spattack, spattackbit))
		{
			case 1:
				pc_att->beginCasting(pc_def, magic::spellMagicArrow );
				break;
			case 2:
				pc_att->beginCasting(pc_def, magic::spellHarm );
				break;
			case 3:
				pc_att->beginCasting(pc_def, magic::spellClumsy );
				break;
			case 4:
				pc_att->beginCasting(pc_def, magic::spellFeebleMind );
				break;
			case 5:
				pc_att->beginCasting(pc_def, magic::spellWeaken );
				break;
			case 6:
				pc_att->beginCasting(pc_def, magic::spellFireball );
				break;
			case 7:
				pc_att->beginCasting(pc_def, magic::spellCurse );
				break;
			case 8:
				pc_att->beginCasting(pc_def, magic::spellLightning );
				break;
			case 9:
				pc_att->beginCasting(pc_def, magic::spellParalyze );
				break;
			case 10:
				pc_att->beginCasting(pc_def, magic::spellMindBlast );
				break;
			case 11:
				pc_att->beginCasting(pc_def, magic::spellEnergyBolt );
				break;
			case 12:
				pc_att->beginCasting(pc_def, magic::spellExplosion );
				break;
			case 13:
				pc_att->beginCasting(pc_def, magic::spellFlameStrike );
				break;
			case 14:
				pc_att->beginCasting(pc_def, magic::spellMindBlast );
				break;
			case 15:
				pc_att->beginCasting(pc_def, magic::spellMindBlast );
				break;
			case 16:
				pc_att->beginCasting(pc_def, magic::spellMindBlast );
				break;
			default:
				break;
		}
	}
}

#define CHECKSPELL(NAME,VALUE) { if ( script1 == NAME ) { spherespells[section][ptr++] = VALUE; continue;	} }
static void initNpcSpells ()
{
	int ptr = 0;

	outPlain("Loading custom NPC grimoires (npcmagic.xss)...");

	for ( int i=0; i<256; ++i )
		for ( int j=0; j<256; ++j )
			spherespells[i][j] = 0;

	cScpIterator*	iter = NULL;
	string	script1,
			script2;

	for (int section=0; section<256; ++section )
	{
		safedelete(iter);
		iter = Scripts::NpcMagic->getNewIterator( "SECTION SPHERE %d", section );
		if (iter != 0 )
		{
			ptr = 0;
			do {
				iter->parseLine(script1, script2);
				if ((script1[0]!='}')&&(script1[0]!='{')) {
					CHECKSPELL("SUMMON", (-atoi(script2.c_str())));
					CHECKSPELL("MAGICARROW", 1);
					CHECKSPELL("HARM", 2);
					CHECKSPELL("CLUMSY", 3);
					CHECKSPELL("FEEBLEMIND", 4);
					CHECKSPELL("WEAKEN", 5);
					CHECKSPELL("FIREBALL", 6);
					CHECKSPELL("CURSE", 7);
					CHECKSPELL("LIGHTNING", 8);
					CHECKSPELL("PARALYZE", 9);
					CHECKSPELL("MINDBLAST", 10);
					CHECKSPELL("ENERGYBOLT", 11);
					CHECKSPELL("EXPLOSION", 12);
					CHECKSPELL("FLAMESTRIKE", 13);
					CHECKSPELL("FIREFIELD", 14);
					CHECKSPELL("POISONFIELD", 15);
					CHECKSPELL("PARALYZEFIELD", 16);
					CHECKSPELL("HEAL", 17);
					CHECKSPELL("GREATHEAL", 18);
					CHECKSPELL("GREATCURE", 19);
					CHECKSPELL("PIROMANCY", 20);
					CHECKSPELL("POISOMANCY", 21);
					CHECKSPELL("POISON", 22);
					CHECKSPELL("MANADRAIN", 23);
					CHECKSPELL("MANAVAMPIRE", 24);
					CHECKSPELL("BLADESPIRITS", 25);
					CHECKSPELL("ENERGYVORTEX", 26);
					CHECKSPELL("MAGICREFLECTION", 27);
					CHECKSPELL("CHAINLIGHTNING", 28);
					CHECKSPELL("METEORSWARM", 29);
					CHECKSPELL("EARTHQUAKE", 30);
				}
			} while (script1[0]!='}');
		}

	}
	safedelete(iter);
	outPlain("[DONE]\n");
}
#undef CHECKSPELL

static void npcCastSpell(pChar pc_att, pChar pc_def)
{
	if ( ! pc_att || ! pc_def )
		return;
	
	int sphere = (pc_att->magicsphere) % 255;
	int spell = (rand()%pc_att->spattack) % 255;


	if (spherespells[sphere][spell]==0) return;

	pFunctionHandle evt = pc_att->getEvent(cChar::evtChrOnCastSpell);
	if ( evt )
	{
		cVariantVector params = cVariantVector(4);
		params[0] = pc_att->getSerial(); params[1] = spell;
		params[2] = -1; params[3] = sphere;
		evt->setParams(params);
		evt->execute();
		if ( evt->isBypassed() )
			return;
	}
	
	pc_att->playAction(6);

	if (spherespells[sphere][spell]<0) {
		//summon an NPC
		if (pc_att->mn>=40) {
			pc_att->emoteall("*Doing a summoning ritual.*",1);
			AddNPC(INVALID, NULL, -spherespells[sphere][spell], pc_def->getPosition());
			pc_att->mn -= 40;
		}
		return;
	}

	switch(spherespells[sphere][spell])
	{
		case 1:
			pc_att->beginCasting(pc_def, magic::spellMagicArrow );
			break;
		case 2:
			pc_att->beginCasting(pc_def, magic::spellHarm );
			break;
		case 3:
			pc_att->beginCasting(pc_def, magic::spellClumsy );
			break;
		case 4:
			pc_att->beginCasting(pc_def, magic::spellFeebleMind );
			break;
		case 5:
			pc_att->beginCasting(pc_def, magic::spellWeaken );
			break;
		case 6:
			pc_att->beginCasting(pc_def, magic::spellFireball );
			break;
		case 7:
			pc_att->beginCasting(pc_def, magic::spellCurse );
			break;
		case 8:
			pc_att->beginCasting(pc_def, magic::spellLightning );
			break;
		case 9:
			pc_att->beginCasting(pc_def, magic::spellParalyze );
			break;
		case 10:
			pc_att->beginCasting(pc_def, magic::spellMindBlast );
			break;
		case 11:
			pc_att->beginCasting(pc_def, magic::spellEnergyBolt );
			break;
		case 12:
			pc_att->beginCasting(pc_def, magic::spellExplosion );
			break;
		case 13:
			pc_att->beginCasting(pc_def, magic::spellFlameStrike );
			break;
		case 14:
			pc_att->beginCasting(pc_def, magic::spellFireField );
			break;
		case 15:
			pc_att->beginCasting(pc_def, magic::spellPoisonField );
			break;
		case 16:
			pc_att->beginCasting(pc_def, magic::spellParalyzeField );
			break;
		case 17:
			pc_att->beginCasting(pc_att, magic::spellHeal );
			break;
		case 18:
			pc_att->beginCasting(pc_att, magic::spellGreatHeal );
			break;
		case 19:
			pc_att->beginCasting(pc_att, magic::spellCure );
			break;
		case 22:
			pc_att->beginCasting(pc_def, magic::spellPoison );
			break;
		case 23:
			pc_att->beginCasting(pc_def, magic::spellManaDrain );
			break;
		case 24:
			pc_att->beginCasting(pc_def, magic::spellManaVampire );
			break;
		case 27:
			pc_att->beginCasting(pc_att, magic::spellReflection );
			break;
		default :
			LogWarning("NPC-Spell %d not yet implemented, sorry :(\n", spherespells[sphere][spell]);
			break;
	}

}
//END MAGIG STUFF

} //namespace
