/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "common_libs.h"
#include "sregions.h"
#include "sndpkg.h"
#include "magic.h"
#include "tmpeff.h"
#include "race.h"
#include "npcai.h"
#include "network.h"
#include "boats.h"
#include "walking.h"
#include "map.h"
#include "inlines.h"
#include "basics.h"
#include "magic.h"
#include "skills/skills.h"
#include "misc.h"
#include "targeting.h"

#include <wefts_mutex.h>

uint32_t cChar::nextSerial = 1;

/*!
\brief Gets the next serial
\see cSerializable
*/
uint32_t cChar::getNewSerial()
{
	static Wefts::Mutex m;
	
	m.lock();
	bool firstpass = true;
	uint32_t fserial = nextSerial;
	uint32_t nserial;
	do {
		nserial = nextSerial++;
		if ( nserial >= 0x40000000 )
			nserial = 1;
		if ( ! firstpass && nserial == fserial )
		{
			LogCritical("Too much chars created!!!! No more serials free");
		}
		firstpass = false;
	} while ( cSerializable::findBySerial(nserial) );
	
	m.unlock();
	return nserial;
}

cChar::cChar()
	: cSerializable(getNewSerial()), cEventThroewr(eventsNumber())
{
	resetData();
}

cChar::cChar( uint32_t ser )
	: cSerializable(ser)
{
	resetData();
}

//! Resets data in the newly created instances
void cChar::resetData()
{
	cSerializable::resetData();
	client = NULL;
	hidden = htUnhidden;
	setJail(NULL, NULL);
	// Hypnos OK!

	setCurrentName("<this is a bug>");
	setRealName("<this is a bug>");
	title[0]=0x00;

	//TIMERS
	antiguardstimer=getclock();
	antispamtimer=getclock();
	begging_timer=getclock();
	fishingtimer=getclock();
	hungertime=getclock();
	invistimeout=getclock();
	nextact=getclock();
	nextAiCheck=getclock();
	npcmovetime=getclock();
	skilldelay=getclock();
	//

	gmMoveEff=0;
	homeloc.x=0;
	homeloc.y=0;
	homeloc.z=0;
	workloc.x=0;
	workloc.y=0;
	workloc.z=0;
	foodloc.x=0;
	foodloc.y=0;
	foodloc.z=0;


	party=INVALID;
	privLevel = PRIVLEVEL_GUEST;

	setId( bodyMale );
	race=INVALID;
	custmenu=INVALID;
	unicode = false; // This is set to 1 if the player uses unicode speech, 0 if not
	account=INVALID;
	setPosition( 100, 100, 0 );
	setOldPosition( 0, 0, 0, 0 );
	dir=0; //&0F=Direction
	setOldId( 0x0190 ); // Character body type
	setOldColor( 0x0000 ); // Skin color
	keyserial=INVALID;  // for renaming keys
	SetPriv(0); // 1:GM clearance, 2:Broadcast, 4:Invulnerable, 8: single click serial numbers
	// 10: Don't show skill titles, 20: GM Pagable, 40: Can snoop others packs, 80: Counselor clearance
	priv2=0;	// 1:Allmove, 2: Frozen, 4: View houses as icons, 8: permanently hidden
	// 10: no need mana, 20: dispellable, 40: permanent magic reflect, 80: no need reagents
	fonttype=3; // Speech font to use
	saycolor=0x1700; // Color for say messages
	emotecolor=0x0023; // Color for emote messages
	setStrength(50, ( ser == INVALID ? false : true ) ); // Strength
	st2=0; // Reserved for calculation
	dx=50; // Dexterity
	dx2=0; // Reserved for calculation
	in=50; // Intelligence
	in2=0; // Reserved for calculation
	hp=50; // Hitpoints
	stm=50; // Stamina
	mn=50; // Mana
	mn2=0; // Reserved for calculation
	hidamage=0; //NPC Damage
	lodamage=0; //NPC Damage
	SetCreationDay( getclockday() );
	resetSkill();
	resetBaseSkill();
	npc=0;
	shopkeeper = false; //true=npc shopkeeper
	tamed = false; // True if NPC is tamed
	robe = INVALID; // Serial number of generated death robe (If char is a ghost)
	SetKarma(0);
	fame=0;
	kills=0; //PvP Kills
	deaths=0;
	dead = false; // Is character dead
	packitemserial=INVALID; // Only used during character creation
	fixedlight=255; // Fixed lighting level (For chars in dungeons, where they dont see the night)
	speech=0; // For NPCs: Number of the assigned speech block
	weight=0; //Total weight
	att=0; // Intrinsic attack (For monsters that cant carry weapons)
	def=0; // Intrinsic defense
	war=0; // War Mode
	questType = cMsgBoard::QTINVALID; //xan - was not initialized!
	targserial=INVALID; // Current combat target
	timeout=0; // Combat timeout (For hitting)
	timeout2=0;

	setRegenRate( STAT_HP, SrvParms->hitpointrate, VAR_REAL );
	setRegenRate( STAT_HP, SrvParms->hitpointrate, VAR_EFF );
	updateRegenTimer( STAT_HP );
	setRegenRate( STAT_STAMINA, SrvParms->staminarate, VAR_REAL );
	setRegenRate( STAT_STAMINA, SrvParms->staminarate, VAR_EFF );
	updateRegenTimer( STAT_STAMINA );
	setRegenRate( STAT_MANA, SrvParms->manarate, VAR_REAL );
	setRegenRate( STAT_MANA, SrvParms->manarate, VAR_EFF );
	updateRegenTimer( STAT_MANA );

	runeserial=INVALID; // Used for naming runes
	attackerserial=INVALID; // Character who attacked this character
	nextAiCheck=getclock();

	npcmovetime=getclock(); // Next time npc will walk
	npcWander=cNPC::WANDER_NOMOVE; // NPC Wander Mode
	fleeTimer=INVALID;
	oldnpcWander=cNPC::WANDER_NOMOVE; // Used for fleeing npcs
	ftargserial=INVALID; // NPC Follow Target
	fx1=-1; //NPC Wander Point 1 x
	fx2=-1; //NPC Wander Point 2 x
	fy1=-1; //NPC Wander Point 1 y
	fy2=-1; //NPC Wander Point 2 y
	fz1=0; //NPC Wander Point 1 z

	spawnserial=INVALID; // Spawned by
	invistimeout=0;
	ResetAttackFirst(); // 0 = defending, 1 = attacked first
	onhorse=false; // On a horse?
	hunger=6;  // Level of hungerness, 6 = full, 0 = "empty"
	hungertime=0; // Timer used for hunger, one point is dropped every 20 min
	npcaitype=NPCAI_GOOD; // NPC ai
	callnum=-1; //GM Paging
	playercallnum=-1; //GM Paging
	pagegm=0; //GM Paging
	region=255;
	skilldelay=0;
	objectdelay=0;
	combathitmessage=0;
	making=-1; // skill number of skill using to make item, 0 if not making anything.
	blocked=0;
	dir2=0;
	spiritspeaktimer=0; // Timer used for duration of spirit speak
	spattack=0;
	spadelay=0;
	spatimer=0;
	taming=0; //Skill level required for taming
	summontimer=0; //Timer for summoned creatures.
	fishingtimer=0; // Timer used to delay the catching of fish
	advobj=0; //Has used advance gate?
	poison=0; // used for poison skill
	poisoned=poisonNone; // type of poison
	poisontime=0; // poison damage timer
	poisontxt=0; // poision text timer
	poisonwearofftime=0; // LB, makes poision wear off ...
	fleeat=0;
	reattackat=0;
	trigger=0; //Trigger number that character activates
	trigword[0]='\x00'; //Word that character triggers on.
	envokeid=0;
	envokeitem=-1;
	split=0;
	splitchnc=0;
	targtrig=0; //Stores the number of the trigger the character for targeting
	ra=0;  // Reactive Armor spell
	trainer=INVALID; // Serial of the NPC training the char, -1 if none.
	trainingplayerin=0; // Index in skillname of the skill the NPC is training the player in
	cantrain=true;
	ResetGuildTitleToggle();		// Toggle for Guildtitle								(DasRaetsel)
	SetGuildTitle( "" );	// Title Guildmaster granted player 					(DasRaetsel)
	SetGuildFealty( INVALID ); 	// Serial of player you are loyal to (default=yourself) (DasRaetsel)
	SetGuildNumber( 0 );		// Number of guild player is in (0=no guild)			(DasRaetsel)

	flag=0x02; //1=red 2=grey 4=Blue 8=green 10=Orange // grey as default - AntiChrist
	tempflagtime=0;

	murderrate=0; //#of ticks until one murder decays //REPSYS
	murdersave=0;

	crimflag=0; //Time when No longer criminal
	spelltime=0; //Time when they are done casting....
	spell=magic::spellInvalid; //current spell they are casting....
	spellaction=0; //Action of the current spell....
	nextact=0; //time to next spell action....
	squelched=0; // zippy  - squelching
	mutetime=0; //Time till they are UN-Squelched.
	med=0; // 0=not meditating, 1=meditating //Morrolan - Meditation
	stealth=-1; //AntiChrist - stealth ( steps already done, -1=not using )
	running=0; //AntiChrist - Stamina Loose while running
	logout=0;//Time till logout for this char -1 means in the world or already logged out //Instalog
	swingtargserial=INVALID; //Tagret they are going to hit after they swing
	holdg=0; // Gold a player vendor is holding for Owner
	fly_steps=0; //LB -> used for flyging creatures
	guarded=false; // True if CHAR is guarded by some NPC
	smoketimer=0;
	smokedisplaytimer=0;
	carve=-1; // AntiChrist - for new carving system
	antiguardstimer=0; // AntiChrist - for "GUARDS" call-spawn
	polymorph=false;//polymorph - AntiChrist
	namedeedserial=INVALID;
	postType = cMsgBoard::LOCALPOST;
	questDestRegion = 0;
	questOrigRegion= 0;
	questBountyReward= 0;
	questBountyPostSerial = 0;
        questEscortPostSerial = 0;
	murdererSer = 0;
	spawnregion = INVALID;
	npc_type = 0;
	stablemaster_serial = INVALID;
	timeused_last = getclock();
	time_unused = 0;
	npcMoveSpeed = (float)NPCSPEED;
	npcFollowSpeed = (float)NPCFOLLOWSPEED;
	setNpcMoveTime();
	ResetGuildTraitor();
	SetGuildType( INVALID );
	magicsphere = 0;
	resetResists();
	damagetype = damBludgeon;
	fstm=0.0f;

	setGuild( NULL, NULL );

	morphed=0;
	resetLockSkills();

	SetInnocent(); //Luxor
	targetcallback = INVALID;

	statGainedToday = 0;	//Luxor

	speechCurrent = NULL; //Luxor
	lastRunning = 0; //Luxor
	path = NULL; //Luxor
	spellTL = NULL; //Luxor

	staticProfile=NULL;

	vendorItemsSell = NULL;
	vendorItemsBuy = NULL;

	oldmenu=INVALID;
}

