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

class cChar;
typedef cChar *pChar;

#include "ai.h"
#include "object.h"
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
	static UI32 nextSerial();

	cChar( SERIAL ser );
	~cChar();

	static void	archive();
	static void	safeoldsave();
	void		getPopupHelp(char *str)
	void		MoveTo(Location newloc);
	void 		loadEventFromScript(TEXT *script1, TEXT *script2);
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
	static const UI64 flagGrey		= 0x0000000000000001ull; //!< Char is grey
	static const UI64 flagPermaGrey		= 0x0000000000000002ull; //!< Char is permanent grey
	static const UI64 flagResistParalisys	= 0x0000000000000004ull; //!< Char resists to paralisys (unused)
	static const UI64 flagWarMode		= 0x0000000000000008ull; //!< Char is in war mode
	static const UI64 flagSpellTelekinesys	= 0x0000000000000010ull; //!< Char under telekinesys spell (Luxor)
	static const UI64 flagSpellProtection	= 0x0000000000000020ull; //!< Char under protection spell (Luxor)

	static const UI64 flagKarmaInnocent	= 0x0000000000000040ull; //!< Char is innocent
	static const UI64 flagKarmaMurderer	= 0x0000000000000080ull; //!< Char is murderer
	static const UI64 flagKarmaCriminal	= 0x0000000000000100ull; //!< Char is criminal

	static const UI64 flagInvulnerable	= 0x0000000000000200ull; //!< Char is invulnerable
	static const UI64 flagNoSkillTitle	= 0x0000000000000400ull; //!< Char hasn't skill title

	static const UI64 flagFrozen		= 0x0000000000000800ull;
	static const UI64 flagPermaHidden	= 0x0000000000001000ull;
	static const UI64 flagNoUseMana		= 0x0000000000002000ull;
	static const UI64 flagReflection	= 0x0000000000004000ull;
	static const UI64 flagNoUseReagents	= 0x0000000000008000ull;

	static const UI64 flagIncognito		= 0x0000000000010000ull;
	static const UI64 flagPolymorphed	= 0x0000000000020000ull;
	static const UI64 flagDead		= 0x0000000000040000ull;

	static const UI64 flagAttackFirst	= 0x0000000000080000ull;
	static const UI64 flagDoorUse		= 0x0000000000100000ull;

	static const UI64 flagIsCasting		= 0x0000000000200000ull;
	static const UI64 flagIsGuarded		= 0x0000000000400000ull;
//@}

//@}
/*!
\name Char Status
*/
protected:
	UI64 flags;	//!< Flags for the character

	SI32 karma;	//!< karma of the char
	SI32 fame;	//!< fame of the char

	UI16 kills;	//!< PvP Kills
	UI16 deaths;	//!< Number of deaths
	R32  fstm;	//!< Unavowed - stamina to remove the next step

	inline void setFlag(UI64 flag, bool set)
	{
		if ( set ) flags |= flag;
		else flags &= ~flag;
	}

public:
	//! Return the karma of the char
	inline const SI32 getKarma() const
	{ return karma; }

	//! Return the fame of the char
	inline const SI32 getFame() const
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
	const bool isOverWeight();
	const bool canDoGestures() const;
	const bool inDungeon() const;


	//! Sets char's karma
	inline void setKarma(SI32 newkarma)
	{ karma = newkarma; }

	inline void setFame(SI32 newfame)
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
	void increaseKarma(SI32 value, P_CHAR pKilled = 0 );
	void modifyFame( SI32 value );

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
	SI32 			race;				//!< Race index
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
	void showLongName( P_CHAR showToWho, bool showSerials );

//@}


//@{
/*!
\name Combat
*/
protected:
	//! Check for combat timeout
	inline const bool combatTimerOk()
	{ return TIMEOUT(timeout); }

	void			checkPoisoning(P_CHAR pc_def);
	void			doMissedSoundEffect();
	SI32			combatHitMessage(SI32 damage);
	void			doCombatSoundEffect(SI32 fightskill, P_ITEM pWeapon);
	void			undoCombat();

