/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declaration of cChar class
*/
#ifndef __CCHAR_H__
#define __CCHAR_H__

#include "common_libs.h"
#include "objects/cbody.h"
#include "objects/ceventthrower.h"
#include "magic.h"
#include "npcs.h"
#include "target.h"
#include "constants.h"
#include "menu.h"
#include "logsystem.h"
#include "basics.h"
#include "map.h"
#include "settings.h"
#include "enums.h"

enum WanderMode {
	WANDER_NOMOVE = 0,
	WANDER_FOLLOW,
	WANDER_FREELY_CIRCLE,
	WANDER_FREELY_BOX,
	WANDER_FREELY,
	WANDER_FLEE,
	WANDER_AMX
};

/*!
\brief AMX Events for Characters
\author Flameeyes
*/

enum StatCap
{
	STATCAP_CAP			= 0,
	STATCAP_STR,
	STATCAP_DEX,
	STATCAP_INT
};

#define REPUTATION_KARMA		1
#define REPUTATION_FAME			2

class cNxwClientObj;
class cPath;
class ClientCrypt;

/*!
\class cChar cchar.h "objects/cchar.h"
\brief Character class
*/
class cChar : public cSerializable, virtual public cEventThrower
{
public:
	enum {
		evtChrOnDeath = evtGenericMax,
		evtChrBeforeDeath,
		evtChrAfterDeath,
		evtChrOnDied,
		evtChrOnWounded,
		evtChrOnHit,
		evtChrOhHitMiss,
		evtChrOnGetHit,
		evtChrOnReputationChange,
		evtChrOnResurrect,
		evtChrOnWalk,
		evtChrOnAdvanceSkill,
		evtChrOnAdvanceStat,
		evtChrOnBeginAttack,
		evtChrOnBeginDefence,
		evtChrOnMultiEnter,
		evtChrOnMultiLeave,
		evtChrOnSnooped,
		evtChrOnStolen,
		evtChrOnPoisoned,
		evtChrOnRegionChange,
		evtChrOnCastSpell,
		evtChrOnGetSkillCap,
		evtChrOnGetStatCap,
		evtChrOnBlock,
		evtChrOnBreakMeditation,
		evtChrOnClick,
		evtChrOnMount,
		evtChrOnDismount,
		evtChrOnKill,
		evtChrOnHearPlayer,
		evtChrOnDoCombat,
		evtChrOnCombatHit,
		evtChrOnSpeech,
		evtChrMax
	};

protected:
	static uint32_t nextSerial;
	uint32_t getNewSerial();
	virtual uint16_t eventsNumber() const
	{ return evtChrMax; }
	
	cChar();
	cChar(uint32_t ser);
	void resetData();
public:
	~cChar();

	virtual const std::string getPopupHelp() const = 0;
	void MoveTo(sLocation newloc);
	void loadEventFromScript(char *script1, char *script2);
	void doGmEffect();
	
protected:
	pClient client;	//!< Client connected with the character
	pBody body;     //! The body the character is currently "using"
	pBody truebody;  //! true body. Holds native character body at all times. Used for reversion of polimorph-type effects

public:
	inline pClient getClient() const
	{ return client; }

	inline void setClient(pClient c)
	{ client = c; }
        
        inline pBody getBody() const
	{ return body; }

	inline void setBody(pBody b)
	{ body = b; }

        inline pBody getTrueBody() const
	{ return truebody; }

	inline void setTrueBody(pBody b)
	{ truebody = b; }

	//! get online status
	inline const bool isOnline() const
	{ return client; }

	/*!
	\brief Gets the location of a serial instance in the world
	\author Flameeyes
	\note This function is a wrapper to cBody::getPosition()
	*/
	sLocation getWorldLocation() const
	{ return getBody()->getPosition(); }

//@{
/*!
\name Character's Flag
*/
public:
	static const uint64_t flagGrey			= 0x0000000000000001ull; //!< Char is grey
	static const uint64_t flagPermaGrey		= 0x0000000000000002ull; //!< Char is permanent grey
	static const uint64_t flagResistParalisys	= 0x0000000000000004ull; //!< Char resists to paralisys (unused)
	static const uint64_t flagWarMode		= 0x0000000000000008ull; //!< Char is in war mode
	static const uint64_t flagSpellTelekinesys	= 0x0000000000000010ull; //!< Char under telekinesys spell (Luxor)
	static const uint64_t flagSpellProtection	= 0x0000000000000020ull; //!< Char under protection spell (Luxor)