/*!
\brief Destructor
*/
cChar::~cChar()
{
	safedelete( staticProfile );
	safedelete( speechCurrent );
	safedelete( path );
	safedelete(crypter);
}

/*!
\note Don't add onstart events in npc scripts, because then they'll also be executed when character is created
add onstart event to character programmatically
\todo Remove this
*/
void cChar::loadEventFromScript(char *script1, char *script2)
{

#if 0
	if (!strcmp("@ONSTART",script1))	{
		amxevents[EVENT_CHR_ONSTART] = newAmxEvent(script2);
		newAmxEvent(script2)->Call(getSerial(), INVALID);
	}
	CASECHAREVENT("@ONDEATH",EVENT_CHR_ONBEFOREDEATH)
	CASECHAREVENT("@ONBEFOREDEATH",EVENT_CHR_ONBEFOREDEATH)		// SYNONYM OF ONDEATH EVENT
	CASECHAREVENT("@ONDIED",EVENT_CHR_ONAFTERDEATH)
	CASECHAREVENT("@ONAFTERDEATH",EVENT_CHR_ONAFTERDEATH)			// SYNONYM OF ONDIED EVENT
	CASECHAREVENT("@ONKILL",EVENT_CHR_ONKILL)
	CASECHAREVENT("@ONWOUNDED",EVENT_CHR_ONWOUNDED)
	CASECHAREVENT("@ONHIT",EVENT_CHR_ONHIT)
	CASECHAREVENT("@ONHITMISS",EVENT_CHR_ONHITMISS)
	CASECHAREVENT("@ONGETHIT",EVENT_CHR_ONGETHIT)
	CASECHAREVENT("@ONWALK",EVENT_CHR_ONWALK)
	CASECHAREVENT("@ONBLOCK",EVENT_CHR_ONBLOCK)
	CASECHAREVENT("@ONHEARTBEAT",EVENT_CHR_ONHEARTBEAT)
	CASECHAREVENT("@ONDISPEL",EVENT_CHR_ONDISPEL)
	CASECHAREVENT("@ONRESURRECT",EVENT_CHR_ONRESURRECT)
	CASECHAREVENT("@ONFLAGCHANGE",EVENT_CHR_ONFLAGCHG)
	CASECHAREVENT("@ONADVANCESKILL",EVENT_CHR_ONADVANCESKILL)
	CASECHAREVENT("@ONGETSKILLCAP",EVENT_CHR_ONGETSKILLCAP)
	CASECHAREVENT("@ONGETSTATCAP",EVENT_CHR_ONGETSTATCAP)
	CASECHAREVENT("@ONADVANCESTAT",EVENT_CHR_ONADVANCESTAT)
	CASECHAREVENT("@ONBEGINATTACK",EVENT_CHR_ONBEGINATTACK)
	CASECHAREVENT("@ONBEGINDEFENSE",EVENT_CHR_ONBEGINDEFENSE)
	CASECHAREVENT("@ONTRANSFER",EVENT_CHR_ONTRANSFER)
	CASECHAREVENT("@ONMULTIENTER",EVENT_CHR_ONMULTIENTER)
	CASECHAREVENT("@ONMULTILEAVE",EVENT_CHR_ONMULTILEAVE)
	CASECHAREVENT("@ONSNOOPED",EVENT_CHR_ONSNOOPED)
	CASECHAREVENT("@ONSTOLEN",EVENT_CHR_ONSTOLEN)
	CASECHAREVENT("@ONPOISONED",EVENT_CHR_ONPOISONED)
	CASECHAREVENT("@ONREGIONCHANGE",EVENT_CHR_ONREGIONCHANGE)
	CASECHAREVENT("@ONCASTSPELL",EVENT_CHR_ONCASTSPELL)
	CASECHAREVENT("@ONREPUTATIONCHANGE",EVENT_CHR_ONREPUTATIONCHG)
	CASECHAREVENT("@ONBREAKMEDITATION",EVENT_CHR_ONBREAKMEDITATION)
	CASECHAREVENT("@ONCLICK",EVENT_CHR_ONCLICK)
	CASECHAREVENT("@ONMOUNT",EVENT_CHR_ONMOUNT)
	CASECHAREVENT("@ONDISMOUNT",EVENT_CHR_ONDISMOUNT)
	CASECHAREVENT("@ONHEARPLAYER",EVENT_CHR_ONHEARPLAYER)
	CASECHAREVENT("@ONDOCOMBAT",EVENT_CHR_ONDOCOMBAT)
	CASECHAREVENT("@ONCOMBATHIT",EVENT_CHR_ONCOMBATHIT)
	CASECHAREVENT("@ONSPEECH",EVENT_CHR_ONSPEECH)
	CASECHAREVENT("@ONCHECKNPCAI",EVENT_CHR_ONCHECKNPCAI)
	else if (!strcmp("@ONCREATION",script1)) 	newAmxEvent(script2)->Call(getSerial(), INVALID);
#endif
}

/*!
\author Luxor
\brief Checks if the stats are regular
*/
void cChar::checkSafeStats()
{
	int32_t nHP, nMN, nSTM;

	nHP = qmin( getStrength(), hp );
	nSTM = qmin( dx, stm );
	nMN = qmin( in, mn );

	//
	// Deactivate temp effects and items stats bonuses
	//
	tempfx::tempeffectsoff();
	NxwItemWrapper si;
	pItem pi;
	si.fillItemWeared( this, true, true, false );
	for( si.rewind(); !si.isEmpty(); si++ ) {
		pi = si.getItem();
		if ( !pi )
			continue;

		if ( pi->st2 != 0 )
			modifyStrength(-pi->st2);
		if ( pi->dx2 != 0 )
			dx -= pi->dx2;
		if ( pi->in2 != 0 )
			in -= pi->in2;
	}

	//
	// Check if stats are correct
	//
	if ( getStrength() != st3 )
		setStrength(st3);

	if ( dx != dx3 )
		dx = dx3;

	if ( in != in3 )
		in = in3;

	//
	// Reactivate temp effects and items stats bonuses
	//
	for( si.rewind(); !si.isEmpty(); si++ ) {
		pi = si.getItem();
		if ( ! pi )
			continue;

		if ( pi->st2 != 0 )
			modifyStrength(pi->st2);
		if ( pi->dx2 != 0 )
			dx += pi->dx2;
		if ( pi->in2 != 0 )
			in += pi->in2;
	}

	tempfx::tempeffectson();

	hp = qmin( getStrength(), nHP );
	stm = qmin( dx, nSTM );
	mn = qmin( in, nMN );
}

void cChar::setStrength(uint32_t val, bool check/*= true*/)
{
	str.value= val;
	if( check )
		checkEquipment();
}

/*!
\brief get the bank box
\author Flameeyes
\param banktype BANK_GOLD or BANK_ITEM .. see note
\note BANK_GOLD are gold&items ( if specialbank onlygold not for only this region )
	BANK_ITEM (only if specialbank) are the bank region of player
\todo make sure that the bank exists
*/
pItem cChar::getBankBox( uint8_t banktype )
{
	if ( ! SrvParms->usespecialbank || banktype == BANK_GOLD )
		return bank
	else
		return specialbanks<region>;
}


/*!
\author Duke
\date 17/03/2001
\brief stops meditation if necessary
\todo Why we can't disturb NPC's meditation?
*/
void cChar::disturbMed()
{
	if (!isMeditating()) return; // no reason to stay here :]
	
	if ( events[evtChrOnBreakMeditation] && getClient() ) {
		events[evtChrOnBreakMeditation]->setParams(tVariantVector(1, getSerial()));
		events[evtChrOnBreakMeditation]->execute();
		if ( events[evtChrOnBreakMeditation]->isBypassed() )
			return;
	}

	setMeditating(false);
	if ( getClient() ) getClient()->sysmessage("You break your concentration.");
}

/*!
\brief Reveals the char if he was hidden
\author Duke and Flameeyes
\todo adding a comment to player? (like "you have been revealed" ? :D)
*/
void cChar::unHide()
{
	//if hidden but not permanently or forced unhide requested
	if ( ! ( isHiddenBySkill() || isPermaHidden() ) )
		return;

	stealth=-1;
	hidden = htUnhidden;

	updateFlag();//AntiChrist - bugfix for highlight color not being updated

	uint32_t my_serial = getSerial();
	sLocation my_pos = getPosition();

	NxwSocketWrapper sw;
	sw.fillOnline( this, false );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient i = sw.getClient();
		if( !i ) continue;

		pChar pj = i->currChar();
		if ( ! pj ) continue;
			
		if (pj != this) { //to other players : recreate player object
			nPackets::Sent::DeleteObj pk(this);
			client->sendPacket(&pk);
			i->sendchar(this, false);
		} else {
			nPackets::Sent::DrawGamePlayer pk(this);
			i->sendPacket(&pk);
		}
	}

	if (IsGM())
		tempfx::add(this, this, tempfx::GM_UNHIDING, 3, 0, 0);
}

/*!
\brief makes a character fight the other
\author Duke
\date 20/03/2001
*/
void cChar::fight(pChar other)
{
	if ( ! other ) return;
	
	//if (!war) toggleCombat(); //Luxor
	targserial=other->getSerial();
	unHide();
	disturbMed(); // Meditation
	attackerserial=other->getSerial();
	if (npc)
	{
		/*if (!war)
			toggleCombat();
		setNpcMoveTime();*/

		//Luxor: let's use npcattack
		npcattacktarget(this, other);
	}
}

/*!
\brief count items of given id and color
\author Flameeyes (based on Duke)
\param matchId id of items to count
\param matchColor color of the items to count

Searches the character recursively, counting the items of the given ID and (if given) color
*/
uint32_t cChar::countItems(uint16_t matchId, uint16_t matchColor)
{
	if ( ! body ) return;
	pContainer bp = body->getBackpack();
	return bp ? bp->countItems(matchId, matchColor) : 0;
}

/*!
\brief Check if an item is in the char's backpack
\author Flameeyes
\param pi item to check
*/
bool cChar::isInBackpack( pItem pi )
{
	if ( ! pi || !body || !body->getBackpack() ) return false;

	return pi->getOutMostCont() == body->getBackpack();
}

void cChar::MoveTo(sLocation newloc)
{
	// Avoid crash if go to 0,0
	if ((newloc.x < 1) || (newloc.y < 1))
		return;

	// <Luxor>
	if ( newloc != getPosition() && (flags & flagIsCasting) && (dynamic_cast<pPC>(this)) ) {
		getClient()->sysmessage( "You stop casting the spell." );
		flags &= ~flagIsCasting;
		spell = magic::spellInvalid;
		spelltime = 0;
	}
	// </Luxor>
	setPosition( newloc );
	pointers::updateLocationMap( this );
}

