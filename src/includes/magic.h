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
	static const uint16_t SPELLFLAG_NOUSEMANA		= 0x0001;
	//! Don't check criminal stuff
	static const uint16_t SPELLFLAG_DONTCRIMINAL		= 0x0002;
	//! Don't use up reagents
	static const uint16_t SPELLFLAG_DONTREQREAGENTS		= 0x0004;
	//! Don't check spellbook for spell presence
	static const uint16_t SPELLFLAG_DONTCHECKSPELLBOOK	= 0x0008;
	//! Optional parameter is the skill to be used instead of magery
	static const uint16_t SPELLFLAG_PARAMISSKILLTOUSE	= 0x0010;
	//! optional parameter is the damage to be done or healed
	static const uint16_t SPELLFLAG_PARAMISDAMAGE		= 0x0020;
	//! optional parameter is the bonus/malus to be applied
	static const uint16_t SPELLFLAG_PARAMISBONUS		= 0x0040;
	//! optional parameter is the npc to be invoked
	static const uint16_t SPELLFLAG_PARAMISNPCNUMBER	= 0x0080;
	//! optional parameter is the duration of some tempfx
	static const uint16_t SPELLFLAG_PARAMISDURATION		= 0x0100;
	//! Don't check magic reflection
	static const uint16_t SPELLFLAG_DONTREFLECT		= 0x0200;
	//! Don't check magic resistance
	static const uint16_t SPELLFLAG_IGNORERESISTANCE	= 0x0400;
	//! Don't check if in town
	static const uint16_t SPELLFLAG_IGNORETOWNLIMITS	= 0x0800;
	//! Don't check skill
	static const uint16_t SPELLFLAG_DONTCHECKSKILL		= 0x1000;
	//! Spell is silent
	static const uint16_t SPELLFLAG_SILENTSPELL		= 0x2000;
	//! Spell is immoble
	static const uint16_t SPELLFLAG_IMMOBLESPELL		= 0x4000;
	//! Spell has a great chance to hit (scrolls)
	static const uint16_t SPELLFLAG_BONUSCHANCE		= 0x8000;
	//@}
	
	//! this is the default for NPC casting :]
	static const uint16_t SPELLFLAGS_NPCDEFAULT = 
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
		spellInvalid = -1,
		spellClumsy,
		spellCreateFood,
		spellFeebleMind,
		spellHeal,
		spellMagicArrow,
		spellNightSight,
		spellReactiveArmour,
		spellWeaken,
		// Level 2
		spellAgility,
		spellCunning,
		spellCure,
		spellHarm,
		spellTrap,
		spellUntrap,
		spellProtection,
		spellStrenght,
		// Level 3
		spellBless,
		spellFireball,
		spellLock,
		spellPoison,
		spellTelekinesys,
		spellTeleport,
		spellUnlock,
		spellWallStone,
		// Level 4
		spellArchCure,
		spellArchProtection,
		spellCurse,
		spellFireField,
		spellGreatHeal,
		spellLightning,
		spellManaDrain,
		spellRecall,
		// Level 5
		SPELL_BLADESPIRITS,
		SPELL_DISPELFIELD,
		SPELL_INCOGNITO,
		SPELL_REFLECTION,
		SPELL_MINDBLAST,
		SPELL_PARALYZE,
		spellPoisonFIELD,
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

		MAX_SPELLS
	};

	enum CastingType {
		CASTINGTYPE_SPELL = 0,
		CASTINGTYPE_SCROLL,
		CASTINGTYPE_ITEM,
		CASTINGTYPE_NOMANAITEM,
		CASTINGTYPE_NPC
	};

	struct sReagents
	{
		uint16_t ginseng;
		uint16_t moss;
		uint16_t drake;
		uint16_t pearl;
		uint16_t silk;
		uint16_t ash;
		uint16_t shade;
		uint16_t garlic;
	};

	//! Spells' data
	struct sSpell
	{
		static const uint8_t flagEnabled	= 0x01; //!< Is the spell enabled?
		static const uint8_t flagReflect	= 0x02; //!< Is the spell reflectable?
		static const uint8_t flagRunic		= 0x04; //!< Should the wop said in runic?
		static const uint8_t flagAttack		= 0x04; //!< Is the spell an attack spell?
		
		uint8_t flags;		//!< Flags used
		uint8_t circle;		//!< Circle number
		uint16_t mana;		//!< Mana requirements
		uint16_t loskill;	//!< Low magery skill req.
		uint16_t hiskill;	//!< high magery skill req.
		uint16_t sclo;		//!< low magery skill req. if using scroll
		uint16_t schi;		//!< high magery skill req. if using scroll
		std::string mantra;	//!< Words of power
		uint16_t action;	//!< character action
		uint16_t delay;		//!< spell delay
		sReagents reagents;	//!< reagents req.
		std::string strToSay;	//!< string visualized with targ. system
		uint16_t lodamage;
		uint16_t hidamage;
		int areasize;
		int alwaysflag;
		DamageType damagetype;
	};

	enum {	TARGTYPE_NONE = 0, TARGTYPE_XYZ, TARGTYPE_ITEM, TARGTYPE_CHAR,
		TARGTYPE_CONTAINERORDOOR, TARGTYPE_CONTAINER, TARGTYPE_RUNE };

	extern sSpell spellsData[MAX_SPELLS];
	void loadSpells();
	
	bool checkMagicalSpeech( pChar pc, char* speech );
	bool beginCasting (SpellId num, pClient client, CastingType type);
	SpellId spellNumberFromScrollId(int id);
	void castSpell(SpellId spellnumber, TargetLocation& dest, pChar src = NULL, int flags = 0, int param = 0);
	void castAreaAttackSpell (sPoint epi, SpellId spellnum, pChar pcaster = NULL);
	bool spellRequiresTarget(SpellId spellnum);
	pChar summon (pChar owner, int npctype, int duration, bool bTamed = true, int x = INVALID, int y = INVALID, int z = INVALID);
	bool checkGateCollision( pChar pc );
	uint32_t getCastingTime( SpellId spell );
} // namespace

#endif