	static const uint64_t flagKarmaInnocent		= 0x0000000000000040ull; //!< Char is innocent
	static const uint64_t flagKarmaMurderer		= 0x0000000000000080ull; //!< Char is murderer
	static const uint64_t flagKarmaCriminal		= 0x0000000000000100ull; //!< Char is criminal

	static const uint64_t flagInvulnerable		= 0x0000000000000200ull; //!< Char is invulnerable
	static const uint64_t flagNoSkillTitle		= 0x0000000000000400ull; //!< Char hasn't skill title

	static const uint64_t flagFrozen		= 0x0000000000000800ull;
	static const uint64_t flagPermaHidden		= 0x0000000000001000ull;
	static const uint64_t flagNoUseMana		= 0x0000000000002000ull;
	static const uint64_t flagReflection		= 0x0000000000004000ull;
	static const uint64_t flagNoUseReagents		= 0x0000000000008000ull;

	static const uint64_t flagPolymorphed		= 0x0000000000010000ull;
	static const uint64_t flagDead			= 0x0000000000020000ull;

	static const uint64_t flagAttackFirst		= 0x0000000000040000ull;
	static const uint64_t flagDoorUse		= 0x0000000000080000ull;

	static const uint64_t flagIsCasting		= 0x0000000000100000ull;
	static const uint64_t flagIsGuarded		= 0x0000000000200000ull;
	
	static const uint64_t flagHolyDamaged		= 0x0000000000400000ull;
	static const uint64_t flagLightDamaged		= 0x0000000000800000ull;

	static const uint64_t flagIsMeditating		= 0x0000000001000000ull; //!< Char is using meditation
//@}

//@{
/*!
\name Char Status
*/
protected:
	HideType hidden; //!< Hide status of the char
			 //!< \see HideType
	
	uint64_t flags;	//!< Flags for the character

	int32_t karma;	//!< karma of the char
	int32_t fame;	//!< fame of the char

	float  fstm;	//!< Unavowed - stamina to remove the next step

	inline void setFlag(uint64_t flag, bool set)
	{
		if ( set ) flags |= flag;
		else flags &= ~flag;
	}

public:
	inline void setHidden(HideType ht)
	{ hidden = ht; }

	inline const bool isHidden() const
	{ return hidden != htUnhidden; }

	inline const bool isHiddenBySpell() const
	{ return hidden == htBySpell; }

	inline const bool isHiddenBySkill() const
	{ return hidden == htBySkill; }

	//! Return the karma of the char
	inline const int32_t getKarma() const
	{ return karma; }

	//! Return the fame of the char
	inline const int32_t getFame() const
	{ return fame; }

	inline const bool isInvul() const
	{ return flags & flagInvulnerable; }

	inline const bool isFrozen() const
	{ return flags & flagFrozen; }

	inline const bool isPermaHidden() const
	{ return flags & flagPermaHidden; }

	inline const bool isInnocent() const
	{ return (flags & flagKarmaInnocent); }

	inline const bool isMurderer() const
	{ return (flags & flagKarmaMurderer); }

	inline const bool isCriminal() const
	{ return (flags & flagKarmaCriminal); }

	inline const bool canUseDoor() const
	{ return flags & flagDoorUse; }

	inline const bool dontUseMana() const
	{ return flags & flagNoUseMana; }

	inline const bool dontUseReagents() const
	{ return flags & flagNoUseReagents; }

	inline const bool hasReflection() const
	{ return flags & flagReflection; }

        inline const bool hasTelekinesis() const
        { return flags & flagSpellTelekinesys; }
	
	inline const bool isDead() const
	{ return flags & flagDead; }

	inline const bool inGuardedArea() const
	{ return false/*::region[region].priv & RGNPRIV_GUARDED*/;
	//!\todo need to fix this
	}

	const bool isGrey() const;
	
