/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
 \file chars.h
 \brief Declaration of cChar class
 */
#ifndef __CHARS_H
#define __CHARS_H

#include "common_libs.h"

class cChar;
typedef cChar *pChar;			//!< Pointer to a Char
typedef std::list<pChar> CharList;	//!< List of pointers to Char

#include "ai.h"
#include "objects/cobject.h"
#include "magic.h"

#include "npcs.h"
#include "msgboard.h"
#include "target.h"
#include "constants.h"
#include "menu.h"
#include "logsystem.h"
#include "globals.h"

#include "basics.h"
#include "items.h"
#include "cmds.h"

#ifndef TIMEOUT
#define TIMEOUT(X) (((X) <= uiCurrentTime) || overflow)
#endif

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
\author Akron
*/
enum AmxCharEvents
{
	EVENT_CHR_ONDEATH 		=  0,
	EVENT_CHR_ONBEFOREDEATH		=  0,
	EVENT_CHR_ONWOUNDED		=  1,
	EVENT_CHR_ONHIT			=  2,
	EVENT_CHR_ONHITMISS		=  3,
	EVENT_CHR_ONGETHIT		=  4,
	EVENT_CHR_ONREPUTATIONCHG	=  5,
	EVENT_CHR_ONDISPEL		=  6,
	EVENT_CHR_ONRESURRECT		=  7,
	EVENT_CHR_ONFLAGCHG		=  8,
	EVENT_CHR_ONWALK		=  9,
	EVENT_CHR_ONADVANCESKILL	= 10,
	EVENT_CHR_ONADVANCESTAT		= 11,
	EVENT_CHR_ONBEGINATTACK		= 12,
	EVENT_CHR_ONBEGINDEFENSE	= 13,
	EVENT_CHR_ONTRANSFER		= 14,
	EVENT_CHR_ONMULTIENTER		= 15,
	EVENT_CHR_ONMULTILEAVE		= 16,
	EVENT_CHR_ONSNOOPED		= 17,
	EVENT_CHR_ONSTOLEN		= 18,
	EVENT_CHR_ONPOISONED		= 19,
	EVENT_CHR_ONREGIONCHANGE	= 20,
	EVENT_CHR_ONCASTSPELL		= 21,
	EVENT_CHR_ONGETSKILLCAP		= 22,
	EVENT_CHR_ONGETSTATCAP		= 23,
	EVENT_CHR_ONBLOCK		= 24,
	EVENT_CHR_ONSTART		= 25,
	EVENT_CHR_ONHEARTBEAT		= 26,
	EVENT_CHR_ONBREAKMEDITATION	= 27,
	EVENT_CHR_ONCLICK		= 28,
	EVENT_CHR_ONMOUNT		= 29,
	EVENT_CHR_ONDISMOUNT		= 30,
	EVENT_CHR_ONKILL		= 31,
	EVENT_CHR_ONHEARPLAYER		= 32,
	EVENT_CHR_ONDOCOMBAT		= 33,
	EVENT_CHR_ONCOMBATHIT		= 34,
	EVENT_CHR_ONSPEECH		= 35,
	EVENT_CHR_ONCHECKNPCAI		= 36,
	EVENT_CHR_ONDIED		= 37,
	EVENT_CHR_ONAFTERDEATH		= 37,
	ALLCHAREVENTS			= 38
};

enum DispelType
{
	DISPELTYPE_UNKNOWN		= 0,
	DISPELTYPE_DISPEL,
	DISPELTYPE_TIMEOUT,
	DISPELTYPE_GMREMOVE
};

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
\brief Character class
*/
class cChar : public cObject
{
public:
	static uint32_t nextSerial();

	cChar( uint32_t ser );
	~cChar();

	static void	archive();
	static void	safeoldsave();
	void		getPopupHelp(char *str)
	void		MoveTo(Location newloc);
	void 		loadEventFromScript(char *script1, TEXT *script2);
	void		doGmEffect();

protected:
	pClient client;
        pBody body;     //! The body the character is currently "using"
        pBody oldbody;  //! Old body. To use in polimorph-type effects

public:
	inline pClient getClient() const
	{ return client; }

	inline void setClient(pClient c)
	{ client = c; }
        
        inline pBody getBody() const
	{ return body; }

	inline void setBody(pBody b)
	{ body = b; }

        inline pBody getOldBody() const
	{ return oldbody; }

	inline void setOldBody(pBody b)
	{ oldbody = b; }