/*!
\brief Called after helping a character for accomplish to criminals
\param pc_i helped character
*/
void cChar::helpStuff(pChar pc_i)
{
	if (!pc_i || this == pc_i )
		return;

	if (pc_i->isGrey())
		setCrimGrey(nSettings::Reputation::getHelpingGreyAction());
	else if (pc_i->isCriminal())
		setCrimGrey(nSettings::Reputation::getHelpingCriminalAction());
	else if (pc_i->isMurderer())
		makeCriminal();
	else if (pc_i->isInnocent())
	{
		if ((pc_i->GetKarma()>0)&&((pc_i->GetKarma()-GetKarma())>100)) {
			IncreaseKarma(+5);
			getClient()->sysmessage("You've gained a little karma.");
		}
		return;
	}
}

/*!
\brief applies a poison to a char
\author Endymion
\param poisontype the poison
\param secs the duration of poison ( if INVALID ( default ) default duration is used )
*/
void cChar::applyPoison(PoisonType poisontype, int32_t secs )
{
        unfreeze();
	if ( IsInvul() || ! (::region[region].priv&0x40) )
		return;
		
	if (poisontype>poisonDeadly) poisontype = poisonDeadly;
	else if (poisontype<poisonWeak) poisontype = poisonWeak;
	if ( poisontype>=poisoned ) {
		poisoned=poisontype;
		if( secs == INVALID )
			poisonwearofftime=getclock()+(SECS*SrvParms->poisontimer); // lb
		else
			poisonwearofftime=getclock()+(SECS*secs);

		pClient client = getClient();
		client->sendchar(this);
		client->sysmessage("You have been poisoned!");
		playSFX( 0x0246 ); //poison sound - SpaceDog
	}
}

/*!
\author Xanathar
\brief unfreezes the char
*/
void cChar::unfreeze( bool calledByTempfx )
{
	if( !calledByTempfx )
		delTempfx( tempfx::spellParalyze, false ); //Luxor
	
	if ( ! isFrozen() ) return;

	setFrozen(false);
	if (!isCasting()) //Luxor
		sysmsg("You are no longer frozen.");
}

/*!
\brief damages a character
\author Xanathar & Luxor
\param amount how much damage
\param typeofdamage type of damage to use
\param stattobedamaged stat to be damaged
*/
void cChar::damage(int32_t amount, DamageType typeofdamage, StatType stattobedamaged)
{
	if (!npc && !IsOnline())
		return;
	pChar myself=cSerializable::findCharBySerial(getSerial());
	if ( ! myself )
		return;
	pChar pc_att=cSerializable::findCharBySerial(attackerserial);
	uint32_t serial_att= pc_att ? pc_att->getSerial() : INVALID;

	if ( events[evtChrOnWounded] ) {
		tVariantVector params = tVariantVector(3);
		params[0] = getSerial(); params[1] = amount;
		params[2] = serial_att;
		events[evtChrOnWounded]->setParams(params);
		tVariant ret = events[evtChrOnWounded]->execute();
		if ( events[evtChrOnWounded]->isBypassed() )
			return;
		amount = ret.toSInt();
	}

	unfreeze();

	if (amount <= 0) return;
	// typeofdamage is ignored till now
	if (typeofdamage!=damPure) {
		amount -= int32_t((amount/100.0)*float(calcResist(typeofdamage)));
	}
	if (amount <= 0) return;

	switch (stattobedamaged)
	{
		case STAT_MANA:
			mn = qmax(0, mn - amount);
			updateMana();
			break;
		case STAT_STAMINA:
			stm = qmax(0, stm - amount);
			updateStamina();
			break;
		case STAT_HP:
		default:
			hp = qmax(0, hp - amount);
			updateHp();
			if (hp<=0) Kill();
	}
}

/*!
\brief calculates resist power of a char
\author Luxor
\param typeofdamage type of damage to use
\return the resistance
*/
int32_t cChar::calcResist(DamageType typeofdamage)
{
	if (typeofdamage == damPure || typeofdamage > MAX_RESISTANCE_INDEX)
		return 0;

	int32_t total = 0;
	total += resists[typeofdamage];

	NxwItemWrapper si;
	si.fillItemWeared( this, true, true, true );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi=si.getItem();
		if( pi ) {
			total += pi->resists[typeofdamage];
		}
	}
	return total;
}

/*!
\author Luxor
\brief add gold to the char
\param totgold amount of gold to add
*/
void cChar::addGold(uint16_t totgold)
{
	pItem pi = item::CreateFromScript( "$item_gold_coin", getBackpack(), totgold );
	if ( pi )
		pi->Refresh();
}

/*!
\brief Get char's distance from the given character
\author Luxor
\return distance ( if invalid is returned VERY_VERY_FAR )
\param pc the char
*/
uint32_t cChar::distFrom(pChar pc)
{
	if ( ! pc ) return VERY_VERY_FAR;
	
	return (uint32_t)dist(getPosition(),pc->getPosition());
}

/*!
\brief Get char's distance from the given item
\author Endymion
\return distance ( if invalid is returned VERY_VERY_FAR )
\param pi the item
\note it check also if is subcontainer, or weared. so np call freely
*/
uint32_t cChar::distFrom(pItem pi)
{
	if ( ! pi ) return VERY_VERY_FAR;
	pItem cont=pi->getOutMostCont(); //return at least itself
	if ( ! cont ) return VERY_VERY_FAR;

	if(cont->isInWorld())
		return (uint32_t)dist(getPosition(),cont->getPosition());
	else if(cSerializable::isCharSerial(cont->getContSerial())) //can be weared
		return distFrom( cSerializable::findCharBySerial(cont->getContSerial()) );
	else
		return VERY_VERY_FAR; //not world, not weared.. and another cont can't be

}

/*!
\author Luxor
\brief Tells if the char can see the given object
\todo Rewrite this
*/
bool cChar::canSee( pObject obj )
{
	//
	// Check if the object is in visRange
	//
	double distance = dist( obj->getPosition(), getPosition(), false );
	if ( distance > VISRANGE ) // We cannot see it!
		return false;

	uint32_t ser = obj.getSerial();
	if ( cSerializable::isCharSerial( ser ) ) {
		pChar pc = pChar( &obj );
		if ( !IsGM() ) { // Players only
			if ( pc->IsHidden() ) // Hidden chars cannot be seen by Players
				return false;
			if ( pc->dead && !pc->war && !dead ) // Non-persecuting ghosts can be seen only by other ghosts
				return false;
		}
		else
		{
			if ( !pc->IsOnline() && !pc->npc)
				return false;
		}
	}

	/*if ( cSerializable::isItemSerial( ser ) ) { //Future use
	}*/
	return true;
}


/*!
\author Luxor
\brief Teleports character to its current set coordinates.
\todo Break cPC stuff into a cPC function
*/
void cChar::teleport( uint8_t flags, pClient cli )
{
	setMulti( cMulti::getAt(getPosition()) );
	
	updateFlag();	//AntiChrist - Update highlight color

	//
	// Send the draw player packet
	//
	if ( client != INVALID ) {

		nPackets::Sent::DrawGamePlayer pk(this);
		ps->sendPacket(&pk);

		getBody()->calcWeight();
		client->statusWindow( this, true);
		walksequence[socket] = INVALID;
	}


	//
	// Send the object remove packet
	//
	if ( cli == NULL ) { //ndEndy, this send also to current char?
		NxwSocketWrapper sw;
		sw.fillOnline( getOldPosition() );
		for ( sw.rewind(); !sw.isEmpty(); sw++ ) {
			pClient ps_w = sw.getClient();
			if ( ps_w == NULL )
				continue;
			pChar pc = ps_w->currChar();
			if ( pc )
				if ( distFrom( pc ) > VISRANGE || !canSee( *pc ) )
				{
					nPackets::Sent::DeleteObj pk(this);
					ps_w->sendPacket(&pk);
				}
		}
	} else
	{
		nPackets::Sent::DeleteObj pk(this);
		cli->sendPacket(&pk);
	}

        //
        // Send worn items and the char itself to the char (if online) and other players
        //
	if ( !cli )
	{
		NxwSocketWrapper sw;
		sw.fillOnline( this, false );
		for ( sw.rewind(); !sw.isEmpty(); sw++ ) {
			pClient ps_i = sw.getClient();
			if( ps_i != NULL ) {
				ps_i->sendchar(this);
				//ndEndy not too sure of this
				if ( flags&teleSendWornItems )
					wornitems( ps_i->toInt(), this );
			}
		}
		sw.clear();
	} else
	{
		cli->sendchar(this);
		if ( flags&teleSendWornItems )
			wornitems( cli->toInt(), this );
	}


	//
	// Send other players and items to char (if online)
	//
	if ( cli == NULL || cli == getClient() )
		if ( socket != INVALID )
		{
			if ( flags&teleSendNearChars )
			{
				NxwCharWrapper sc;
				sc.fillCharsNearXYZ( getPosition(), VISRANGE, IsGM() ? false : true );
				for( sc.rewind(); !sc.isEmpty(); sc++ )
				{
					pChar pc=sc.getChar();
					if( pc )
					{
						if( getSerial() != pc->getSerial() )
						{
							if ( !pc->IsOnline() && !pc->npc )
							{
								if ( seeForLastTime( *pc ))
								{
									nPackets::Sent::DeleteObj pk(pc);
									getClient()->sendPacket(&pk);
								}
							}
							else
							{
								seeForLastTime( *pc );
								seeForFirstTime( *pc );
								cli->sendchar(pc);
							}
						}
					}
				}

				if ( flags&teleSendNearItems )
				{
					NxwItemWrapper si;
					si.fillItemsNearXYZ( getPosition(), VISRANGE, false );
					for( si.rewind(); !si.isEmpty(); si++ )
					{
						pItem pi = si.getItem();
						if( pi )
							cli->senditem( pi );
					}
				}
			}
		}

	//
	// Send the light level
	//
	if ( socket != INVALID && (flags&teleSendLight) )
		dolight( socket, worldcurlevel );

	//
	// Check if the region changed
	//
	checkregion( this );

	//
	// Send the weather
	//
	if( socket != INVALID && (flags&teleSendWeather) )
		pweather(socket);
}

/*!
\brief Paperdoll title for character [1]
\return A string with the title
*/
std::string cChar::getTitle1() const
{
	int titlenum = 0;
	uint16_t x = baseskill[bestSkill()];

	if (x>=1000) titlenum=8;
	else if (x>=900) titlenum=7;
	else if (x>=800) titlenum=6;
	else if (x>=700) titlenum=5;
	else if (x>=600) titlenum=4;
	else if (x>=500) titlenum=3;
	else if (x>=400) titlenum=2;
	else if (x>=300) titlenum=1;

	return std::string(title[titlenum].prowess);
}