	inline const bool holyDamaged() const
	{ return flags & flagHolyDamaged; }
	
	inline const bool lightDamaged() const
	{ return flags & flagLightDamaged; }
	
	inline const bool isMeditating() const
	{ return flags & flagIsMeditating; }
	
	/*!
	\author Xanathar
	\brief Checks char weight
	\note this function modify the class variable, very bad...
	\return true if the char is over weight
	\todo Reactivate GM Support
	*/
	inline const bool cChar::isOverWeight()
	{ return /*!isGM() && */body->overloadedTeleport(); }
	
	const bool canDoGestures() const;
	const bool inDungeon() const;

	//! Sets char's karma
	inline void setKarma(int32_t newkarma)
	{ karma = newkarma; }

	inline void setFame(int32_t newfame)
	{ fame=newfame; }

	inline void setFrozen(bool set = true)
	{ setFlag(flagFrozen, set); }

	inline void setPermaHidden(bool set = true)
	{ setFlag(flagPermaHidden, set); }

	inline void setReflection(bool set = true)
	{ setFlag(flagReflection, set); }

	inline void setPermaGrey(bool set = true)
	{ setFlag(flagGrey|flagPermaGrey, set); }

	inline void setCanUseDoor(bool set = true)
	{ setFlag(flagDoorUse, set); }

	inline void makeInvulnerable(bool set = true)
	{ setFlag(flagInvulnerable, set); }

	inline const bool inWarMode() const
	{ return flags & flagWarMode; }

	inline void setWarMode(bool set = true)
	{ setFlag(flagWarMode, set); }

	inline void toggleWarMode()
	{ flags ^= flagWarMode; warUpdate(); }

	inline void setHolyDamaged(bool set = true)
	{ setFlag(flagHolyDamaged, set); }
	
	inline void setLightDamaged(bool set = true)
	{ setFlag(flagLightDamaged, set); }
	
	inline void setIsMeditating(bool set = true)
	{ setFlag(flagIsMeditating, set); }
	
	void warUpdate();

	/*!
	\brief Sets criminal or grey depending on a server.cfg setting
	\param mode server.cfg setting to test (1 | 2)
	*/
	inline void cChar::setCrimGrey(int mode)
	{
		/*if ( mode == 1 ) setGrey();
		else makeCriminal();*/
	}

	void increaseKarma(int32_t value, pChar pKilled = 0 );
	void modifyFame( int32_t value );

	void unHide();

	/*!
	\brief Update character's flag (reputation)
	\author Unknown - backport by Flameeyes
	\return true if the flag has changed
	*/
	virtual bool updateFlag() = 0;
//@}

//@{
/*!
\name Races
\brief Race related functions and attributes
*/
public:
	int32_t 			race;				//!< Race index
//@}


public:
	void			checkSafeStats();
	virtual void		heartbeat() = 0;

private:
	void			generic_heartbeat();
	void 			do_lsd();

//@{
/*!
\name Appearence
*/
public:
	void showLongName( pChar showToWho, bool showSerials );
//@}


//@{
/*!
\name Combat
*/
private:
	bool checkForCastingLoss(int damage);
protected:
	//! Check for combat timeout
	inline const bool combatTimerOk()
	{ return TIMEOUT(timeout); }

	void			checkPoisoning(pChar pc_def);
	void			doMissedSoundEffect();
	int32_t			combatHitMessage(int32_t damage);
	void			doCombatSoundEffect(int32_t fightskill, pWeapon pw);
	void			undoCombat();

public:
	inline const bool hasAttackedFirst() const
	{ return flags & flagAttackFirst; }

	inline void setAttackFirst(bool set = true)
	{ setFlag(flagAttackFirst, set); }

	void			checkPoisoning();
	void 			fight(pChar pOpponent);
	void			combatHit( pChar pc_def, int32_t nTimeOut = 0 );
	void			doCombat();
	void			combatOnHorse();
	void			combatOnFoot();
	void			playCombatAction();
	virtual uint16_t 	calcAtt() = 0;
	int32_t			calcDef(int32_t x = 0);
	void			setWresMove(int32_t move = 0);
	int32_t			calcResist(DamageType typeofdamage);
	void			toggleCombat();
	int32_t			getCombatSkill();