	//! get online status
	inline const bool isOnline() const
	{ return client; }

//@{
/*!
\name char_flags
\brief Flags used for the char
*/
public:
	static const uint64_t flagGrey		= 0x0000000000000001ull; //!< Char is grey
	static const uint64_t flagPermaGrey		= 0x0000000000000002ull; //!< Char is permanent grey
	static const uint64_t flagResistParalisys	= 0x0000000000000004ull; //!< Char resists to paralisys (unused)
	static const uint64_t flagWarMode		= 0x0000000000000008ull; //!< Char is in war mode
	static const uint64_t flagSpellTelekinesys	= 0x0000000000000010ull; //!< Char under telekinesys spell (Luxor)
	static const uint64_t flagSpellProtection	= 0x0000000000000020ull; //!< Char under protection spell (Luxor)

	static const uint64_t flagKarmaInnocent	= 0x0000000000000040ull; //!< Char is innocent
	static const uint64_t flagKarmaMurderer	= 0x0000000000000080ull; //!< Char is murderer
	static const uint64_t flagKarmaCriminal	= 0x0000000000000100ull; //!< Char is criminal

	static const uint64_t flagInvulnerable	= 0x0000000000000200ull; //!< Char is invulnerable
	static const uint64_t flagNoSkillTitle	= 0x0000000000000400ull; //!< Char hasn't skill title

	static const uint64_t flagFrozen		= 0x0000000000000800ull;
	static const uint64_t flagPermaHidden	= 0x0000000000001000ull;
	static const uint64_t flagNoUseMana		= 0x0000000000002000ull;
	static const uint64_t flagReflection	= 0x0000000000004000ull;
	static const uint64_t flagNoUseReagents	= 0x0000000000008000ull;

	static const uint64_t flagIncognito		= 0x0000000000010000ull;
	static const uint64_t flagPolymorphed	= 0x0000000000020000ull;
	static const uint64_t flagDead		= 0x0000000000040000ull;

	static const uint64_t flagAttackFirst	= 0x0000000000080000ull;
	static const uint64_t flagDoorUse		= 0x0000000000100000ull;

	static const uint64_t flagIsCasting		= 0x0000000000200000ull;
	static const uint64_t flagIsGuarded		= 0x0000000000400000ull;
//@}

//@}
/*!
\name Char Status
*/
protected:
	uint64_t flags;	//!< Flags for the character

	int32_t karma;	//!< karma of the char
	int32_t fame;	//!< fame of the char

	uint16_t kills;	//!< PvP Kills
	uint16_t deaths;	//!< Number of deaths
	float  fstm;	//!< Unavowed - stamina to remove the next step

	inline void setFlag(uint64_t flag, bool set)
	{
		if ( set ) flags |= flag;
		else flags &= ~flag;
	}

public:
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

	inline const bool isHidden() const
	{ return (hidden != UNHIDDEN); }

	inline const bool isHiddenBySpell() const
	{ return (hidden & HIDDEN_BYSPELL); }

	inline const bool isHiddenBySkill() const
	{ return (hidden & HIDDEN_BYSKILL); }

	inline const bool canUseDoor() const
	{ return flags & flagDoorUse; }

	inline const bool dontUseMana() const
	{ return flags & flagNoUseMana; }

	inline const bool dontUseReagents() const
	{ return flags & flagNoUseReagents; }

	inline const bool hasReflection() const
	{ return flags & flagReflection; }

        inline const bool hasTelekinesis() const
        { return flags & flagSpellTelekinesys }

	inline const bool inGuardedArea() const
	{ return ::region[region].priv & RGNPRIV_GUARDED; }



	const bool isGrey() const;
	/*!
	\author Xanathar
	\brief Checks char weight
	\note this function modify the class variable, very bad...
	\return true if the char is over weight
	*/
	inline const bool cChar::isOverWeight()
	{ return !isGM() && getBody()->overloadedTeleport(); }
	
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

	void warUpdate();



	/*!
	\brief Sets criminal or grey depending on a server.cfg setting
	\param mode server.cfg setting to test (1 | 2)
	*/
	inline void cChar::setCrimGrey(int mode)
	{
		if ( mode == 1 ) SetGrey();
		else makeCriminal();
	}

	void setMurderer();
	void setInnocent();
	void setCriminal();
	void makeCriminal();
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
protected:
	//! Check for combat timeout
	inline const bool combatTimerOk()
	{ return TIMEOUT(timeout); }