/*!
\brief Paperdoll title for the character [3]
\return a string with the paperdoll title
*/
std::string cChar::getTitle3() // Paperdoll title for character p (3)
{
	int titlenum=0;

	if ( getKarma() >= 10000)
	{
		titlenum=3;
		if (getFame() >= 5000) titlenum=0;
		else if (getFame() >= 2500) titlenum=1;
		else if (getFame() >= 1250) titlenum=2;
	}
	else if ( between(getKarma(), 5000, 9999) )
	{
		titlenum=7;
		if (getFame() >= 5000) titlenum=4;
		else if (getFame() >= 2500) titlenum=5;
		else if (getFame() >= 1250) titlenum=6;
	}
	else if ( between(getKarma(), 2500, 4999) )
	{
		titlenum=11;
		if (getFame() >= 5000) titlenum=8;
		else if (getFame() >= 2500) titlenum=9;
		else if (getFame() >= 1250) titlenum=10;
	}
	else if ( between(getKarma(), 1250, 2499) )
	{
		titlenum=15;
		if (getFame() >= 5000) titlenum=12;
		else if (getFame() >= 2500) titlenum=13;
		else if (getFame() >= 1250) titlenum=14;
	}
	else if ( between(getKarma(), 625, 1249) )
	{
		titlenum=19;
		if (getFame() >= 5000) titlenum=16;
		else if (getFame() >= 1000) titlenum=17;
		else if (getFame() >= 500) titlenum=18;
	}
	else if ( between(getKarma(), -625, 624) )
	{
		titlenum=23;
		if (getFame() >= 5000) titlenum=20;
		else if (getFame() >= 2500) titlenum=21;
		else if (getFame() >= 1250) titlenum=22;
	}
	else if ( between(getKarma(), -1250, -626) )
	{
		titlenum=24;
		if (getFame() >= 10000) titlenum=28;
		else if (getFame() >= 5000) titlenum=27;
		else if (getFame() >= 2500) titlenum=26;
		else if (getFame() >= 1250) titlenum=25;
	}
	else if ( between(getKarma(), -2500, -1251) )
	{
		titlenum=29;
		if (getFame() >= 5000) titlenum=32;
		else if (getFame() >= 2500) titlenum=31;
		else if (getFame() >= 1250) titlenum=30;
	}
	else if ( between(getKarma(), -5000, -2501) )
	{
		titlenum=33;
		if (getFame() >= 10000) titlenum=37;
		else if (getFame() >= 5000) titlenum=36;
		else if (getFame() >= 2500) titlenum=35;
		else if (getFame() >= 1250) titlenum=34;
	}
	else if ( between(getKarma(), -10000, -5001) )
	{
		titlenum=38;
		if (getFame() >= 5000) titlenum=41;
		else if (getFame() >= 2500) titlenum=40;
		else if (getFame() >= 1250) titlenum=39;
	}
	else if ( getKarma() < -10000)
	{
		titlenum=42;
		if (getFame() >= 5000) titlenum=45;
		else if (getFame() >= 2500) titlenum=44;
		else if (getFame() >= 1250) titlenum=43;
	}
	
	if ( getFame() >= 10000 )
	{
		if ( pc->kills >= repsys.maxkills )
		{
			if ( pc->getId() == bodyFemale )
				return std::string("The Murderous Lady ");
			else
				return std::string("The Murderer Lord ");
		} else {
			if ( pc->getId() == bodyFemale )
				return std::string("The ") + title[titlenum].fame + std::string(" Lady ");
			else
				return std::string("The ") + title[titlenum].fame + std::string(" Lord ");
		}
	} else {
		if ( pc->kills >= repsys.maxkills )
			return std::string("The Murderer ");
		else
			return std::string("The ") + title[titlenum].fame;
	}
}

/*!
\brief Generates the entire title plus criminal stuff
*/
std::string cChar::getCompleteTitle() const
{
	//!\todo The gm stuff needs to be changed here
	if (/*IsGM() && account!=0*/ false)
		return getCurrentName() + " " + title;
	else if ( ! isDead() && ( isCriminal() || kills ) ) {
		int titleindex = 0;
		if ( kills >= 100 )
			titleindex = 5;
		else if ( kills >= 50 )
			titleindex = 4;
		else if ( kills >= 20 )
			titleindex = 3;
		else if ( kills >= 10 )
			titleindex = 2;
		else if ( kills >= 5 )
			titleindex = 1
		
		return std::string(title[titleindex].other) + " " + getCurrentName() + ", " + title + getTitle1() + " " + getTitle2();
	} else 
		return getTitle3() + getCurrentName() + 
			(title.lenght() > 0 ? " " + title : title) +
			", " + getTitle1() + " " + getTitle2();
}

/*!
\author Luxor and Flameeyes
\brief returns char's combat skill
\return the index of the char's combat skill
*/
int32_t cChar::getCombatSkill()
{
	pWeapon pw = dynamic_cast<pWeapon>(getBody()->getLayerItem(layWeapon1H));
	if ( ! pw )
		pw = dynamic_cast<pWeapon>(getBody()->getLayerItem(layWeapon2H));
	if ( ! pw )
		return skWrestling;
	
	if ( pw->fightskill != 0 )
		return pw->fightskill;
	else if ( pw->isSwordType() )
		return skSwordsmanship;
	else if ( pw->isMaceType() )
		return skMacefighting;
	else if ( pw->isBowType() )
		return skArchery;
	
	return skWrestling;
}

bool const cChar::canDoGestures() const
{
	if ( IsGM() )
		return true;
	
	if ( isHiddenBySpell() ) return false;	//Luxor: cannot do magic gestures if under invisible spell

	pEquippable pe = getBody()->getLayerItem(layWeapon1H);
	if ( ! (pe->getId()==0x13F9 || pe->getId()==0x0E8A || pe->getId()==0x0DF0 || pe->getId()==0x0DF2
		|| pe->IsChaosOrOrderShield() ))
		return false;
	
	pe = getBody()->getLayerItem(layWeapon2H);
	if ( ! (pe->getId()==0x13F9 || pe->getId()==0x0E8A || pe->getId()==0x0DF0 || pe->getId()==0x0DF2
		|| pe->IsChaosOrOrderShield() ))
		return false;

	return true;	
}

/*!
\author Xanathar
\brief Checks a skill for success
\param sk skill
\param low low bound
\param high high bound
\param bRaise should be raised?
\todo Rewrite
*/
bool cChar::checkSkill(Skill sk, int32_t low, int32_t high, bool bRaise)
{
	pClient ps = getClient();;
	pClient client = INVALID;

	if ( sk < 0 || sk > skTrueSkills ) //Luxor
		return false;

	if( ps != NULL )
		s=ps->toInt();

	if( dead )
		return false;

	if (IsGM())
		return true;

	bool skillused=false;


	if(high>1200)
		high=1200;

	int32_t charrange=skill[sk]-low;    // how far is the player's skill above the required minimum ?

	if(charrange<0)
		charrange=0;

	if (low == high)
	{
		LogCritical("minskill equals maxskill");
		return false;
	}

	float chance=static_cast<float>((charrange*890)/(high-low));

	if (sk!=FISHING)
		chance+=100;  // +100 means: *allways* a minimum of 10% for success
	else
		chance-=100;

	if (skill[sk]==1000)
		chance = 990; //gm always have 10% chance of fail

	if (chance>990)
		chance=990; // *allways* a 10% chance of failure

	if( chance >= rand()%1000 )
		skillused = true;
	else
		if (skillinfo[sk].unhide_onfail == 1)
			unHide();

	if (skillinfo[sk].unhide_onuse == 1)
		unHide();

	if(baseskill[sk]<high)
	{
		if (bRaise)
		{
			if(Skills::AdvanceSkill(this, sk, skillused))
			{
				Skills::updateSkillLevel(this, sk);
				if(!npc && IsOnline())
					updateSkill(sk);
			}
		}
	}
	return skillused;
}

/*!
\brief sends a remove packet to everyone nearby and deletes itself
\author Flameeyes
*/
void cChar::Delete()
{
	if( spawnregion!=INVALID )
		Spawns->removeObject( spawnregion, this );

	if( spawnserial!=INVALID )
		Spawns->removeSpawnDinamic( this );

	pointers::delChar(this);	//Luxor

	nPackets::Sent::DeleteObject pk(serial);

        NxwSocketWrapper sc;
        sc.fillOnline( this );
	for ( sc.rewind(); !sc.isEmpty(); sc++ ) {
		pClient ps = sc.getClient();
		if ( ps != NULL )
			ps->sendPacket(&pk);
	}

	safedelete(this);
}

/*!
\author Luxor
\brief Tells if a char sees an object for the first time
*/
bool cChar::seeForFirstTime( cObject &obj )
{
	uint32_t objser = obj.getSerial();

	//
        // The char cannot see itself for the first time ;)
        //
	if ( objser == getSerial() )
		return false;

	//
	// Check if we can see it
	//
	if ( !canSee( obj ) )
		return false;

	//
	// Check if the object was sent before
	//
	uint32_t_SLIST::iterator it( find( sentObjects.begin(), sentObjects.end(), objser ) );

	if ( it != sentObjects.end() ) // Already sent before
		return false;

	//
	// Seen for the first time, push it in the list and return true
	//
	sentObjects.push_front( objser );

	return true;
}


/*!
\author Luxor
\brief Tells if a char sees an object for the last time
*/
bool cChar::seeForLastTime( cObject &obj )
{
	uint32_t objser = obj.getSerial();

        //
        // The char cannot see itself for the last time ;)
        //
	if ( objser == getSerial() )
		return false;

	//
	// Check if we can see it
	//
	if ( canSee( obj ) )
		return false;

	//
	// Check if the object was sent before
	//
	uint32_t_SLIST::iterator it( find( sentObjects.begin(), sentObjects.end(), objser ) );

	if ( it == sentObjects.end() ) // Never sent before, so why remove it from the display?
		return false;

	//
	// Seen for the last time, remove it from the list and return true
	//
	sentObjects.erase( it );

	return true;
}

/*!
\author Xanathar
\brief Hides a player by skill
*/
void cChar::hideBySkill()
{
	pChar pc_att = cSerializable::findCharBySerial(attackerserial);
	pClient client = (ppc = dynamic_cast<pPC>(this))? ppc->getClient() : NULL;

	if ( pc_att && hasInRange(pc_att) )
	{
		if (client)
			client->sysmessage("You cannot hide while fighting.");
		return;
	}

	if ( IsHidden() )
	{
		if (client)
			client->sysmessage("You are already hidden");
		return;
	}

	if ( !checkSkill(HIDING, 0, 1000) )
	{
		if (client)
			sysmessage("You are unable to hide here.");
		return;
	}

	if ( IsGM() )
	{
		staticFX(this, 0x3709, 9, 25);
		playSFX( 0x0208 );
		tempfx::add(this, this, tempfx::GM_HIDING, 1, 0, 0);
		// immediate hiding overwrites the effect.
		// so lets hide after 4 secs.
		// 1 sec works fine now so changed to this.
		return;
	}

	if (client)
		client->sysmessage("You have hidden yourself well.");

	hidden = htBySkill;
	teleport( teleNone );
}

/*!
\author Xanathar & Luxor
\brief Hides a player by spell
*/
void cChar::hideBySpell(int32_t timer)
{
	if (timer == INVALID) timer = SrvParms->invisibiliytimer;
	tempfx::add(this, this, tempfx::spellInvisibility, 0,0,0, timer);
}