	/*!
	\author Luxor
	\brief Makes the char casting a spell
	\param spellnumber Spell identifier
	\param dest target location of the spell
	\todo Document parameters
	*/
	inline void castSpell(magic::SpellId spellnumber, TargetLocation& dest, int32_t flags = 0, int32_t param = 0)
	{ magic::castSpell(spellnumber, dest, this, flags, param); }
//@}

//@{
/*!
\name Movement
*/
protected:
	cPath*		path;			//!< current path
	void		walkNextStep();		//!< walk next path step
	uint32_slist	sentObjects;
	int8_t		dir;			//!< &0F=Direction
	uint32_t	LastMoveTime;		//!< server time of last move

public:
	//! has a path set?
	inline const bool hasPath() const
	{ return path; }

	bool		canSee( pObject obj );	//!< can it see the object?
	bool		seeForFirstTime( pObject obj );	//!< does it see the object for the first time?
	bool		seeForLastTime( pObject obj ); //!< does it see the object for the first time?
	void		walk();			//!< execute walk code <Luxor>
	void		follow( pChar pc ); //!< follow pc
	void 		flee( pChar pc, int32_t seconds=INVALID ); //!< flee from pc
	void		pathFind( sLocation pos, bool bOverrideCurrentPath = true );
	uint8_t		getDirFromXY( uint16_t targetX, uint16_t targetY );
//@}

//@{
/*!
\name Mount
*/
public:
	bool unmountHorse();
	void mountHorse(pNPC mount);
	pNPC getHorse();
	void setOnHorse();
	bool isMounting( );
	bool isMounting(pNPC horse);
//@}

//@{
/*!
\name Stats regen rate info
*/
private:
	sRegen regens[ALL_STATS]; //!< stats regen info
public:
	void setRegenRate( StatType stat, uint32_t rate, VarType type );
	uint32_t getRegenRate( StatType stat, VarType type );
	bool regenTimerOk( StatType stat );
	void updateRegenTimer( StatType stat );
//@}

//@{
/*!
\name Skills and doubleclicking
*/
public:
	uint32_t skilldelay;
	uint32_t objectdelay;
public:
        void singleClick(pClient client);	//!< "this" is the clicked char, client is the client of the clicker
        void doubleClick(pClient client);	//!< Doubleclicking a char. Argument is the client of the pg who has doubleclicked on "this"

        /*!
        returns a NotEquippableReason with the either nerEquipOK or a reason for failure if char cannot equip pi
        NOTE:  it will report only the most lacking stat or skill if more than one is below required minimum
        NOTE2: it doesn't check if item already in that layer
	*/
	NotEquippableReason canEquip(pEquippable pi);

        /*!
        returns a NotEquippableReason with the either nerEquipOK or a reason for failure if char cannot equip pi
        NOTE:  it will report only the most lacking stat or skill if more than one is below required minimum
        NOTE2: it doesn't check if item already in that layer
	*/
        NotEquippableReason canEquip(pItem pi);

	inline void setSkillDelay( uint32_t seconds = nSettings::Server::getDelaySkills() )
	{ skilldelay = getclock() + seconds * SECS; }

	inline const bool canDoSkillAction() const
	{ return TIMEOUT( skilldelay ); }

	inline void setObjectDelay( uint32_t seconds = nSettings::Server::getDelayObjects() )
	{ objectdelay = getclock() + seconds * SECS; }

	inline const bool canDoObjectAction() const
	{ return TIMEOUT( objectdelay ); }
	
	uint8_t bestSkill() const;
	uint8_t nextBestSkill(uint8_t previous) const;
	
/*	//! Gets the overridden title 
	inline std::string getTitle() const
	{ return title; }
*/	
	std::string getTitle1() const;

	/*!
	\brief Gets the paperdoll title [2]
	\return A string with the character's title
	*/
	inline std::string getTitle2() const
	{ return std::string(/*title[ bestSkill()+1 ].skill*/ ""); }
	
	std::string getTitle3() const;
	std::string getCompleteTitle() const;

//@}

