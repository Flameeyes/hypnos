/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Magic Related stuff
*/

#ifndef __MAGIC_H__
#define __MAGIC_H__

#include "common_libs.h"
#include "targeting.h"
#include "menu.h"
#include "enums.h"

/*!
\brief Magic related stuff
*/
namespace magic {
	//@{
	/*!
	\name Spell flags
	\brief Flag used on spell casting
	*/
	
	//! Don't use up mana
	static const uint8_t SPELLFLAG_NOUSEMANA		= 0x0001;
	//! Don't check criminal stuff
	static const uint8_t SPELLFLAG_DONTCRIMINAL		= 0x0002;
	//! Don't use up reagents
	static const uint8_t SPELLFLAG_DONTREQREAGENTS		= 0x0004;
	//! Don't check spellbook for spell presence
	static const uint8_t SPELLFLAG_DONTCHECKSPELLBOOK	= 0x0008;
	//! Optional parameter is the skill to be used instead of magery
	static const uint8_t SPELLFLAG_PARAMISSKILLTOUSE	= 0x0010;
	//! optional parameter is the damage to be done or healed
	static const uint8_t SPELLFLAG_PARAMISDAMAGE		= 0x0020;
	//! optional parameter is the bonus/malus to be applied
	static const uint8_t SPELLFLAG_PARAMISBONUS		= 0x0040;
	//! optional parameter is the npc to be invoked
	static const uint8_t SPELLFLAG_PARAMISNPCNUMBER		= 0x0080;
	//! optional parameter is the duration of some tempfx
	static const uint8_t SPELLFLAG_PARAMISDURATION		= 0x0100;
	//! Don't check magic reflection
	static const uint8_t SPELLFLAG_DONTREFLECT		= 0x0200;
	//! Don't check magic resistance
	static const uint8_t SPELLFLAG_IGNORERESISTANCE		= 0x0400;
	//! Don't check if in town
	static const uint8_t SPELLFLAG_IGNORETOWNLIMITS		= 0x0800;
	//! Don't check skill
	static const uint8_t SPELLFLAG_DONTCHECKSKILL		= 0x1000;
	//! Spell is silent
	static const uint8_t SPELLFLAG_SILENTSPELL		= 0x2000;
	//! Spell is immoble
	static const uint8_t SPELLFLAG_IMMOBLESPELL		= 0x4000;
	//! Spell has a great chance to hit (scrolls)
	static const uint8_t SPELLFLAG_BONUSCHANCE		= 0x8000;
	//@}
	
	//! this is the default for NPC casting :]
	static const uint8_t SPELLFLAGS_NPCDEFAULT = 
		SPELLFLAG_DONTCRIMINAL|
		SPELLFLAG_DONTREQREAGENTS|
		SPELLFLAG_DONTCHECKSPELLBOOK|
		SPELLFLAG_IGNORETOWNLIMITS|
		SPELLFLAG_DONTCHECKSKILL;
	
	
	enum FieldType
	{
		fieldInvalid = 0,
		fieldFire,
		fieldPoison,
		fieldParalyse,
		fieldEnergy
	};

	/*!
	\author Luxor
	\brief Polymorph menu
	*/
	class cPolymorphMenu : public cIconListMenu {
	public:
		cPolymorphMenu( pChar pc );
		virtual void handleButton( pClient client, void /*cClientPacket*/* pkg  );
	};

	/*!
	\author Luxor
	\brief Create food menu
	*/
	class cCreateFoodMenu : public cIconListMenu {
	public:
		cCreateFoodMenu( pChar pc );
		virtual void handleButton( pClient client, void /*cClientPacket*/* pkg  );
	};

	/*!
	\author Luxor
	\brief Summon creature menu
	*/
	class cSummonCreatureMenu : public cIconListMenu {
	public:
		cSummonCreatureMenu( pChar pc );
		virtual void handleButton( pClient client, void /*cClientPacket*/* pkg  );
	};