/*!
\brief Cures poison
\author Luxor
*/
void cChar::curePoison()
{
	poisoned = poisonNone;
	poisonwearofftime = getclock();
	if (getClient()) getClient()->sendchar(this);
}

/*!
\author Xanather
\brief Resurrects a char
\param healer Player that resurrected the char
\todo Reverify
*/
void cChar::resurrect( pClient healer )
{
	if ( ! isDead() )
	{
		if( healer )
			healer->sysmessage("That person isn't dead");
		return;
	}
	
	setDead(false);
	
	if(!npc || morphed)
		morph();
	hp= getStrength();
	stm= dx;
	mn=in;

	if ( events[evtChrOnResurrect] ) {
		tVariantVector params = tVariantVector(2);
		params[0] = getSerial(); params[1] = healer ? healer->currChar()->getSerial() : INVALID;
		events[evtChrOnResurrect]->setParams(params);
		events[evtChrOnResurrect]->execute();
		if ( events[evtChrOnResurrect]->isBypassed() )
			return;
	}

	modifyFame(0);
	playSFX( 0x0214);
	setId( getOldId() );
	setColor( getOldColor() );
	attacker = NULL;
	ResetAttackFirst();
	war=0;
	hunger=6;

	pEquippable pe = getBody()->getLayerItem(layNeck);
	getBody()->setLayerItem(layNeck, NULL);
	getBody()->setLayerItem(layBackpack, pe);
	packitemserial = pe->getSerial();
	
	if( robe )
		robe->Delete();

	pItem pi = item::CreateFromScript( "$item_robe_1", this );
	if( pi ) {
		pi->setCurrentName( "a resurrect robe" );
		pi->layer = LAYER_OUTER_TORSO;
		pi->setContainer(this);
		pi->setDyeable(true);
	}
	teleport( teleSendWornItems | teleSendLight );
}

/*!
\brief Sets owner fields
\param nowner new owner
\todo Should be virtu
*/
void cChar::setOwner(pChar nowner)
{
	cObject::setOwner(nowner);
	
	npcWander=cNPC::WANDER_NOMOVE;
	setTamed(true);
	npcaitype=NPCAI_GOOD;
}

/*!
\author Xanathar
\note based on Antichrist incognito code
\brief Characters morphing (incognito, polymorph, etc)
\note Calling it with no params will undo any morphing
\note Any morphing with backup on will undo all previous morphings and install itself
\note Can recurse :]
\todo must be rewrited! body's now used!!!
*/
void cChar::morph ( short bodyid, short skincolor, short hairstyle, short haircolor,
    short beardstyle, short beardcolor, const char* newname, bool bBackup)
{
	if ((bodyid==INVALID)&&(skincolor==INVALID)&&(hairstyle==INVALID)&&
	    (haircolor==INVALID)&&(beardstyle==INVALID)&&(beardcolor==INVALID)&& (newname==NULL))
	{ // if empty morph called, no matter which bBackup status
		if(!morphed)
		{
			WarnOut("cChar::morph(<void>) called on non-morphed char\n");
			return;
		}
		morphed = false; //otherwise it will inf-loop
		morph( getOldId(), getOldColor(), oldhairstyle, oldhaircolor, oldbeardstyle, oldbeardcolor,getRealNameC(), false);
		return;
	}

	// if already morphed and should backup, restore old backup first
	if ((morphed)&&(bBackup))
		morph();

	pItem phair = getHairItem();
	pItem pbeard = getBeardItem();

	if (bBackup)
	{
		setOldId( getId() );
		setOldColor( getColor() );

		setRealName( getCurrentName().c_str() );
		if( pbeard )
		{
			oldbeardstyle = pbeard->getId();
			oldbeardcolor = pbeard->getColor();
		}
		if( phair )
		{
			oldhairstyle = phair->getId();
			oldhaircolor = phair->getColor();
		}
	}

	if(bodyid!=INVALID)
		setId( bodyid );

	if(skincolor!=INVALID)
		setColor( skincolor );

	if (newname!=NULL)
		setCurrentName(newname);

	if( pbeard )
	{
		if (beardstyle!=INVALID)
			pbeard->setId( beardstyle );
		if (beardcolor!=INVALID)
			pbeard->setColor( beardcolor );
	}

	if( phair )
	{
		if (hairstyle!=INVALID)
			phair->setId( hairstyle );
		if (haircolor!=INVALID)
			phair->setColor( haircolor );
	}

	morphed = bBackup;

	teleport( teleSendWornItems );

}

/*!
\brief shifts a player in the body of a npc, usually used only by GMs
\author Luxor
\param pc the character to possess
*/
void cChar::possess(pChar pc)
{
	if ( !IsOnline() )
		return;

	if ( ! pc ) return;
	bool bSwitchBack = false;

	pClient client = getSocket();
	if ( socket == INVALID )
		return;

	if ( possessorSerial != INVALID ) { //We're in a possessed Char! Switch back to possessor
		pChar pcPossessor = cSerializable::findCharBySerial( possessorSerial );
		if ( pcPossessor ) {
			bSwitchBack = true;
			pc = pcPossessor;
			possessorSerial = INVALID;
		} else
			return;
	} else { //Normal checks to prevent possessing a not permitted char
		if ( pc->shopkeeper ) {
			sysmsg( "You cannot use shopkeepers." );
			return;
		}
		if ( !pc->npc ) {
			sysmsg( "You can only possess NPCs." );
			return;
		}
		if ( pc->possessorSerial != INVALID ) {
				sysmsg( "This NPC is already possessed by someone!" );
				return;
		}
	}

	uint8_t usTemp;
	int8_t sTemp;

	//PRIV
	usTemp = GetPriv();
	SetPriv( pc->GetPriv() );
	pc->SetPriv(usTemp);

	//PRIV2
	sTemp = GetPriv2();
	SetPriv2( pc->GetPriv2() );
	pc->SetPriv2(sTemp);

	//commandLevel
	usTemp = commandLevel;
	commandLevel = pc->commandLevel;
	pc->commandLevel = usTemp;

	//Serials
	if ( bSwitchBack ) {
		possessorSerial = INVALID;
		pc->possessedSerial = INVALID;
	} else {
		pc->possessorSerial = getSerial();
		possessedSerial = pc->getSerial();
	}

	//Network related stuff
	( bSwitchBack ) ? npc = 1 : pc->npc = 0;
	currchar[ socket ] = pc->getSerial();
	pc->setClient( new cNxwClientObj( socket ) );
	setClient( NULL );

	//Set offline the old body, and online the new one
	if ( bSwitchBack ) {
		Accounts->SetOffline( pc->account );
		Accounts->SetOnline( pc->account, pc );

	} else {
		Accounts->SetOffline( account );
		Accounts->SetOnline( account, pc );
	}

	//Let's go! :)
	Network->enterchar( socket );
}

/*!
\author Xanathar
\brief Teleports char to a goplace location
\param loc goplace location
*/
void cChar::goPlace(int32_t loc)
{
	uint16_t xx,yy,zz;
	location2xyz(loc, xx,yy,zz);
	MoveTo( xx,yy,zz );
}

/*!
\author Flameeyes
\brief Checks if a char knows a given spell
\param spellnumber spell identifier to check
\return true if the char know the spell
*/
bool cChar::knowsSpell(magic::SpellId spellnumber)
{
	if ( ! body->getBackpack() )
		return false;

	pContainer sb = dynamic_cast<pContainer>(body->getBackpack()->findFirstType(ITYPE_SPELLBOOK));

	return sb && sb->containsSpell(spellnumber);
}