	void			checkPoisoning(pChar pc_def);
	void			doMissedSoundEffect();
	int32_t			combatHitMessage(int32_t damage);
	void			doCombatSoundEffect(int32_t fightskill, pItem pWeapon);
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
	int32_t			calcAtt();
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
	uint32_t_SLIST	sentObjects;
	int8_t		dir;			//!< &0F=Direction
	uint32_t		LastMoveTime;		//!< server time of last move

public:
	//! has a path set?
	inline const bool hasPath() const
	{ return path; }

	bool		canSee( cObject &obj );	//!< can it see the object?
	bool		seeForFirstTime( cObject &obj );	//!< does it see the object for the first time?
	bool		seeForLastTime( cObject &obj ); //!< does it see the object for the first time?
	void		walk();			//!< execute walk code <Luxor>
	void		follow( pChar pc ); //!< follow pc
	void 		flee( pChar pc, int32_t seconds=INVALID ); //!< flee from pc
	void		pathFind( Location pos, bool bOverrideCurrentPath = true );
	uint8_t		getDirFromXY( uint16_t targetX, uint16_t targetY );
//@}

//@{
/*!
\name Mount
*/
public:
	int32_t			unmountHorse();
	void			mounthorse( pChar mount );
	pChar			getHorse();
	void			setOnHorse();
	bool			isMounting( );
	bool			isMounting( pChar horse );
//@}

//@{
/*!
\name Guilds
*/
	private:
		P_GUILD guild; //!< the guild
		P_GUILD_MEMBER member; //!< the guild member info

	public:
		bool	isGuilded();
		void	setGuild( P_GUILD guild, P_GUILD_MEMBER member );
		P_GUILD	getGuild();
		P_GUILD_MEMBER getGuildMember();
//@}


//@{
/*!
\name Stats regen rate info
*/
private:
	regen_st 	regens[ALL_STATS]; //!< stats regen info
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
	TIMERVAL skilldelay;
	TIMERVAL objectdelay;
public:
        void singleClick(pClient client);  // "this" is the clicked char, client is the client of the clicker
        void doubleClick(pClient client, int keyboard);       //!< Doubleclicking a char. Argument is the client of the pg who has doubleclicked on "this"
	inline void setSkillDelay( uint32_t seconds = server_data.skilldelay )
	{ skilldelay = uiCurrentTime + seconds * MY_CLOCKS_PER_SEC; }

	inline const bool canDoSkillAction() const
	{ return TIMEOUT( skilldelay ); }

	inline void setObjectDelay( uint32_t seconds = server_data.objectdelay )
	{ objectdelay = uiCurrentTime + seconds * MY_CLOCKS_PER_SEC; }

	inline const bool canDoObjectAction() const
	{ return TIMEOUT( objectdelay ); }

//@}

	/********************************/
	/*     TO REMOVE/REPLACE        */
	/********************************/
	public:
		void 			setMultiSerial(long mulser);

		inline const bool isOwnerOf(const cObject *obj) const
		{ return this == obj->getOwner(); }

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
		wstring profile; //!< player profile

	private:
		wstring* speechCurrent;
	public:
		//! Return current speech
		inline const wstring* getSpeechCurrent() const
		{ return speechCurrent; }

		//! Set current speech
		inline void setSpeechCurrent( wstring* speech )
		{ speechCurrent=speech; }

		//! Reset current speech
		inline void resetSpeechCurrent()
		{ setSpeechCurrent(NULL); }

		void deleteSpeechCurrent();

	public:
		uint32_t oldmenu; //!< old menu serial

		int32_t			stat3crc; // xan : future use to keep safe stats
		class			AmxEvent *amxevents[ALLCHAREVENTS];
		int32_t			hp;  // Hitpoint32_ts
		int32_t			stm; // Stamina
		int32_t			mn;  // Mana
		int32_t			mn2; // Reserved for calculation
		uint16_t			baseskill[ALLSKILLS+1]; // Base skills without stat modifiers
		uint16_t			skill[ALLSKILLS+1]; // List of skills (with stat modifiers)


		uint32_t			robe; // Serial number of generated death robe (If char is a ghost)
		uint8_t			fixedlight; // Fixed lighting level (For chars in dungeons, where they dont see the night)
		char			speech; // For NPCs: Number of the assigned speech block
		uint32_t			att; // Intrinsic attack (For monsters that cant carry weapons)
		uint32_t			def; // Intrinsic defense
		char			wresmove; // Luxor: for Wrestling's special moves