	/********************************/
	/*     TO REMOVE/REPLACE        */
	/********************************/
#if 0
public:
	inline const bool isOwnerOf(const cObject *obj) const
	{ return this == obj->getOwner(); }
#endif
	/********************************/

	pItem			nameKey;	//!< for renaming keys
	pItem			nameRune;	//!< Used for naming runes
	pItem			nameDeed;

	pChar			target;		//!< Current combat target
	pChar			attacker;	//!< Character who attacked this character
	pChar			followtarget;	//!< NPC Follow Target
	pChar			swingtarget;	//!< Target they are going to hit after they swing

	pAccount		account;

public:
	std::wstring profile; //!< player profile

protected:
	std::wstring* speechCurrent;
public:
	//! Return current speech
	inline const std::wstring* getSpeechCurrent() const
	{ return speechCurrent; }

	//! Set current speech
	inline void setSpeechCurrent( std::wstring* speech )
	{ speechCurrent=speech; }

	//! Reset current speech
	inline void resetSpeechCurrent()
	{ setSpeechCurrent(NULL); }

	void deleteSpeechCurrent();

public:
	uint32_t oldmenu; //!< old menu serial

	int32_t			stat3crc; // xan : future use to keep safe stats
	
	int32_t			hp;  // Hitpoint32_ts
	int32_t			stm; // Stamina
	int32_t			mn;  // Mana
	int32_t			mn2; // Reserved for calculation
	uint16_t		baseskill[ALLSKILLS+1]; // Base skills without stat modifiers
	uint16_t		skill[ALLSKILLS+1]; // List of skills (with stat modifiers)

	uint32_t			robe; // Serial number of generated death robe (If char is a ghost)
	uint8_t			fixedlight; // Fixed lighting level (For chars in dungeons, where they dont see the night)
	char			speech; // For NPCs: Number of the assigned speech block
	uint32_t			att; // Intrinsic attack (For monsters that cant carry weapons)
	uint32_t			def; // Intrinsic defense
	char			wresmove; // Luxor: for Wrestling's special moves

	uint32_t		timeout; // Combat timeout (For hitting)
	uint32_t		timeout2; // memory of last shot timeout

	uint32_t		invistimeout;
	int32_t			hunger;  // Level of hungerness, 6 = full, 0 = "empty"
	uint32_t		hungertime; // Timer used for hunger, one point is dropped every 20 min
	int32_t			callnum; //GM Paging
	int32_t			playercallnum; //GM Paging
	int32_t			pagegm; //GM Paging
	//char region;
	uint8_t			region;

	int32_t			combathitmessage;
	int32_t			making; // skill number of skill using to make item, 0 if not making anything.
	char			blocked;
	char			dir2;
	uint32_t		spiritspeaktimer; // Timer used for duration of spirit speak
	int32_t			spattack;
	uint32_t		spadelay;
	uint32_t		spatimer;
	int32_t			taming; //Skill level required for taming
	uint32_t		summontimer; //Timer for summoned creatures.
	uint32_t		fishingtimer; // Timer used to delay the catching of fish

	//<Luxor>
	int32_t			resists[MAX_RESISTANCE_INDEX];
	DamageType		damagetype;
	//</Luxor>
	int32_t			advobj;		//!< Has used advance gate?

	int32_t			poison;		//!< used for poison skill
	PoisonType		poisoned;	//!< type of poison
	uint32_t		poisontime;	//!< poison damage timer
	uint32_t		poisontxt;	//!< poision text timer
	uint32_t		poisonwearofftime; //!< LB, makes poision wear off ...

	int32_t			fleeat;
	int32_t			reattackat;
	int32_t			trigger;	//!< Trigger number that character activates
	std::string		trigword;	//!< Word that character triggers on.
	uint16_t			envokeid;
	int32_t			envokeitem;
	int32_t			split;
	int32_t			splitchnc;
	int32_t			targtrig;	//!< Stores the number of the trigger the character for targeting
	char			ra;		//!< Reactive Armor spell

	uint32_t		tempflagtime;

	std::wstring* staticProfile;			//!< player static profile


	uint32_t		murderrate; //!< # of ticks until one murder decays //REPSYS
	uint32_t		murdersave; //!< # of second for murder decay