/*!
\brief Manages/causes character death
\author Xanathar
\note Original by LB, ANtichrist, Ripper, Tauriel, JustMicheal annd others
\note expanded call to old PlayDeathSound
*/
void cChar::Kill()
{
	if (dead || npcaitype == NPCAI_PLAYERVENDOR || IsInvul() )
		return;

	pClient ps=getClient();

	char murderername[128];
	murderername[0] = '\0';

	if ( events[evtChrBeforeDeath] ) {
		tVariantVector params = tVariantVector(2);
		params[0] = getSerial(); params[1] = INVALID;
		events[evtChrBeforeDeath]->setParams(params);
		events[evtChrBeforeDeath]->execute();
		if ( events[evtChrBeforeDeath]->isBypassed() )
			return;
	}

	if ( ps != NULL )
		unmountHorse();	//Luxor bug fix
	if (morphed)
		morph();

	dead = true;
	poisoned = poisonNone;
	poison = hp = 0;

	if( getOldId() == bodyFemale)
	{
		switch(RandomNum(0, 3)) // AntiChrist - uses all the sound effects
		{
			case 0:	playSFX( 0x0150 ); break;// Female Death
			case 1:	playSFX( 0x0151 ); break;// Female Death
			case 2:	playSFX( 0x0152 ); break;// Female Death
			case 3:	playSFX( 0x0153 ); break;// Female Death
		}
	}
	else if ( getOldId()  == bodyMale)
	{
		switch( RandomNum(0, 3) ) // AntiChrist - uses all the sound effects
		{
			case 0:	playSFX( 0x015A ); break;// Male Death
			case 1:	playSFX( 0x015B ); break;// Male Death
			case 2:	playSFX( 0x015C ); break;// Male Death
			case 3:	playSFX( 0x015D ); break;// Male Death
		}
	}
	else
		playMonsterSound(SND_DIE);

	if( polymorph )
	{ // legacy code : should be cut when polymorph will be translated to morph
		setId( getOldId() );
		polymorph=false;
		teleport( teleSendWornItems );
	}

	murdererSer = INVALID;

	//--------------------- reputation stuff
#ifdef SPAR_C_LOCATION_MAP
	CharList *pCV = pointers::getNearbyChars( this, VISRANGE*2, pointers::COMBATTARGET );
	CharList it( pCV->begin() ), end( pCV->end() );
	pChar pKiller = 0;
	while( it != end )
	{
		pKiller = (*it);
		if( pKiller->npcaitype==NPCAI_TELEPORTGUARD )
		{
			pKiller->summontimer=(getclock()+(SECS*20));
			pKiller->npcWander=cNPC::WANDER_FREELY_CIRCLE;
			pKiller->setNpcMoveTime();
			pKiller->talkAll("Thou have suffered thy punishment, scoundrel.",0);
		}

		pKiller->targserial=INVALID;
		pKiller->timeout=0;

		pChar pk_att = cSerializable::findCharBySerial( pKiller->attackerserial );
		if ( pk_att )
		{
			pk_att->ResetAttackFirst();
			pk_att->attackerserial = INVALID;
		}

		pKiller->attackerserial = INVALID;
		pKiller->ResetAttackFirst();

		if( pKiller->attackerserial == getSerial() )
		{
			pKiller->attackerserial = INVALID;
			pKiller->ResetAttackFirst();
		}

		if( !pKiller->npc )
		{
			strncpy(murderername, pKiller->getCurrentName().c_str(), 48);

			if( pKiller->party != INVALID )
			{
				CharList *pcvParty = pointers::getNearbyChars( pKiller, VISRANGE, pointers::PARTYMEMBER );
				CharList partyIt( pcvParty->begin() ), partyEnd( pcvParty->end() );
				pChar pMember = 0;
				while( partyIt != partyEnd )
				{
					pMember = (*partyIt);
					pMember->IncreaseKarma( (0-(karma)), this  );
					pMember->modifyFame( fame );
					++partyIt;
				}
			}

			pKiller->IncreaseKarma( (0-(karma)), this  );
			pKiller->modifyFame( fame );

			//murder count \/
			if (!npc)
			{ // PvP
				if ( !IsGrey() && IsInnocent() && Guilds->Compare(pKiller,this) == 0 )
				{
					murdererSer = pKiller->getSerial();
					++pKiller->kills;
					pKiller->sysmsg("You have killed %i innocent people.", pKiller->kills);

					if (pKiller->kills==(unsigned)repsys.maxkills)
						pKiller->sysmsg("You are now a murderer!");
					pKiller->updateFlag();

					if (SrvParms->pvp_log)
					{
							LogFile pvplog("PvP.log");
							pvplog.Write("%s was killed by %s!\n",getCurrentName().c_str(), pKiller->getCurrentName().c_str());
					}
				}   // was innocent

				pFunctionHandle evt = pKiller->getEvent(evtChrOnKill);
				if ( evt ) {
					tVariantVector params = tVariantVector(2);
					params[0] = pKiller->getSerial(); params[1] = getSerial();
					evt->setParams(params);
					evt->execute();
				}
			
			} //PvP
		}//if !npc
		else
		{
			pFunctionHandle evt = pKiller->getEvent(evtChrOnKill);
			if ( evt ) {
				tVariantVector params = tVariantVector(2);
				params[0] = pKiller->getSerial(); params[1] = getSerial();
				evt->setParams(params);
				evt->execute();
			}
			
			if (pKiller->war)
				pKiller->toggleCombat(); // ripper
		}
		++it;
	}
#else
	pChar pk = NULL;

	NxwCharWrapper sc;
	sc.fillCharsNearXYZ( getPosition(), VISRANGE*2, false );
	for( sc.rewind(); !sc.isEmpty(); sc++ )
	{
		pk = sc.getChar();
		if(! pk || pk->targserial!=getSerial() )
			continue;

		if (pk->npcaitype==NPCAI_TELEPORTGUARD)
		{
			pk->summontimer=(getclock()+(SECS*20));
			pk->npcWander=cNPC::WANDER_FREELY_CIRCLE;
			pk->setNpcMoveTime();
			pk->talkAll("Thou have suffered thy punishment, scoundrel.", false);
		}

		pk->targserial=INVALID;
		pk->timeout=0;

		pChar pk_att=cSerializable::findCharBySerial(pk->attackerserial);
		if ( pk_att )
		{
			pk_att->ResetAttackFirst();
			pk_att->attackerserial=INVALID; // lb crashfix
		}

		pk->attackerserial=INVALID;
		pk->ResetAttackFirst();

		if (!pk->npc)
		{
			strncpy(murderername, pk->getCurrentName().c_str(), 48);

			NxwCharWrapper party;
			party.fillPartyFriend( pk, VISRANGE, true );
			for( party.rewind(); !party.isEmpty(); party++ )
			{
				pChar fr=party.getChar();
				if( fr )
				{
					fr->IncreaseKarma( (0-(karma)), this  );
					fr->modifyFame( fame );
				}
			}

			pk->IncreaseKarma( (0-(karma)), this  );
			pk->modifyFame( fame );

			//murder count \/
			if (!npc)
			{ // PvP
				if ( (!IsGrey()) && IsInnocent() && Guilds->Compare(pk,this)==0)
				{
					murdererSer = pk->getSerial();
					pk->kills++;
					pk->sysmsg("You have killed %i innocent people.", pk->kills);

					if (pk->kills==(unsigned)repsys.maxkills)
						pk->sysmsg("You are now a murderer!");
					pk->updateFlag();

					if (SrvParms->pvp_log)
					{
							LogFile pvplog("PvP.log");
							pvplog.Write("%s was killed by %s!\n",getCurrentName().c_str(), pk->getCurrentName().c_str());
					}
				}   // was innocent
	
				pFunctionHandle evt = pk->getEvent(evtChrOnKill);
				if ( evt ) {
					tVariantVector params = tVariantVector(2);
					params[0] = pk->getSerial(); params[1] = getSerial();
					evt->setParams(params);
					evt->execute();
				}
			} //PvP
		}//if !npc
		else
		{
			pFunctionHandle evt = pk->getEvent(evtChrOnKill);
			if ( evt ) {
				tVariantVector params = tVariantVector(2);
				params[0] = pk->getSerial(); params[1] = getSerial();
				evt->setParams(params);
				evt->execute();
			}
			if (pk->war)
				pk->toggleCombat(); // ripper

		}
	}
#endif

	// Remove summoned monsters

	//--------------------- trade stuff

	if ( summontimer > 0 )
	{
		Delete();
		return;
	}
	NxwItemWrapper weared;
	weared.fillItemWeared( this, true, true, false );
	for( weared.rewind(); !weared.isEmpty(); weared++ ) {

		pItem pi_j=weared.getItem();

		if(! pi_j )
			continue;

		if ((pi_j->type==ITYPE_CONTAINER) && (pi_j->getPosition().x==26) && (pi_j->getPosition().y==0) &&
			(pi_j->getPosition().z==0) && (pi_j->getId()==0x1E5E) )
		{
			endtrade(pi_j);
		}
	}

	//--------------------- corpse & ghost stuff

	bool hadHumanBody=HasHumanBody();
	int32_t corpseid = (getId() == bodyFemale)? bodyFemaleDead : bodyMaleDead;

	if( ps!=NULL )
		morph( corpseid, 0, 0, 0, 0, 0, NULL, true);

	if (!npc)
	{
		pItem pDeathRobe = cItem::addByID( ITEMID_DEATHSHROUD, 1, "a death shroud", 0, getPosition());
        	if (pDeathRobe)
		{
			robe = pDeathRobe->getSerial();
			pDeathRobe->setContainer(this);
			pDeathRobe->layer = LAYER_OUTER_TORSO;
			pDeathRobe->def = 1;
			pDeathRobe->Refresh();
		}
		else
		{
			ErrOut("cChar::Kill() : Failed to create death robe\n");
		}
	}

	char szCorpseName[128];
	sprintf(szCorpseName, "corpse of %s", getCurrentName().c_str());

	pItem pCorpse = cItem::addByID( ITEMID_CORPSEBASE, 1, szCorpseName, getOldColor(), getPosition());
	if ( !pCorpse )
	{
	    // panic
	    PanicOut("cChar::Kill() failed to create corpse.\n");
	    return;
	}

	if (!npc)
	{
		// looting mode of corpse
		if     ( IsInnocent() )
			pCorpse->more2 = 1;
		else if(IsGrey())
			pCorpse->more2 = ServerScp::g_nGreyCanBeLooted+1;
		else if (IsCriminal())
			pCorpse->more2 = 2;
		else if (IsMurderer())
			pCorpse->more2 = 3;
		pCorpse->setOwner(this);
		pCorpse->more4 = char( SrvParms->playercorpsedecaymultiplier&0xff ); // how many times longer for the player's corpse to decay
	}

	pCorpse->amount = getOldId(); // Amount == corpse type
	pCorpse->morey = hadHumanBody;

	pCorpse->carve=carve;               //store carve section - AntiChrist

	pCorpse->setSecondaryName(getCurrentName().c_str());

	pCorpse->type = ITYPE_CONTAINER;

	pCorpse->more1 = 0;
	pCorpse->dir = dir;
	pCorpse->corpse = 1;
	pCorpse->setDecayTime();

	pCorpse->murderer = string(murderername);
	pCorpse->murdertime = getclock();
	pCorpse->Refresh();


	//--------------------- dropping items to corpse

	pItem pBackPack = getBackpack();
	if (! pBackPack )
		pBackPack = pCorpse;
	//
	//	Sparhawk:	new just in time loot handling
	//
	if( !lootVector.empty() )
	{
		std::vector< uint32_t >::iterator it( lootVector.begin() ), end( lootVector.end() );
		while( it != end )
		{
			char lootItem[32];
			numtostr( *it, lootItem );
			npcs::AddRandomLoot( pBackPack, lootItem );
			++it;
		}
		lootVector.clear();
	}
	//
	//
	//


	NxwItemWrapper si;
	si.fillItemWeared( this, false, false, true ); //Endymion adding weared item
	si.fillItemsInContainer( pBackPack, false, false ); //Endymion adding backpack item

	for( si.rewind(); !si.isEmpty(); si++ ) {

		pItem pi_j=si.getItem();
		if( !pi_j )
			continue;

		//the backpack
		if (pi_j == pBackPack ) continue;

		//not the death robe
		if (pi_j->getSerial() == robe) continue;

		//weared so remove stat bonus ( MOVING IT TO CORPSE so not continue :] )
		if (pi_j->getContainer() == this)
		{
			if(pi_j->st2) modifyStrength(-pi_j->st2);
			if(pi_j->dx2) dx-=pi_j->dx2;
			if(pi_j->in2) in-=pi_j->in2;
		}

		//spell book or newbie are moved to backpack
		if ( pi_j->type == ITYPE_SPELLBOOK || pi_j->isNewbie() ) {
			pi_j->setContainer( pBackPack );
			pi_j->Refresh();
			continue;
		}

		pi_j->setContainer( pCorpse );
		//General Lee
		sLocation lj = pi_j->getPosition();
		lj.y = RandomNum(85,160);
		lj.x = RandomNum(20,70);
		pi_j->setPosition( lj );
		pi_j->Refresh();
		//General Lee
	}

        if ( !npc )
		teleport( teleSendWornItems );

	pCorpse->Refresh();

	if ( events[evtChrAfterDeath] ) {
		tVariantVector params = tVariantVector(2);
		params[0] = getSerial(); params[1] = pCorpse->getSerial();
		events[evtChrAfterDeath]->setParams(params);
		events[evtChrAfterDeath]->execute();
	}

	if ( npc )
	{
		Delete();
	}
	else
		++deaths;
}

/*!
\brief Update npcmove timer
\author Sparhawk
*/
void cChar::setNpcMoveTime()
{
//	npcmovetime = getclock();
	if ( npcWander == cNPC::WANDER_FOLLOW )
		npcmovetime = uint32_t( getclock() + ( float( npcFollowSpeed * SECS ) ) );
	else
		npcmovetime = uint32_t( getclock() + ( float( npcMoveSpeed * SECS ) ) );
}