public:
	inline const bool hasAttackedFirst() const
	{ return flags & flagAttackFirst; }

	inline void setAttackFirst(bool set = true)
	{ setFlag(flagAttackFirst, set); }

	void			checkPoisoning();
	void 			fight(P_CHAR pOpponent);
	void			combatHit( P_CHAR pc_def, SI32 nTimeOut = 0 );
	void			doCombat();
	void			combatOnHorse();
	void			combatOnFoot();
	void			playCombatAction();
	SI32			calcAtt();
	SI32			calcDef(SI32 x = 0);
	void			setWresMove(SI32 move = 0);
	SI32			calcResist(DamageType typeofdamage);
	void			toggleCombat();
	SI32			getCombatSkill();

	/*!
	\author Luxor
	\brief Makes the char casting a spell
	\param spellnumber Spell identifier
	\param dest target location of the spell
	\todo Document parameters
	*/
	inline void castSpell(magic::SpellId spellnumber, TargetLocation& dest, SI32 flags = 0, SI32 param = 0)
	{ magic::castSpell(spellnumber, dest, this, flags, param); }
//@}

//@{
/*!
\name Movement
*/
protected:
	cPath*		path;			//!< current path
	void		walkNextStep();		//!< walk next path step
	SERIAL_SLIST	sentObjects;
	SI08		dir;			//!< &0F=Direction
	UI32		LastMoveTime;		//!< server time of last move

public:
	//! has a path set?
	inline const bool hasPath() const
	{ return path; }

	bool		canSee( cObject &obj );	//!< can it see the object?
	bool		seeForFirstTime( cObject &obj );	//!< does it see the object for the first time?
	bool		seeForLastTime( cObject &obj ); //!< does it see the object for the first time?
	void		walk();			//!< execute walk code <Luxor>
	void		follow( P_CHAR pc ); //!< follow pc
	void 		flee( P_CHAR pc, SI32 seconds=INVALID ); //!< flee from pc
	void		pathFind( Location pos, bool bOverrideCurrentPath = true );
	UI08		getDirFromXY( UI16 targetX, UI16 targetY );
//@}