	uint32_t		crimflag; //!< Time when No longer criminal -1=Not Criminal
	uint32_t		spelltime; //Time when they are done casting....
	magic::SpellId		spell; //current spell they are casting....
	int32_t			spellaction; //Action of the current spell....
	magic::CastingType	spelltype;
	uint32_t		nextact; //time to next spell action....
	TargetLocation*		spellTL; //Luxor: npc spell targetlocation

	int32_t			squelched; // zippy  - squelching
	uint32_t		mutetime; //Time till they are UN-Squelched.
	//int32_t statuse[3]; //Morrolan - stat/skill cap STR/INT/DEX in that order
	//int32_t skilluse[skTrueSkills][1]; //Morrolan - stat/skill cap
	uint8_t			lockSkill[ALLSKILLS+1]; // LB, client 1.26.2b skill managment
	int32_t			stealth; //AntiChrist - stealth ( steps already done, -1=not using )
	uint32_t		running; //AntiChrist - Stamina Loose while running
	uint32_t		lastRunning; //Luxor
	int32_t			logout; //Time till logout for this char -1 means in the world or already logged out //Instalog
	//uint32_t swing;

	uint32_t		holdg; // Gold a player vendor is holding for Owner
	char			fly_steps; // number of step the creatures flies if it can fly
	uint32_t		smoketimer; // LB
	uint32_t		smokedisplaytimer;

	uint32_t		antispamtimer;//LB - anti spam

	uint32_t		antiguardstimer;//AntiChrist - anti "GUARDS" spawn

	int32_t			carve; //AntiChrist - for new carve system

	uint32_t		begging_timer;
	PostType		postType;
	QuestType		questType;
	int32_t			questDestRegion;
	int32_t			questOrigRegion;
	int32_t			questBountyReward;		// The current reward amount for the return of this chars head
	uint32_t		questBountyPostSerial;	// The global posting serial number of the bounty message
	uint32_t		questEscortPostSerial;	// The global posting serial number of the escort message
	uint32_t		murdererSer;			// Serial number of last person that murdered this char

	// COORDINATE	previousLocation;

	uint32_t 		time_unused;
	uint32_t 		timeused_last;



private:
	inline void resetBaseSkill()
	{ memset(baseskill, 0, sizeof(baseskill)); }

	inline void resetSkill()
	{ memset(skill, 0, sizeof(skill)); }

	inline void resetFlags()
	{ flags = 0; }

	inline void resetResists()
	{ memset(resists, 0, sizeof(resists)); }

	inline void resetLockSkills()
	{ memset(lockSkill, 0, sizeof(lockSkill)); }

public:
	//! tells if a character is running
	inline const bool isRunning() const
	{ return ( (getclock() - lastRunning) <= 100 ); }

	inline void setRunning()
	{ lastRunning = getclock(); }

	void 			updateStats(int32_t stat);

	void 			setNextMoveTime(short tamediv=1);
	void 			disturbMed();

	void                    drink(pItem pi);       //Luxor: delayed drinking
	void 			hideBySkill();
	void 			hideBySpell(int32_t timer = INVALID);
	uint32_t  		countItems(uint16_t ID, uint16_t col= 0xFFFF);

	inline const uint32_t CountGold()
	{ return countItems(ITEMID_GOLD); }

	bool			isInBackpack( pItem pi );
	void			addGold(uint16_t totgold);

	// The bit for setting what effect gm movement
	// commands shows
	// 0 = off
	// 1 = FlameStrike
	// 2-6 = Sparkles
	int32_t			gmMoveEff;

	uint32_t		getSkillSum();
	int32_t			getTeachingDelta(pChar pPlayer, int32_t skill, int32_t sum);
	void			removeItemBonus(cItem* pi);
	inline const bool	isSameAs(pChar pc) const
	{ return this == pc; }

	//! Return the resistance for a defined type
	inline const bool resist(uint32_t n) const
	{ return flags & n; }

	void			sysmsg(const char *txt, ...);