/*!
\brief unequip items that you can nop longer wear/use
\author LB
*/
void cChar::checkEquipment()
{
	char temp2[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	pItem pi;

	if (npc) return;

	sLocation charpos = getPosition();

	NxwItemWrapper si;
	si.fillItemWeared( this, false, false, true );
	for( si.rewind(); !si.isEmpty(); si++ ) {

		pi=si.getItem();
		if( ! pi )
			continue;
		if (((pi->st > getStrength()) || !pi->checkItemUsability(this, ITEM_USE_CHECKEQUIP)) && !pi->isNewbie())//if strength required > character's strength, and the item is not newbie
		{
			if( strncmp(pi->getCurrentName().c_str(), "#", 1) )
				pi->getName(temp2);
			else
				strcpy(temp2,pi->getCurrentName().c_str());

			if( pi->st > getStrength()) sysmsg("You are not strong enough to keep %s equipped!", temp2);
			playSFX( itemsfx(pi->getId()) );

			//Subtract stats bonus and poison
			modifyStrength(-pi->st2,false);
			dx-=pi->dx2;
			in-=pi->in2;
			if(poison && pi->poisoned) poison-=pi->poisoned;
			if(poison<0) poison=0;

			pi->setContainer(o);
//			pi->MoveTo( charpos.x, charpos.y, charpos.z );
			pi->MoveTo( charpos );

			NxwSocketWrapper sw;
			sw.fillOnline( this, false );

			for( sw.rewind(); !sw.isEmpty(); sw++ ) {
				pClient ps=sw.getClient();
				if(ps!=NULL ) {
					wornitems(ps->toInt(), this );
					senditem(ps->toInt(), pi);
				}
			}
		}
	}
}

void cChar::showLongName( pChar showToWho, bool showSerials )
{
	if ( ! showToWho ) return;
	//!\todo Virtualize
	
	pClient client = showToWho->getSocket();
	if (socket < 0 || socket > now) return;

	//!\todo Remove these temp vars
	char temp[TEMP_STR_SIZE];
 	char temp1[TEMP_STR_SIZE];

	*(temp1)='\0';

	if( fame >= 10000 )
	{ // adding Lord/Lady to title overhead
		switch ( getId() )
		{
			case bodyFemale :
				if ( strcmp(::title[9].other,"") )
				{
					sprintf(temp,"%s ",::title[9].other);
					strcat(temp1,temp);
				}
				break;
			case bodyMale	:
				if (strcmp(::title[10].other,""))
				{
					sprintf(temp,"%s ",::title[10].other);
					strcat(temp1,temp);
				}
				break;
			default	:
				break;
		}
	}

	if ( !npc && !IsOnline() && strcmp(::title[8].other,"") )
	{
		sprintf( temp, "%s (%s)",::title[8].other, getCurrentName().c_str());
		strcat( temp1, temp );
	}
	else
		strcat( temp1, getCurrentName().c_str() );

	if ( showToWho->CanSeeSerials() || showSerials )
	{
		sprintf( temp, " [%x]", getSerial() );
		strcat( temp1, temp );
	}

	if ( squelched )
	{
		if ( !strcmp(::title[7].other,"") )
		{
			sprintf(temp," [%s]",::title[7].other);
			strcat(temp1,temp);
		}
	}

	if ( IsInvul() && account != 0 )
	{
		if (strcmp(::title[11].other,""))
		{
			sprintf(temp, " [%s]",::title[11].other);
			strcat(temp1,temp);
		}
	}
	if ( account==0 && IsGM() )
	{
		if (strcmp(::title[12].other,""))
		{
			sprintf(temp, " [%s]",::title[12].other);
			strcat(temp1,temp);
		}
	}
	if ( isFrozen() )
	{
		if (strcmp(::title[13].other,""))
		{
			sprintf(temp, " [%s]",::title[13].other);
			strcat(temp1,temp);
		}
	}
	if (guarded)
	{
		if (strcmp(::title[14].other,""))
		{
			sprintf(temp, " [%s]",::title[14].other);
			strcat(temp1,temp);
		}
	}
	if (tamed && npcaitype==NPCAI_PETGUARD && getOwner() == showToWho && showToWho->guarded)
	{
		if  (strcmp(::title[15].other,""))
		{
			sprintf(temp, " [%s]",::title[15].other);
			strcat(temp1,temp);
		}
	}
	if (tamed && npcaitype!=NPCAI_PLAYERVENDOR )
	{
		if  (strcmp(::title[16].other,""))
		{
			sprintf(temp, " [%s]",::title[16].other);
			strcat(temp1,temp);
		}
	}
	if (war)
	{
		if  (strcmp(::title[17].other,""))
		{
			sprintf(temp, " [%s]",::title[17].other);
			strcat(temp1,temp);
		}
	}
	if (!npc && IsCriminal() && kills < repsys.maxkills )
	{
		if  (strcmp(::title[18].other,""))
		{
			sprintf(temp, " [%s]",::title[18].other);
			strcat(temp1,temp);
		}
	}
	if (!npc && kills>=repsys.maxkills)
	{
		if  (strcmp(::title[19].other,""))
		{
			sprintf(temp, " [%s]",::title[19].other);
			strcat(temp1,temp);
		}
	}

	Guilds->Title( socket, this );

	uint16_t color;
	int32_t guild = Guilds->Compare(showToWho,this);

	uint8_t sysname[30]={ 0x00, };
	strcpy((char *)sysname, "System");

	if (guild==1) //Same guild (Green)
	{
		color = 0x0043;
	}
	else if (guild==2) //enemy (Orange)
	{
		color = 0x0030;
	}
	else if( IsGM() && account==0 ) //Admin & GM get yellow names
	{
		color = 0x0481;
	}
	else if (IsGrey())
	{
		color = 0x03B2;
	}
	else
	{
		switch(flag)
		{
		case 0x01:	color = 0x0026; break;//red
		case 0x04:	color = 0x005A; break;//blue
		case 0x08:	color = 0x0049; break;//green
		case 0x10:	color = 0x0030; break;//orange
		default:	color = 0x03B2;	//grey
		}
	}

	//!\todo redo adding to cpeech all the data and verifying
	nPackets::Sent::Speech pk(cSpeech(temp1));
	client->sendPacket(&pk);

	//SendSpeechMessagePkt(socket, getSerial(), 0x0101, 6, color, 0x0003, sysname, temp1);
}

/*!
\brief makes the character drinking something
\author Luxor
\since 0.82a
\param pi the object to drink from
*/
void cChar::drink(pItem pi)
{
        if (pi && pi->getType() == ITYPE_POTION) {
                tempfx::add(this, pi, tempfx::DRINK_EMOTE, 0, 0, 0, 0);
                tempfx::add(this, pi, tempfx::DRINK_EMOTE, 0, 0, 0, 1);
                tempfx::add(this, pi, tempfx::DRINK_EMOTE, 0, 0, 0, 2);
                tempfx::add(this, pi, tempfx::DRINK_FINISHED, 0, 0, 0, 3);
        }
}

void cChar::generic_heartbeat()
{
	bool update[3] = { false, false, false };

	if (hp > getStrength()) {
		hp = getStrength();
		update[ 0 ] = true;
	}
	if (stm > dx) {
		stm = dx;
		update[ 2 ] = true;
	}
	if (mn > in) {
		mn = in;
		update[ 1 ] = true;
	}

	//HP REGEN
	if( regenTimerOk( STAT_HP ) ) {
		if (hp < getStrength() && (hunger > 3 || SrvParms->hungerrate == 0)) {
			hp++;
			update[ 0 ] = true;
		}

		this->updateRegenTimer( STAT_HP );
	}

	//STAMINA REGEN
	if( regenTimerOk( STAT_STAMINA )) {
		if (stm < dx) {
			stm++;
			update[ 2 ] = true;
		}

		this->updateRegenTimer( STAT_STAMINA );
	}

	//MANA REGEN
	if( regenTimerOk( STAT_MANA ) )
	{
		if (mn < in)
		{
			mn += 3;
			update[ 1 ] = true;
		}
		else if (med)
		{
			if( !npc )
				sysmsg("You are at peace.");
			med = 0;
		}

		uint32_t manarate = getRegenRate( STAT_MANA, VAR_REAL );
		if (med)
			manarate += uint32_t( calcDef(0) / 10.0 ) - uint32_t( skill[skMeditation]/222.2 );
		else
			manarate += uint32_t( calcDef(0) / 5.0 );
                manarate = qmax( 1, manarate );
		setRegenRate( STAT_MANA, manarate, VAR_EFF );
		updateRegenTimer( STAT_MANA );

	}
	if ( hp <= 0 )
		Kill();
	else
	{
		updateHp();
		updateMana();
		updateStamina();
	}
}

/*!
\brief Updates Hp to nearby players
*/

void cChar::updateHp()
{
	checkSafeStats();

	nPackets::Sent::UpdateHp pk(this);

	NxwSocketWrapper sw;
	sw.fillOnline( this, false );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient i=sw.getSocket();
		if( i ) i->sendPacket(&pk);
	}
}

/*!
\brief Updates Statmina to this player and party members
*/
void cChar::updateStamina()
{
	checkSafeStats();

	nPackets::Sent::UpdateStamina pk(this);

	pClient client = getClient();
	if (client) sendPacket(&pk);
	//! \todo when party updated, add a sendPacket for each party member in visual range
}
/*!
\brief Updates Mana to this player and party members
*/
void cChar::updateMana()
{
	checkSafeStats();

	nPackets::Sent::UpdateMana pk(this);

	pClient client = getClient();
	if (client) sendPacket(&pk);
	//! \todo when party updated, add a sendPacket for each party member in visual range
}

void checkFieldEffects(uint32_t currenttime, pChar pc, char timecheck );

void target_castSpell( pClient ps, pTarget t )
{
	TargetLocation TL( t );
	magic::castSpell( static_cast<magic::SpellId>(t->buffer[0]), TL, ps->currChar() );
}