		TIMERVAL		timeout; // Combat timeout (For hitting)
		TIMERVAL		timeout2; // memory of last shot timeout

		uint8_t			hidden; // 0 = not hidden, 1 = hidden, 2 = invisible spell
		TIMERVAL		invistimeout;
		int32_t			hunger;  // Level of hungerness, 6 = full, 0 = "empty"
		TIMERVAL		hungertime; // Timer used for hunger, one point is dropped every 20 min
		int32_t			callnum; //GM Paging
		int32_t			playercallnum; //GM Paging
		int32_t			pagegm; //GM Paging
		//char region;
		uint8_t			region;

		int32_t			combathitmessage;
		int32_t			making; // skill number of skill using to make item, 0 if not making anything.
		char			blocked;
		char			dir2;
		TIMERVAL		spiritspeaktimer; // Timer used for duration of spirit speak
		int32_t			spattack;
		TIMERVAL		spadelay;
		TIMERVAL		spatimer;
		int32_t			taming; //Skill level required for taming
		TIMERVAL		summontimer; //Timer for summoned creatures.
		TIMERVAL		fishingtimer; // Timer used to delay the catching of fish

		//<Luxor>
		int32_t			resists[MAX_RESISTANCE_INDEX];
		bool			holydamaged;
		bool			lightdamaged;
		DamageType		damagetype;
		//</Luxor>
		int32_t			advobj;		//!< Has used advance gate?

		int32_t			poison;		//!< used for poison skill
		PoisonType		poisoned;	//!< type of poison
		TIMERVAL		poisontime;	//!< poison damage timer
		TIMERVAL		poisontxt;	//!< poision text timer
		TIMERVAL		poisonwearofftime; //!< LB, makes poision wear off ...

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

		TIMERVAL		tempflagtime;

		wstring* staticProfile;			//!< player static profile


		TIMERVAL		murderrate; //!< # of ticks until one murder decays //REPSYS
		TIMERVAL		murdersave; //!< # of second for murder decay

		TIMERVAL		crimflag; //!< Time when No longer criminal -1=Not Criminal
		TIMERVAL		spelltime; //Time when they are done casting....
		magic::SpellId		spell; //current spell they are casting....
		int32_t			spellaction; //Action of the current spell....
		magic::CastingType	spelltype;
		TIMERVAL		nextact; //time to next spell action....
		TargetLocation*		spellTL; //Luxor: npc spell targetlocation

		int32_t			squelched; // zippy  - squelching
		TIMERVAL		mutetime; //Time till they are UN-Squelched.
		int32_t			med; // 0=not meditating, 1=meditating //Morrolan - Meditation
		//int32_t statuse[3]; //Morrolan - stat/skill cap STR/INT/DEX in that order
		//int32_t skilluse[TRUESKILLS][1]; //Morrolan - stat/skill cap
		uint8_t			lockSkill[ALLSKILLS+1]; // LB, client 1.26.2b skill managment
		int32_t			stealth; //AntiChrist - stealth ( steps already done, -1=not using )
		uint32_t			running; //AntiChrist - Stamina Loose while running
		uint32_t			lastRunning; //Luxor
		int32_t			logout; //Time till logout for this char -1 means in the world or already logged out //Instalog
		//uint32_t swing;

		uint32_t			holdg; // Gold a player vendor is holding for Owner
		char			fly_steps; // number of step the creatures flies if it can fly
		TIMERVAL		smoketimer; // LB
		TIMERVAL		smokedisplaytimer;

		TIMERVAL		antispamtimer;//LB - anti spam

		TIMERVAL		antiguardstimer;//AntiChrist - anti "GUARDS" spawn

		int32_t			carve; //AntiChrist - for new carve system

		TIMERVAL		begging_timer;
		cMsgBoard::PostType	postType;
		cMsgBoard::QuestType	questType;
		int32_t			questDestRegion;
		int32_t			questOrigRegion;
		int32_t			questBountyReward;		// The current reward amount for the return of this chars head
		uint32_t			questBountyPostSerial;	// The global posting serial number of the bounty message
                uint32_t			questEscortPostSerial;	// The global posting serial number of the escort message
		uint32_t			murdererSer;			// Serial number of last person that murdered this char

		// COORDINATE	previousLocation;

		TIMERVAL 		time_unused;
		TIMERVAL 		timeused_last;



	private:
		inline void resetBaseSkill()
		{ memset(baseskill, 0, sizeof(baseskill)); }

		inline void resetSkill()
		{ memset(skill, 0, sizeof(skill)); }