	/*!
	\brief Spells ID
	*/
	enum SpellId {
		// Level 1
		SPELL_CLUMSY = 0,
		SPELL_CREATEFOOD,
		SPELL_FEEBLEMIND,
		SPELL_HEAL,
		SPELL_MAGICARROW,
		SPELL_NIGHTSIGHT,
		SPELL_REACTIVEARMOUR,
		SPELL_WEAKEN,
		// Level 2
		SPELL_AGILITY,
		SPELL_CUNNING,
		SPELL_CURE,
		SPELL_HARM,
		SPELL_TRAP,
		SPELL_UNTRAP,
		SPELL_PROTECTION,
		SPELL_STRENGHT,
		// Level 3
		SPELL_BLESS,
		SPELL_FIREBALL,
		SPELL_LOCK,
		SPELL_POISON,
		SPELL_TELEKINESYS,
		SPELL_TELEPORT,
		SPELL_UNLOCK,
		SPELL_WALLSTONE,
		// Level 4
		SPELL_ARCHCURE,
		SPELL_ARCHPROTECTION,
		SPELL_CURSE,
		SPELL_FIREFIELD,
		SPELL_GREATHEAL,
		SPELL_LIGHTNING,
		SPELL_MANADRAIN,
		SPELL_RECALL,
		// Level 5
		SPELL_BLADESPIRITS,
		SPELL_DISPELFIELD,
		SPELL_INCOGNITO,
		SPELL_REFLECTION,
		SPELL_MINDBLAST,
		SPELL_PARALYZE,
		SPELL_POISONFIELD,
		SPELL_SUMMON,
		// Level 6
		SPELL_DISPEL,
		SPELL_ENERGYBOLT,
		SPELL_EXPLOSION,
		SPELL_INVISIBILITY,
		SPELL_MARK,
		SPELL_MASSCURSE,
		SPELL_PARALYZEFIELD,
		SPELL_REVEAL,
		// Level 7
		SPELL_CHAINLIGHTNING,
		SPELL_ENERGYFIELD,
		SPELL_FLAMESTRIKE,
		SPELL_GATE,
		SPELL_MANAVAMPIRE,
		SPELL_MASSDISPEL,
		SPELL_METEORSWARM,
		SPELL_POLYMORPH,
		// Level 8
		SPELL_EARTHQUAKE,
		SPELL_ENERGYVORTEX,
		SPELL_RESURRECTION,
		SPELL_SUMMON_AIR,
		SPELL_SUMMON_DEAMON,
		SPELL_SUMMON_EARTH,
		SPELL_SUMMON_FIRE,
		SPELL_SUMMON_WATER,

		MAX_SPELLS,
		SPELL_INVALID = -1
	};

	enum CastingType {
		CASTINGTYPE_SPELL = 0,
		CASTINGTYPE_SCROLL,
		CASTINGTYPE_ITEM,
		CASTINGTYPE_NOMANAITEM,
		CASTINGTYPE_NPC
	};

	struct reag_st
	{
		uint32_t ginseng;
		uint32_t moss;
		uint32_t drake;
		uint32_t pearl;
		uint32_t silk;
		uint32_t ash;
		uint32_t shade;
		uint32_t garlic;
	};

	struct g_Spell
	{
		bool enabled;		// spell enabled?
		int circle; 		// circle number
		int mana;			// mana requirements
		int loskill;		// low magery skill req.
		int hiskill;		// high magery skill req.
		int sclo;			// low magery skill req. if using scroll
		int schi;			// high magery skill req. if using scroll
		//char mantra[27];	// words of power
		std::string mantra;	// words of power
		int action; 		// character action
		int delay;			// spell delay
		reag_st reagents;	// reagents req.
		//char strToSay[102]; // string visualized with targ. system
		std::string strToSay; // string visualized with targ. system
		bool reflect;		// 1=spell reflectable, 0=spell not reflectable
		bool runic;
		int lodamage;
		int hidamage;
		bool attackSpell;
		int areasize;
		int alwaysflag;
		DamageType damagetype;
	};

	enum {	TARGTYPE_NONE = 0, TARGTYPE_XYZ, TARGTYPE_ITEM, TARGTYPE_CHAR,
		TARGTYPE_CONTAINERORDOOR, TARGTYPE_CONTAINER, TARGTYPE_RUNE };

	extern g_Spell g_Spells[MAX_SPELLS];

	bool checkMagicalSpeech( pChar pc, char* speech );
	bool beginCasting (SpellId num, pClient client, CastingType type);
	SpellId spellNumberFromScrollId(int id);
	void castSpell(SpellId spellnumber, TargetLocation& dest, pChar src = NULL, int flags = 0, int param = 0);
	void castAreaAttackSpell (int x, int y, SpellId spellnum, pChar pcaster = NULL);
	void loadSpellsFromScript();
	bool spellRequiresTarget(SpellId spellnum);
	pChar summon (pChar owner, int npctype, int duration, bool bTamed = true, int x = INVALID, int y = INVALID, int z = INVALID);
	bool checkGateCollision( pChar pc );
	uint32_t getCastingTime( SpellId spell );


} // namespace

#endif