/*!
\author Sparhawk
\remark Luxor: damage calculation adjusted.
*/
void cChar::checkPoisoning()
{
	if ( poisoned > 0 && !IsInvul() )
	{
		if ( TIMEOUT( poisontime ) )
		{
			if ( !TIMEOUT( poisonwearofftime ) )
			{
				switch ( poisoned )
				{
				case poisonWeak:
					poisontime= getclock() + ( 15 * SECS );
					// between 0% and 5% of player's hp reduced by racial combat poison resistance
					hp -= int32_t(
							qmax( ( ( hp ) * RandomNum( 0, 5 ) ) / 100, 3 ) *
							( (100 - Race::getPoisonResistance( race, poisonWeak ) ) / 100 )
						     );
					break;
				case poisonNormal:
					poisontime = getclock() + ( 10 * SECS );
					// between 5% and 10% of player's hp reduced by racial combat poison resistance
					hp -= int32_t(
							qmax( ( ( hp ) * RandomNum( 5, 10 ) ) / 100, 5 ) *
							( (100 - Race::getPoisonResistance( race, poisonNormal ) ) / 100 )
						      );
					break;
				case poisonGreater:
					poisontime = getclock()+( 10 * SECS );
					// between 10% and 15% of player's hp reduced by racial combat poison resistance
					hp -= int32_t(
							qmax( ( ( hp ) * RandomNum( 10,15 ) ) / 100, 7 ) *
							( (100 - Race::getPoisonResistance( race, poisonGreater ) ) / 100 )
						     );
					break;
				case poisonDeadly:
					poisontime = getclock() + ( 5 * SECS );
					// between 15% and 20% of player's hp reduced by racial combat poison resistance
					if ( hp <= (getStrength()/4) ) {
						stm = qmax( stm - 6, 0 );
						updateStamina();
					}
					hp -= int32_t(
							qmax( ( ( hp ) * RandomNum( 15, 20 ) ) / 100, 6) *
							( (100 - Race::getPoisonResistance( race, poisonDeadly ) ) / 100 )
						     );
					break;
				default:
					ErrOut("checkPoisoning switch fallout for char with serial [%u]\n", getSerial() );
					poisoned = poisonNone;
					return;
				}
				if ( hp < 1 )
				{
					Kill();
					if ( !npc )
						sysmsg("The poison has killed you.");
				}
				else
				{
					updateHp();
					if ( poisontxt <= getclock()  )
					{
						emotecolor = 0x0026;
						switch ( poisoned )
						{
						case poisonWeak:
							emoteall("* %s looks a bit nauseous *", true, getCurrentName().c_str() );
							break;
						case poisonNormal:
							emoteall("* %s looks disoriented and nauseous! *", true, getCurrentName().c_str());
							break;
						case poisonGreater:
							emoteall("* %s is in severe pain! *", true, getCurrentName().c_str());
							break;
						case poisonDeadly:
							emoteall("* %s looks extremely weak and is wrecked in pain! *", true, getCurrentName().c_str());
							break;
						}
						poisontxt = getclock() + ( 10 * SECS );
					}
				}
			}
			else
			{
				poisoned = poisonNone;
				getChar()->sendchar(this); // updating to blue stats-bar ...
				if ( !npc )
					sysmsg("The poison has worn off.");
			}
		}
	}
}

/*!
\brief Delete current speech
\author Endymion
\warning DELETE older speech string
*/
void cChar::deleteSpeechCurrent()
{
	if( speechCurrent!=NULL )
		safedelete(speechCurrent);
}

void cChar::setRegenRate( StatType stat, uint32_t rate, VarType type )
{
	if( stat>=ALL_STATS ) return;
	switch( type ) {
		case VAR_EFF:
			regens[stat].rate_eff=rate; break;
		case VAR_REAL:
			regens[stat].rate_real=rate; break;
		default:
			break; //error here?
	}
}

uint32_t cChar::getRegenRate( StatType stat, VarType type )
{
	if( stat>=ALL_STATS ) return 0;
	switch( type ) {
		case VAR_EFF:
			return regens[stat].rate_eff;
		case VAR_REAL:
			return regens[stat].rate_real;
		default:
			return 0; //error here?
	}
}

bool cChar::regenTimerOk( StatType stat )
{
	if( stat>=ALL_STATS ) return false;
	return TIMEOUT( regens[stat].timer );
}

void cChar::updateRegenTimer( StatType stat )
{
	if( stat>=ALL_STATS ) return;
	regens[stat].timer= getclock()+ regens[stat].rate_eff*SECS;
}

/*!
\author Luxor
\brief checks a skill for success (with sparring check)
\return true if success
\param sk skill
\param low low bound
\param high high bound
\param defend defending player
*/
bool cChar::checkSkillSparrCheck(Skill sk, int32_t low, int32_t high, pChar defend)
{
	bool bRaise = false;

	if (dynamc_cast<pNPC>(defend))
		bRaise = true;

	if (nSettings::Skills::getLimitPlayerSparring() == 0)
		bRaise = true;

	if (defend->skill[sk] > (skill[sk] + nSettings::Skills::getLimitPlayerSparring()))
		bRaise = true;

	return checkSkill(sk, low, high, bRaise);
}

/*!
\brief Uses an hair dye bottle
\author Flameeyes
\param bottle the Hair Dye bottle used
*/
void cChar::useHairDye(pItem bottle)
{
	if ( ! bottle || ! body ) return;

	pItem beard = body->getLayerItem(layBeard),
		hair = body->getLayerItem(layHair);

	if ( beard )
		beard->setColor( bottle->getColor() );
	if ( hair )
		hair->setColor( bottle->getColor() );

	bottle->Delete();	//Now delete the hair dye bottle!
}

/*!
\brief The char is in a dungeon?
\todo WRONG!!!! If you change the map, this will be totally wrong!
*/
const bool cChar::inDungeon() const
{
	if ( getPosition().x < 5119)
		return 0;

	int x1 = (getPosition().x - 5119) >> 8;
	int y1 = (getPosition().y >> 8);

	switch (y1)
	{
		case 5:
		case 0:
			return true;
		case 1:
			return x != 0;
		case 2:
		case 3:
			return x1 < 3;
		case 4:
		case 6:
			return x1 == 0;
		case 7:
			return x1 < 2;
	}
	return false;
}

//! Only for switching to combat mode
void cChar::warUpdate()
{
	bool sendit;

	sLocation charpos= getPosition();

	if (!inWarMode())
	// we have to execute this no matter if invisble or not LB
	{
		attacker = NULL;
		target = NULL;
	}

	bool hidden = (IsHidden() || (isDead() && !inWarMode()));
	const bool npc = rtti() == rtti::cNPC;

	NxwSocketWrapper sw;
	sw.fillOnline( pc_s, false );
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient cl = NULL; //! \todo TODO!! The client sets!
		if( ! cl ) continue;

		pPC pc_i = cl->currChar();
		if ( ! pc_i ) continue;

		if ( hidden && this != pc_i && !pc_i->isGM() )
		{
			if (!pc_i->isDead())
			{
				nPackets::Sent::DeleteObj pk(pc_i->getSerial());
		                ps_i->sendPacket(&pk);
				sendit = false;
			}
			else
				sendit = true;
		}
		else
			sendit = true; // LB 9-12-99 client 1.26.2 fix

		if (sendit)
		{
			uint8_t ndir = dir&0x7f, flag, hi_color, guild;

			// running stuff

			if (npc && (inWarMode() || ftarget)) // Skyfire
				ndir |= 0x80;

			flag =  inWarMode() ? 0x40 : 0 |
				isHidden() ? 0x80 : 0 |
				isPoisoned() ? 0x04 : 0;

			if (kills >= 4)
				hi_color = 6; // ripper

			guild = Guilds->Compare(pc_s, pc_i);
			if (guild == 1)        // Same guild (Green)
				hi_color = 2;
			else if (guild == 2)   // Enemy guild.. set to orange
				hi_color = 5;
			else if (isMurderer()) // show red
				hi_color = 6;
			else if (isInnocent()) // show blue
				hi_color = 1;
			else if (flag == 0x08) // show green
				hi_color = 2;
			else if (flag == 0x10) // show orange
				hi_color = 5;
			else
				hi_color = 3;  // show grey

			// end of if sendit
			nPackets::Sent::UpdatePlayer pk(this, ndir, flag, hi_color );
			ps_i->sendPacket(&pk);
		}
	}
}

NotEquippableReason cChar::canEquip(pEquippable pi)
{
	if (!pi) return nerNotEquippableItem;
        if (isDead()) return nerCharDead;	//Dead people cannot equip anything
	body = getBody();
	if (!body->isHumanBody()) return nerNotHumanBody;
        sint16_t st,dx,in,sk1,sk2,sk3;

	if ( (body->getId() == bodyMale) && ( pi->getId()==0x1c00 || pi->getId()==0x1c02 || pi->getId()==0x1c04 || pi->getId()==0x1c06 || pi->getId()==0x1c08 || pi->getId()==0x1c0a || pi->getId()==0x1c0c ) ) // Ripper...so males cant wear female armor
		return nerMaleEquippingFemaleArmor;

	// Minimum stats & skill check
	if ( isGM() || pi->isNewbie()) //If GM or newbie item it can be worn even if stats are normally too low
	{
		st = dx = in = sk1 = sk2 = sk3 = 0;
	}
	else
	{
		st = body->getStrength() - pi->getMinEquippingStrength();
		dx = body->getDexterity() - pi->getMinEquippingDexterity();
		in = body->getIntelligence() - pi->getMinEquippingIntelligence();
		sk1 = (pi->getMinEquippingSkill1() != UINVALID16) ? (body->getSkill(pi->getMinEquippingSkill1()) - p1->getMinEquippingSkillValue1()) /10 : 0;
		sk2 = (pi->getMinEquippingSkill2() != UINVALID16) ? (body->getSkill(pi->getMinEquippingSkill2()) - p1->getMinEquippingSkillValue2()) /10 : 0;
		sk3 = (pi->getMinEquippingSkill3() != UINVALID16) ? (body->getSkill(pi->getMinEquippingSkill3()) - p1->getMinEquippingSkillValue3()) /10 : 0;
	}
	if (st>=0 && dx>=0 && in>=0 && sk1>=0 && sk2>=0 && sk3>=0) return nerEquipOK;
	if (st<=dx  &&  st<=in && st<=sk1 && st<=sk2  && st<=sk3 ) return nerInsufficientStrength;
        if (dx<=st  &&  dx<=in && dx<=sk1 && dx<=sk2  && dx<=sk3 ) return nerInsufficientDexterity;
	if (in<=st  &&  in<=dx && in<=sk1 && in<=sk2  && in<=sk3 ) return nerInsufficientIntelligence;
	if (sk1<=st && sk1<=dx && sk1<=in && sk1<=sk2 && sk1<=sk3) return nerInsufficientSkill1;
	if (sk2<=st && sk2<=dx && sk2<=in && sk2<=sk1 && sk2<=sk3) return nerInsufficientSkill2;
	if (sk3<=st && sk3<=dx && sk3<=in && sk3<=sk1 && sk3<=sk2) return nerInsufficientSkill3;

	return nerUnknown;	//should never arrive here
}

NotEquippableReason cChar::canEquip(pItem pi)
{
	pEquippable ei = dynamic_cast<pEquippable> pi;
	if (ei) return canEquip(ei);
	return nerNotEquippableItem;
}

/*!
\brief Sets the character criminal, grey or nothing depending on the given mode
\param mode Mode to set the character
*/
void cChar::setCrimGrey(SuspectAction mode)
{
	switch(mode)
	{
		case saNormal:
			return;
		case saCriminal:
			makeCriminal();
			return;
		case saGrey:
			setGrey();
			return;
	}
}


void cChar::dyeChar(pClient client, uint16_t color)
{

	pc = client->currChar();
	if( pc->IsGMorCounselor() )
	{
		bodyid = getBody()->getId();

		if( color < 0x8000  && bodyid >= bodyMale && bodyid <= bodyFemaleDead ) color |= 0x8000; // why 0x8000 ?! ^^;

		if ((color & 0x4000) && (bodyid >= bodyMale && bodyid<= 0x03E1)) color = 0xF000; // but assigning the only "transparent" value that works, namly semi-trasnparency.

		if (color != 0x8000)
		{
			setColor(color);
			setOldColor(color);
			teleport( teleNone );
			client->playSFX(0x023E);
		}
	}
}