	void                    attackStuff (pChar victim);
	void			helpStuff(pChar pc_i);
	void			applyPoison(PoisonType poisontype, int32_t secs = INVALID);
	void			setOwner(pChar owner);
	void			curePoison();
	void			resurrect(pClient healer = NULL);
	void			unfreeze( bool calledByTempfx = false );
	void			damage(int32_t amount, DamageType typeofdamage = damPure, StatType stattobedamaged = STAT_HP);
	void			playAction(uint16_t action);
	void			impAction(uint16_t action);

//@{
/*!
\name Talk and Emote stuff
*/
protected:
	uint16_t			emotecolor;		//!< Color for emote messages
	uint8_t			fonttype;		//!< Speech font to use
	uint16_t			saycolor;		//!< Color for say messages
public:
	void			talkAll(char *txt, bool antispam = 1);
	void			talk(pClient client, char *txt, bool antispam = 1);
	void			emote(pClient client,char *txt, bool antispam, ...);
	void			emoteall(char *txt, bool antispam, ...);
	void			talkRunic(pClient client, char *txt, bool antispam = 1);
	void			talkAllRunic(char *txt, bool antispam = 0);
//@}

	uint16_t		distFrom(pChar pc);
	uint16_t		distFrom(pItem pi);
	
	/*!
	\brief Check if a specified char is in range from this char
	\param pc Char to check if in range
	\param range Maximum distance from this char
	*/
	inline const bool hasInRange(pChar pc, uint16_t range = VISRANGE)
	{ return pc && distFrom( pc ) <= range; }
	
	/*!
	\brief Check if a specified char is in range from this char
	\param pi Char to check if in range
	\param range Maximum distance from this char
	*/
	inline const bool hasInRange(pItem pi, uint16_t range = VISRANGE)
	{ return pi && distFrom( pi ) <= range; }
	
	void			teleport( uint8_t flags = TELEFLAG_SENDALL, pClient cli = NULL );
	void			facexy(uint16_t facex, uint16_t facey);

	/*!
	\author Luxor
	\brief Returns line of sight from the char to the give char
	\param pc pointer to the char to check line of sight from
	\return true if is in line of sight
	*/
	inline const bool losFrom(const pChar pc) const
	{ return pc && pc->getBody() ? lineOfSight( body->getPosition(), pc->getBody()->getPosition() ) : false; }

	bool			checkSkill(Skill sk, int32_t low, int32_t high, bool bRaise = true);
	bool			checkSkillSparrCheck(Skill sk, int32_t low, int32_t high, pChar defend);

	/*!
	\author Xanathar
	\brief Deletes items from backpack, by id
	\param id id of the item to delete
	\param amount amount of item to delete
	\param color color of item to delete
	\return number of items deleted
	\todo Implement cEquippableContainer
	*/
	inline const uint32_t delItems(uint16_t id, uint32_t amount = 1, uint16_t color = 0xFFFF)
	{ return body->getBackpack() ? /*body->getBackpack()->removeItems(amount, id, color)*/ amount : amount; }


	/*!
	\brief Get the amount of the given id, color
	\author Flameeyes
	\return amount of items counted
	\param id the id
	\param col the color ( 0xFFFF for all colors )
	\param onlyPrimaryBackpack false if search also in th subpack
	*/
	inline const uint32_t getAmount(uint16_t id, uint16_t col=0xFFFF, bool onlyPrimaryBackpack=false )
	{ return body->getBackpack() ? /*body->getBackpack()->countItems(id, col, !onlyPrimaryBackpack)*/ 0 : 0; }

	void			useHairDye(pItem bottle);

	void			morph ( short bodyid = INVALID, short skincolor = INVALID,
							short hairstyle = INVALID, short haircolor = INVALID, short beardstyle = INVALID,
							short beardcolor = INVALID, const char* newname = NULL, bool bBackup = true);


	void		Kill();
	void		goPlace(int32_t);
	bool		knowsSpell(magic::SpellId spellnumber);

	public:
	#ifdef SPAR_NEW_WR_SYSTEM
		NxwCharWrapper*		nearbyChars;
		NxwItemWrapper*		nearbyItems;
	#endif

	uint32_vector lootVector;
	virtual void Delete();
//@{
/*!
\name Sound and Visual effects
*/

	void playSFX(int16_t sound, bool onlyToMe = false);
	void playMonsterSound(MonsterSound sfx);
//@}
};

#endif