//@{
/*!
\name Mount
*/
public:
	SI32			unmountHorse();
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
	void setRegenRate( StatType stat, UI32 rate, VarType type );
	UI32 getRegenRate( StatType stat, VarType type );
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
	inline void setSkillDelay( UI32 seconds = server_data.skilldelay )
	{ skilldelay = uiCurrentTime + seconds * MY_CLOCKS_PER_SEC; }

	inline const bool canDoSkillAction() const
	{ return TIMEOUT( skilldelay ); }

	inline void setObjectDelay( UI32 seconds = server_data.objectdelay )
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
		SERIAL oldmenu; //!< old menu serial

		SI32			stat3crc; // xan : future use to keep safe stats
		class			AmxEvent *amxevents[ALLCHAREVENTS];
		SI32			hp;  // HitpoSI32s
		SI32			stm; // Stamina
		SI32			mn;  // Mana
		SI32			mn2; // Reserved for calculation
		UI16			baseskill[ALLSKILLS+1]; // Base skills without stat modifiers
		UI16			skill[ALLSKILLS+1]; // List of skills (with stat modifiers)


		SERIAL			robe; // Serial number of generated death robe (If char is a ghost)
		UI08			fixedlight; // Fixed lighting level (For chars in dungeons, where they dont see the night)
		char			speech; // For NPCs: Number of the assigned speech block
		UI32			att; // Intrinsic attack (For monsters that cant carry weapons)
		UI32			def; // Intrinsic defense
		char			wresmove; // Luxor: for Wrestling's special moves

		TIMERVAL		timeout; // Combat timeout (For hitting)
		TIMERVAL		timeout2; // memory of last shot timeout

		UI08			hidden; // 0 = not hidden, 1 = hidden, 2 = invisible spell
		TIMERVAL		invistimeout;
		SI32			hunger;  // Level of hungerness, 6 = full, 0 = "empty"
		TIMERVAL		hungertime; // Timer used for hunger, one point is dropped every 20 min
		SI32			callnum; //GM Paging
		SI32			playercallnum; //GM Paging
		SI32			pagegm; //GM Paging
		//char region;
		UI08			region;

		SI32			combathitmessage;
		SI32			making; // skill number of skill using to make item, 0 if not making anything.
		char			blocked;
		char			dir2;
		TIMERVAL		spiritspeaktimer; // Timer used for duration of spirit speak
		SI32			spattack;
		TIMERVAL		spadelay;
		TIMERVAL		spatimer;
		SI32			taming; //Skill level required for taming
		TIMERVAL		summontimer; //Timer for summoned creatures.
		TIMERVAL		fishingtimer; // Timer used to delay the catching of fish

		//<Luxor>
		SI32			resists[MAX_RESISTANCE_INDEX];
		bool			holydamaged;
		bool			lightdamaged;
		DamageType		damagetype;
		//</Luxor>
		SI32			advobj;		//!< Has used advance gate?

		SI32			poison;		//!< used for poison skill
		PoisonType		poisoned;	//!< type of poison
		TIMERVAL		poisontime;	//!< poison damage timer
		TIMERVAL		poisontxt;	//!< poision text timer
		TIMERVAL		poisonwearofftime; //!< LB, makes poision wear off ...

		SI32			fleeat;
		SI32			reattackat;
		SI32			trigger;	//!< Trigger number that character activates
		std::string		trigword;	//!< Word that character triggers on.
		UI16			envokeid;
		SI32			envokeitem;
		SI32			split;
		SI32			splitchnc;
		SI32			targtrig;	//!< Stores the number of the trigger the character for targeting
		char			ra;		//!< Reactive Armor spell

		TIMERVAL		tempflagtime;

		wstring* staticProfile;			//!< player static profile


		TIMERVAL		murderrate; //!< # of ticks until one murder decays //REPSYS
		TIMERVAL		murdersave; //!< # of second for murder decay

		TIMERVAL		crimflag; //!< Time when No longer criminal -1=Not Criminal
		TIMERVAL		spelltime; //Time when they are done casting....
		magic::SpellId		spell; //current spell they are casting....
		SI32			spellaction; //Action of the current spell....
		magic::CastingType	spelltype;
		TIMERVAL		nextact; //time to next spell action....
		TargetLocation*		spellTL; //Luxor: npc spell targetlocation

		SI32			squelched; // zippy  - squelching
		TIMERVAL		mutetime; //Time till they are UN-Squelched.
		SI32			med; // 0=not meditating, 1=meditating //Morrolan - Meditation
		//SI32 statuse[3]; //Morrolan - stat/skill cap STR/INT/DEX in that order
		//SI32 skilluse[TRUESKILLS][1]; //Morrolan - stat/skill cap
		UI08			lockSkill[ALLSKILLS+1]; // LB, client 1.26.2b skill managment
		SI32			stealth; //AntiChrist - stealth ( steps already done, -1=not using )
		UI32			running; //AntiChrist - Stamina Loose while running
		UI32			lastRunning; //Luxor
		SI32			logout; //Time till logout for this char -1 means in the world or already logged out //Instalog
		//UI32 swing;

		UI32			holdg; // Gold a player vendor is holding for Owner
		char			fly_steps; // number of step the creatures flies if it can fly
		TIMERVAL		smoketimer; // LB
		TIMERVAL		smokedisplaytimer;

		TIMERVAL		antispamtimer;//LB - anti spam

		TIMERVAL		antiguardstimer;//AntiChrist - anti "GUARDS" spawn

		SI32			carve; //AntiChrist - for new carve system

		TIMERVAL		begging_timer;
		cMsgBoard::PostType	postType;
		cMsgBoard::QuestType	questType;
		SI32			questDestRegion;
		SI32			questOrigRegion;
		SI32			questBountyReward;		// The current reward amount for the return of this chars head
		SERIAL			questBountyPostSerial;	// The global posting serial number of the bounty message
		SERIAL			murdererSer;			// Serial number of last person that murdered this char

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

		void 			updateStats(SI32 stat);

		void 			setNextMoveTime(short tamediv=1);
		void 			disturbMed();

		void                    drink(P_ITEM pi);       //Luxor: delayed drinking
		void 			hideBySkill();
		void 			hideBySpell(SI32 timer = INVALID);
		UI32  			countItems(UI16 ID, UI16 col= 0xFFFF);

		inline const UI32 CountGold()
		{ return countItems(ITEMID_GOLD); }

		bool			isInBackpack( P_ITEM pi );
		void			addGold(UI16 totgold);

		// The bit for setting what effect gm movement
		// commands shows
		// 0 = off
		// 1 = FlameStrike
		// 2-6 = Sparkles
		SI32			gmMoveEff;

		UI32			getSkillSum();
		SI32			getTeachingDelta(P_CHAR pPlayer, SI32 skill, SI32 sum);
		void			removeItemBonus(cItem* pi);
		inline const bool	isSameAs(pChar pc) const
		{ return this == pc; }

		//! Return the resistance for a defined type
		inline const bool resist(UI32 n) const
		{ return flags & n; }

		void			sysmsg(const TEXT *txt, ...);

                void                    attackStuff (pChar victim);
		void			helpStuff(P_CHAR pc_i);
		void			applyPoison(PoisonType poisontype, SI32 secs = INVALID);
		void			setOwner(P_CHAR owner);
		void			curePoison();
		void			resurrect(NXWCLIENT healer = NULL);
		void			unfreeze( bool calledByTempfx = false );
		void			damage(SI32 amount, DamageType typeofdamage = DAMAGE_PURE, StatType stattobedamaged = STAT_HP);
		void			playAction(UI16 action);
		void			impAction(UI16 action);