		inline void resetFlags()
		{ flags = 0; }

		inline void resetAmxEvents()
		{ memset(amxevents, 0, sizeof(amxevents)); }

		inline void resetResists()
		{ memset(resists, 0, sizeof(resists)); }

		inline void resetLockSkills()
		{ memset(lockSkill, 0, sizeof(lockSkill)); }

	public:
		//! tells if a character is running
		inline const bool isRunning() const
		{ return ( (uiCurrentTime - lastRunning) <= 100 ); }

		inline void setRunning()
		{ lastRunning = uiCurrentTime; }

		void 			updateStats(int32_t stat);

		void 			setNextMoveTime(short tamediv=1);
		void 			disturbMed();

		void                    drink(pItem pi);       //Luxor: delayed drinking
		void 			hideBySkill();
		void 			hideBySpell(int32_t timer = INVALID);
		uint32_t  			countItems(uint16_t ID, uint16_t col= 0xFFFF);

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

		uint32_t			getSkillSum();
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
		void			resurrect(NXWCLIENT healer = NULL);
		void			unfreeze( bool calledByTempfx = false );
		void			damage(int32_t amount, DamageType typeofdamage = DAMAGE_PURE, StatType stattobedamaged = STAT_HP);
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
	void			talk(NXWSOCKET s, char *txt, bool antispam = 1);
	void			emote(NXWSOCKET s,char *txt, bool antispam, ...);
	void			emoteall(char *txt, bool antispam, ...);
	void			talkRunic(NXWSOCKET s, char *txt, bool antispam = 1);
	void			talkAllRunic(char *txt, bool antispam = 0);
//@}

	uint16_t			distFrom(pChar pc);
	uint16_t			distFrom(pItem pi);
	
	/*!
	\brief Check if a specified char is in range from this char
	\param pc Char to check if in range
	\param range Maximum distance from this char
	*/
	inline const bool       hasInRange(pChar pc, uint16_t range = VISRANGE)
	{ return pc && distFrom( pc ) <= range; }
	
	/*!
	\brief Check if a specified char is in range from this char
	\param pc Char to check if in range
	\param range Maximum distance from this char
	*/
	inline const bool       hasInRange(pItem pc, uint16_t range = VISRANGE)
	{ return pi && distFrom( pi ) <= range; }
	
	void			teleport( uint8_t flags = TELEFLAG_SENDALL, NXWCLIENT cli = NULL );
	void			facexy(uint16_t facex, uint16_t facey);

	/*!
	\author Luxor
	\brief Returns line of sight from the char to the give char
	\param pc pointer to the char to check line of sight from
	\return true if is in line of sight
	*/
	inline const bool losFrom(const pChar pc) const
	{ return pc ? lineOfSight( getPosition(), pc->getPosition() ) : false; }

	void			playSFX(int16_t sound, bool onlyToMe = false);
	void			playMonsterSound(MonsterSound sfx);

	bool			checkSkill(Skill sk, int32_t low, int32_t high, bool bRaise = true);

	/*!
	\author Xanathar
	\brief Deletes items from backpack, by id
	\param id id of the item to delete
	\param amount amount of item to delete
	\param color color of item to delete
	\return number of items deleted
	*/
	inline const uint32_t delItems(uint16_t id, uint32_t amount = 1, uint16_t color = 0xFFFF)
	{ return body->getBackpack() ? body->getBackpack()->removeItems(amount,id, color) : amount; }

	const bool	checkSkillSparrCheck(Skill sk, int32_t low, int32_t high, pChar pcd);

	/*!
	\brief Get the amount of the given id, color
	\author Flameeyes
	\return amount of items counted
	\param id the id
	\param col the color ( 0xFFFF for all colors )
	\param onlyPrimaryBackpack false if search also in th subpack
	*/
	inline const bool uint32_t getAmount(uint16_t id, uint16_t col=0xFFFF, bool onlyPrimaryBackpack=false )
	{ return body->getBackpack() ? body->getBackpack()->countItems(id, col, !onlyPrimaryBackpack); }

	void			movingFX(pChar destination, short id, int32_t speed, int32_t loop, bool explode, class ParticleFx* part = NULL);
	void			staticFX(short id, int32_t speed, int32_t loop, class ParticleFx* part = NULL);
	void			boltFX(bool bNoParticles);
	void			circleFX(short id);

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

		std::vector< uint32_t >	lootVector;
		virtual void		Delete();
} PACK_NEEDED;

#endif