//@{
/*!
\name Talk and Emote stuff
*/
protected:
	UI16			emotecolor;		//!< Color for emote messages
	UI08			fonttype;		//!< Speech font to use
	UI16			saycolor;		//!< Color for say messages
public:
	void			talkAll(TEXT *txt, bool antispam = 1);
	void			talk(NXWSOCKET s, TEXT *txt, bool antispam = 1);
	void			emote(NXWSOCKET s,TEXT *txt, bool antispam, ...);
	void			emoteall(char *txt, bool antispam, ...);
	void			talkRunic(NXWSOCKET s, TEXT *txt, bool antispam = 1);
	void			talkAllRunic(TEXT *txt, bool antispam = 0);
//@}

	UI16			distFrom(pChar pc);
	UI16			distFrom(pItem pi);
	
	/*!
	\brief Check if a specified char is in range from this char
	\param pc Char to check if in range
	\param range Maximum distance from this char
	*/
	inline const bool       hasInRange(pChar pc, UI16 range = VISRANGE)
	{ return pc && distFrom( pc ) <= range; }
	
	/*!
	\brief Check if a specified char is in range from this char
	\param pc Char to check if in range
	\param range Maximum distance from this char
	*/
	inline const bool       hasInRange(pItem pc, UI16 range = VISRANGE)
	{ return pi && distFrom( pi ) <= range; }
	
	void			teleport( UI08 flags = TELEFLAG_SENDALL, NXWCLIENT cli = NULL );
	void			facexy(UI16 facex, UI16 facey);

	/*!
	\author Luxor
	\brief Returns line of sight from the char to the give char
	\param pc pointer to the char to check line of sight from
	\return true if is in line of sight
	*/
	inline const bool losFrom(const pChar pc) const
	{ return pc ? lineOfSight( getPosition(), pc->getPosition() ) : false; }

	void			playSFX(SI16 sound, bool onlyToMe = false);
	void			playMonsterSound(MonsterSound sfx);

	bool			checkSkill(Skill sk, SI32 low, SI32 high, bool bRaise = true);

	/*!
	\author Xanathar
	\brief Deletes items from backpack, by id
	\param id id of the item to delete
	\param amount amount of item to delete
	\param color color of item to delete
	\return number of items deleted
	*/
	inline const UI32 delItems(UI16 id, UI32 amount = 1, UI16 color = 0xFFFF)
	{ return body->getBackpack() ? body->getBackpack()->removeItems(amount,id, color) : amount; }

	const bool	checkSkillSparrCheck(Skill sk, SI32 low, SI32 high, P_CHAR pcd);

	/*!
	\brief Get the amount of the given id, color
	\author Flameeyes
	\return amount of items counted
	\param id the id
	\param col the color ( 0xFFFF for all colors )
	\param onlyPrimaryBackpack false if search also in th subpack
	*/
	inline const bool UI32 getAmount(UI16 id, UI16 col=0xFFFF, bool onlyPrimaryBackpack=false )
	{ return body->getBackpack() ? body->getBackpack()->countItems(id, col, !onlyPrimaryBackpack); }

	void			movingFX(P_CHAR destination, short id, SI32 speed, SI32 loop, bool explode, class ParticleFx* part = NULL);
	void			staticFX(short id, SI32 speed, SI32 loop, class ParticleFx* part = NULL);
	void			boltFX(bool bNoParticles);
	void			circleFX(short id);

	void			useHairDye(P_ITEM bottle);

	void			morph ( short bodyid = INVALID, short skincolor = INVALID,
							short hairstyle = INVALID, short haircolor = INVALID, short beardstyle = INVALID,
							short beardcolor = INVALID, const char* newname = NULL, bool bBackup = true);


	void		Kill();
	void		goPlace(SI32);
	bool		knowsSpell(magic::SpellId spellnumber);

	public:
	#ifdef SPAR_NEW_WR_SYSTEM
		NxwCharWrapper*		nearbyChars;
		NxwItemWrapper*		nearbyItems;
	#endif

		std::vector< UI32 >	lootVector;
		virtual void		Delete();
} PACK_NEEDED;

#endif
