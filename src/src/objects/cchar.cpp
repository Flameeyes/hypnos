/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cChar class
*/

#include "nxwcommn.h"
#include "itemid.h"
#include "sregions.h"
#include "sndpkg.h"
#include "magic.h"
#include "debug.h"
#include "amx/amxcback.h"
#include "set.h"
#include "tmpeff.h"
#include "race.h"
#include "npcai.h"
#include "layer.h"
#include "network.h"
#include "set.h"
#include "accounts.h"
#include "jail.h"
#include "nxw_utils.h"
#include "weight.h"
#include "boats.h"
#include "archive.h"
#include "walking.h"
#include "rcvpkg.h"
#include "map.h"
#include "chars.h"
#include "items.h"
#include "inlines.h"
#include "basics.h"
#include "magic.h"
#include "skills.h"
#include "range.h"
#include "classes.h"
#include "utils.h"
#include "nox-wizard.h"
#include "targeting.h"
#include "cmds.h"


void cChar::archive()
{
	std::string saveFileName( SrvParms->savePath + SrvParms->characterWorldfile + SrvParms->worldfileExtension );
	std::string timeNow( getNoXDate() );
	for( int32_t i = timeNow.length() - 1; i >= 0; --i )
		switch( timeNow[i] )
		{
			case '/' :
			case ' ' :
			case ':' :
				timeNow[i]= '-';
		}
	std::string archiveFileName( SrvParms->archivePath + SrvParms->characterWorldfile + timeNow + SrvParms->worldfileExtension );


	if( rename( saveFileName.c_str(), archiveFileName.c_str() ) != 0 )
	{
		LogWarning("Could not rename/move file '%s' to '%s'\n", saveFileName.c_str(), archiveFileName.c_str() );
	}
	else
	{
		InfoOut("Renamed/moved file '%s' to '%s'\n", saveFileName.c_str(), archiveFileName.c_str() );
	}
}

void cChar::safeoldsave()
{
	std::string oldFileName( SrvParms->savePath + SrvParms->characterWorldfile + SrvParms->worldfileExtension );
	std::string newFileName( SrvParms->savePath + SrvParms->characterWorldfile + SrvParms->worldfileExtension + "$" );
	remove( newFileName.c_str() );
	rename( oldFileName.c_str(), newFileName.c_str() );
}

cChar::cChar( SERIAL ser ) : cObject()
{

	m_client = NULL;

	setSerial32(ser);

	setMultiSerial32Only(INVALID);//Multi serial
	setOwnerSerial32Only(INVALID);


	setCurrentName("<this is a bug>");
	setRealName("<this is a bug>");
	title[0]=0x00;


	///TIMERS
	antiguardstimer=uiCurrentTime;
	antispamtimer=uiCurrentTime;
	begging_timer=uiCurrentTime;
	fishingtimer=uiCurrentTime;
	hungertime=uiCurrentTime;
	invistimeout=uiCurrentTime;
	nextact=uiCurrentTime;
	nextAiCheck=uiCurrentTime;
	npcmovetime=uiCurrentTime;
	skilldelay=uiCurrentTime;
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

	setId( BODY_MALE );
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
	setOwnerSerial32Only(INVALID);
	tamed = false; // True if NPC is tamed
	robe = INVALID; // Serial number of generated death robe (If char is a ghost)
	SetKarma(0);
	fame=0;
	//pathnum = PATHNUM;
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
	nextAiCheck=uiCurrentTime;

	npcmovetime=uiCurrentTime; // Next time npc will walk
	npcWander=WANDER_NOMOVE; // NPC Wander Mode
	fleeTimer=INVALID;
	oldnpcWander=WANDER_NOMOVE; // Used for fleeing npcs
	ftargserial=INVALID; // NPC Follow Target
	fx1=-1; //NPC Wander Point 1 x
	fx2=-1; //NPC Wander Point 2 x
	fy1=-1; //NPC Wander Point 1 y
	fy2=-1; //NPC Wander Point 2 y
	fz1=0; //NPC Wander Point 1 z

	spawnserial=INVALID; // Spawned by
	hidden=UNHIDDEN;
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
	poisoned=POISON_NONE; // type of poison
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
	spell=magic::SPELL_INVALID; //current spell they are casting....
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
	incognito=false;//incognito - AntiChrist
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
	resetNxwFlags();
	resetAmxEvents();
	ResetGuildTraitor();
	SetGuildType( INVALID );
	magicsphere = 0;
	resetResists();
	lightdamaged = false;
	holydamaged = false;
	damagetype = DAMAGE_BLUDGEON;
	fstm=0.0f;

	setGuild( NULL, NULL );

	jailed=false;
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

	commandLevel=0;
	// initializing crypt
#ifdef ENCRYPTION
	crypter = NULL;
#endif
	// initializing amx
	resetAmxEvents();
}

/*
\brief Destructor
*/
cChar::~cChar()
{
	if( staticProfile!=NULL )
		safedelete( staticProfile );
	if( speechCurrent!=NULL )
		safedelete( speechCurrent );
	if ( path != NULL )
		safedelete( path );
#ifdef ENCRYPTION
	if ( crypter != NULL )
		safedelete(crypter);
#endif
}


/*!
\note Don't add onstart events in npc scripts, because then they'll also be executed when character is created
add onstart event to character programmatically
*/
void cChar::loadEventFromScript(TEXT *script1, TEXT *script2)
{

#define CASECHAREVENT( NAME, ID ) 	else if (!strcmp( NAME,script1))	amxevents[ID] = newAmxEvent(script2);


	if (!strcmp("@ONSTART",script1))	{
		amxevents[EVENT_CHR_ONSTART] = newAmxEvent(script2);
		newAmxEvent(script2)->Call(getSerial32(), INVALID);
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
	else if (!strcmp("@ONCREATION",script1)) 	newAmxEvent(script2)->Call(getSerial32(), INVALID);
}

void cChar::getPopupHelp(char *str)
{
	if (!npc) {
		sprintf(str, TRANSLATE("He/She is a player like you. You've met new people!!"));

		if (IsCounselor()) {
			sprintf(str, TRANSLATE("He/She is a Counselor. You can ask him if you need help on the game"));
		}

		if (IsGM()) {
			sprintf(str, TRANSLATE("He/She is a Game Master. You can ask his help if you're stuck or have bugs or other problems"));
		}
	}
	else {
		if (npcaitype==NPCAI_PLAYERVENDOR)
			sprintf(str, TRANSLATE("He/She is a vendor which sells items owned by a player. Good shopping!"));
		else if (npcaitype==NPCAI_EVIL)
			sprintf(str, TRANSLATE("Run AWAY!!! It will kill you!!"));
	}
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
	P_ITEM pi;
	si.fillItemWeared( this, true, true, false );
	for( si.rewind(); !si.isEmpty(); si++ ) {
		pi = si.getItem();
		if ( !ISVALIDPI(pi) )
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
		if ( !ISVALIDPI(pi) )
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
		checkEquipement();
}

/*!
\brief get the bank box
\author Flameeyes
\param banktype BANK_GOLD or BANK_ITEM .. see note
\note BANK_GOLD are gold&items ( if specialbank onlygold not for only this region )
	BANK_ITEM (only if specialbank) are the bank region of player
\todo make sure that the bank exists
*/
P_ITEM cChar::getBankBox( uint8_t banktype )
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
*/
void cChar::disturbMed()
{
	if (!med) return; // no reason to stay here :]

	if (amxevents[EVENT_CHR_ONBREAKMEDITATION]) {
		g_bByPass = false;
		//<Luxor>
		pClient cli = getClient();
		if (cli != NULL)
  			amxevents[EVENT_CHR_ONBREAKMEDITATION]->Call( getSerial32() );
		//</Luxor>
		if (g_bByPass==true) return;
  	}

	/*
	//<Luxor>
	NXWCLIENT cli = getClient();
	if (cli != NULL)
	{
		g_bByPass = false;
		runAmxEvent( EVENT_CHR_ONBREAKMEDITATION, getSerial32(), cli->toInt(), INVALID );
		if (g_bByPass==true)
			return;
	}
	//</Luxor>
	*/
   	med=0;
   	sysmsg(TRANSLATE("You break your concentration."));
}

/*!
\brief Reveals the char if he was hidden
\author Duke
\date 17/03/2001
\date 20/03/2003 ported unhidesendchar into the function code - Flameeyes
*/
void cChar::unHide()
{
	//if hidden but not permanently or forced unhide requested
	if ( IsHiddenBySkill() && isPermaHidden() )
	{
		stealth=-1;
		hidden=UNHIDDEN;

		updateFlag();//AntiChrist - bugfix for highlight color not being updated

		SERIAL my_serial = getSerial32();
		Location my_pos = getPosition();

		NxwSocketWrapper sw;
		sw.fillOnline( this, false );
		for( sw.rewind(); !sw.isEmpty(); sw++ )
		{
			NXWSOCKET i = sw.getSocket();
			NXWCLIENT ps_i = sw.getClient();
			if( ps_i==NULL ) continue;

			P_CHAR pj=ps_i->currChar();
			if (ISVALIDPC(pj))
			{
				if (pj->getSerial32() != my_serial) { //to other players : recreate player object
					SendDeleteObjectPkt(i, my_serial);
					impowncreate(i, this, 0);
				} else {
					SendDrawGamePlayerPkt(i, my_serial, getId(), 0x00, getColor(), (poisoned ? 0x04 : 0x00), my_pos, 0x0000, dir|0x80);
				}
			}
		}

		if (IsGM())
			tempfx::add(this, this, tempfx::GM_UNHIDING, 3, 0, 0);
	}
}

/*!
\brief makes a character fight the other
\author Duke
\date 20/03/2001
*/
void cChar::fight(pChar other)
{
	VALIDATEPC(other);
	//if (!war) toggleCombat(); //Luxor
	targserial=other->getSerial32();
	unHide();
	disturbMed(); // Meditation
	attackerserial=other->getSerial32();
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
uint32_t cChar::CountItems(uint16_t matchId, uint16_t matchColor)
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

void cChar::MoveTo(Location newloc)
{
	// Avoid crash if go to 0,0
	if ((newloc.x < 1) || (newloc.y < 1))
		return;

	// <Luxor>
	if ( newloc != getPosition() && (flags & flagIsCasting) && !npc ) {
		sysmsg( "You stop casting the spell." );
		flags &= ~flagIsCasting;
		spell = magic::SPELL_INVALID;
		spelltime = 0;
	}
	// </Luxor>
#ifdef SPAR_C_LOCATION_MAP
	setPosition( newloc );
	pointers::updateLocationMap( this );
#else
	mapRegions->remove(this);
	setPosition( newloc );
	mapRegions->add( this );
#endif
}

/*!
\author Xanathar
\param pc_i helped character
\brief Called after helping a character for accomplish to criminals
*/
void cChar::helpStuff(pPC pc_i)
{
	if (! pc_i || this == pc_i )
		return;

	if (pc_i->IsGrey()) setCrimGrey (this, ServerScp::g_nHelpingGreyWillCriminal);

	if (pc_i->IsInnocent())
	{
		if ((pc_i->GetKarma()>0)&&((pc_i->GetKarma()-GetKarma())>100)) {
			IncreaseKarma(+5);
			sysmsg(TRANSLATE("You've gained a little karma."));
		}
		return;
	}

	if (pc_i->IsCriminal()) setCrimGrey (ServerScp::g_nHelpingCriminalWillCriminal);

	if (pc_i->IsMurderer()) makeCriminal();
}

/*!
\brief applies a poison to a char
\author Xanathar, modified by Endymion
\param poisontype the poison
\param secs the duration of poison ( if INVALID ( default ) default duration is used )
*/
void cChar::applyPoison(PoisonType poisontype, int32_t secs )
{
        unfreeze();
	if ( !IsInvul() && (::region[region].priv&0x40)) // LB magic-region change
	{
		if (poisontype>POISON_DEADLY) poisontype = POISON_DEADLY;
		else if (poisontype<POISON_WEAK) poisontype = POISON_WEAK;
		if ( poisontype>=poisoned ) {
			poisoned=poisontype;
			if( secs == INVALID )
				poisonwearofftime=uiCurrentTime+(MY_CLOCKS_PER_SEC*SrvParms->poisontimer); // lb
			else
				poisonwearofftime=uiCurrentTime+(MY_CLOCKS_PER_SEC*secs);

			NXWSOCKET s = getSocket();
			if (s != -1) impowncreate(s, this, 1); //Lb, sends the green bar !
			sysmsg( TRANSLATE("You have been poisoned!"));
			playSFX( 0x0246 ); //poison sound - SpaceDog
		}
	}
}

/*!
\author Xanathar
\brief unfreezes the char
*/
void cChar::unfreeze( bool calledByTempfx )
{
	if( !calledByTempfx )
		delTempfx( tempfx::SPELL_PARALYZE, false ); //Luxor

	if ( isFrozen() )
	{
		setFrozen(false);
		if (!isCasting()) //Luxor
			sysmsg(TRANSLATE("You are no longer frozen."));
	}
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
	P_CHAR myself=pointers::findCharBySerial(getSerial32());
	if ( ! ISVALIDPC(myself) )
		return;
	P_CHAR pc_att=pointers::findCharBySerial(attackerserial);
	SERIAL serial_att= ISVALIDPC(pc_att)? pc_att->getSerial32() : INVALID;

	if (amxevents[EVENT_CHR_ONWOUNDED]) {
		g_bByPass = false;
		amount = amxevents[EVENT_CHR_ONWOUNDED]->Call(getSerial32(), amount, serial_att);
		if (g_bByPass==true) return;
	}
	/*
	if ( getAmxEvent(EVENT_CHR_ONWOUNDED) != NULL ) {
		amount = runAmxEvent( EVENT_CHR_ONWOUNDED, getSerial32(), amount, serial_att );
		if (g_bByPass==true)
			return;
	}
	*/
	unfreeze();

	if (amount <= 0) return;
	// typeofdamage is ignored till now
    if (typeofdamage!=DAMAGE_PURE) {
    	amount -= int32_t((amount/100.0)*float(calcResist(typeofdamage)));
    }
	if (amount <= 0) return;

	switch (stattobedamaged)
	{
		case STAT_MANA:
			mn = qmax(0, mn - amount);
			updateStats(STAT_MANA);
			break;
		case STAT_STAMINA:
			stm = qmax(0, stm - amount);
			updateStats(STAT_STAMINA);
			break;
		case STAT_HP:
		default:
			hp = qmax(0, hp - amount);
			updateStats(STAT_HP);
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
	if (typeofdamage == DAMAGE_PURE || typeofdamage > MAX_RESISTANCE_INDEX)
		return 0;

	int32_t total = 0;
	total += resists[typeofdamage];

	NxwItemWrapper si;
	si.fillItemWeared( this, true, true, true );
	for( si.rewind(); !si.isEmpty(); si++ )
	{
		P_ITEM pi=si.getItem();
		if (ISVALIDPI(pi)) {
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
	if ( pi != 0 )
		pi->Refresh();
}

/*!
\brief Get char's distance from the given character
\author Luxor
\return distance ( if invalid is returned VERY_VERY_FAR )
\param pc the char
*/
uint32_t cChar::distFrom(P_CHAR pc)
{
	VALIDATEPCR(pc, VERY_VERY_FAR); //Endymion, fix: if not valid very far :P
	return (uint32_t)dist(getPosition(),pc->getPosition());
}

/*!
\brief Get char's distance from the given item
\author Endymion
\return distance ( if invalid is returned VERY_VERY_FAR )
\param pi the item
\note it check also if is subcontainer, or weared. so np call freely
*/
uint32_t cChar::distFrom(P_ITEM pi)
{
	VALIDATEPIR(pi, VERY_VERY_FAR);
	P_ITEM cont=pi->getOutMostCont(); //return at least itself
	VALIDATEPIR(cont, VERY_VERY_FAR);

	if(cont->isInWorld())
	{
		return (uint32_t)dist(getPosition(),cont->getPosition());
	}
	else
		if(isCharSerial(cont->getContSerial())) //can be weared
			return distFrom( pointers::findCharBySerial(cont->getContSerial()) );
		else
			return VERY_VERY_FAR; //not world, not weared.. and another cont can't be

}

/*!
\author Luxor
\brief Tells if the char can see the given object
*/
bool cChar::canSee( cObject &obj )
{
	//
	// Check if the object is in visRange
	//
	R64 distance = dist( obj.getPosition(), getPosition(), false );
	if ( distance > VISRANGE ) // We cannot see it!
		return false;

	SERIAL ser = obj.getSerial32();
	if ( isCharSerial( ser ) ) {
		P_CHAR pc = P_CHAR( &obj );
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

	/*if ( isItemSerial( ser ) ) { //Future use
	}*/
	return true;
}


/*!
\author Luxor
\brief Teleports character to its current set coordinates.
*/
void cChar::teleport( uint8_t flags, NXWCLIENT cli )
{


	P_ITEM p_boat = Boats->GetBoat(getPosition());
	if( ISVALIDPI(p_boat) ) {
		setMultiSerial(p_boat->getSerial32());
		Location boatpos = getPosition();
		boatpos.z = p_boat->getPosition().z +3;
		boatpos.dispz = p_boat->getPosition().dispz +3;
		setPosition( boatpos );
	} else
		setMultiSerial(INVALID);

	updateFlag();	//AntiChrist - Update highlight color

	NXWSOCKET socket = getSocket();

    //
    // Send the draw player packet
    //
	if ( socket != INVALID ) {
		uint8_t flag = 0x00;
		Location pos = getPosition();

		if( poisoned )
			flag |= 0x04;

		if ( IsHidden() )
			flag |= 0x80;

		SendDrawGamePlayerPkt(socket, getSerial32(), getId(), 0x00, getColor(), flag, pos, 0x0000, dir | 0x80, true);

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
			NXWCLIENT ps_w = sw.getClient();
			if ( ps_w == NULL )
				continue;
			P_CHAR pc = ps_w->currChar();
            if ( ISVALIDPC( pc ) )
	            if ( distFrom( pc ) > VISRANGE || !canSee( *pc ) )
					ps_w->sendRemoveObject(static_cast<P_OBJECT>(this));
		}
	} else
		cli->sendRemoveObject(static_cast<P_OBJECT>(this));

        //
        // Send worn items and the char itself to the char (if online) and other players
        //
	if ( cli == NULL ) {
		NxwSocketWrapper sw;
		sw.fillOnline( this, false );
		for ( sw.rewind(); !sw.isEmpty(); sw++ ) {
			NXWCLIENT ps_i = sw.getClient();
			if( ps_i != NULL ) {
				impowncreate( ps_i->toInt(), this, 1 );
				//ndEndy not too sure of this
				if ( flags&TELEFLAG_SENDWORNITEMS )
					wornitems( ps_i->toInt(), this );
			}
		}
		sw.clear();
	} else {
		impowncreate( cli->toInt(), this, 1 );
		if ( flags&TELEFLAG_SENDWORNITEMS )
			wornitems( cli->toInt(), this );
	}


    //
    // Send other players and items to char (if online)
    //
    if ( cli == NULL || cli == getClient() )
		if ( socket != INVALID )
		{
			if ( flags&TELEFLAG_SENDNEARCHARS )
			{
				NxwCharWrapper sc;
				sc.fillCharsNearXYZ( getPosition(), VISRANGE, IsGM() ? false : true );
				for( sc.rewind(); !sc.isEmpty(); sc++ )
				{
					P_CHAR pc=sc.getChar();
					if( ISVALIDPC( pc ) )
					{
						if( getSerial32() != pc->getSerial32() )
						{
							if ( !pc->IsOnline() && !pc->npc )
							{
								if ( seeForLastTime( *pc ))
									getClient()->sendRemoveObject( P_OBJECT(pc) );
							}
							else
							{
								seeForLastTime( *pc );
								seeForFirstTime( *pc );
								impowncreate( socket, pc, 1 );
							}
						}
					}
				}

				if ( flags&TELEFLAG_SENDNEARITEMS )
				{
					NxwItemWrapper si;
					si.fillItemsNearXYZ( getPosition(), VISRANGE, false );
					for( si.rewind(); !si.isEmpty(); si++ )
					{
						P_ITEM pi = si.getItem();
						if( ISVALIDPI( pi ) )
							senditem( socket, pi );
					}
				}
			}
		}

	//
	// Send the light level
	//
	if ( socket != INVALID && (flags&TELEFLAG_SENDLIGHT) )
		dolight( socket, worldcurlevel );

    //
    // Check if the region changed
    //
	checkregion( this );

	//
	// Send the weather
	//
	if( socket != INVALID && (flags&TELEFLAG_SENDWEATHER) )
		pweather(socket);
}

/*!
\author Luxor
\brief returns char's combat skill
\return the index of the char's combat skill
*/
int32_t cChar::getCombatSkill()
{

	NxwItemWrapper si;
	si.fillItemWeared( this, false, false, true );
	for( si.rewind(); !si.isEmpty(); si++ ) {

		P_ITEM pi = si.getItem();
		if( ISVALIDPI( pi ) )
		{
			if( pi->layer == LAYER_1HANDWEAPON || pi->layer == LAYER_2HANDWEAPON )
			{
				if (pi->fightskill != 0)
				{
					return pi->fightskill;
				}
				else if (pi->IsSwordType() )
				{
					return SWORDSMANSHIP;
				}
				else if (pi->IsMaceType() )
				{
					return MACEFIGHTING;
				}
				else if (pi->IsFencingType() )
				{
					return FENCING;
				}
				else if (pi->IsBowType() )
				{
					return ARCHERY;
				}
			}
		}
	}

	return WRESTLING;

}

bool const cChar::CanDoGestures() const
{
	if (!IsGM())
	{
		if (hidden == HIDDEN_BYSPELL) return false;	//Luxor: cannot do magic gestures if under invisible spell

		NxwItemWrapper si;
		si.fillItemWeared( (P_CHAR)this, false, false, true );
		for( si.rewind(); !si.isEmpty(); si++ ) {

			P_ITEM pj=si.getItem();

			if( ISVALIDPI( pj ) )
			{
				if ( pj->layer == LAYER_2HANDWEAPON || ( pj->layer == LAYER_1HANDWEAPON && pj->type!=ITYPE_SPELLBOOK ) )
				{
					if (!(pj->getId()==0x13F9 || pj->getId()==0x0E8A || pj->getId()==0x0DF0 || pj->getId()==0x0DF2
						|| pj->IsChaosOrOrderShield() ))
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}

/*!
\author Xanathar
\brief Checks a skill for success
\param sk skill
\param low low bound
\param high high bound
\param bRaise should be raised?
*/
bool cChar::checkSkill(Skill sk, int32_t low, int32_t high, bool bRaise)
{
	NXWCLIENT ps = getClient();;
	NXWSOCKET s=INVALID;

	if ( sk < 0 || sk > TRUESKILLS ) //Luxor
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
			if(Skills::AdvanceSkill(DEREF_P_CHAR(this), sk, skillused))
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

	cPacketSendDeleteObject pk(serial);

        NxwSocketWrapper sc;
        sc.fillOnline( this );
	for ( sc.rewind(); !sc.isEmpty(); sc++ ) {
		NXWCLIENT ps = sc.getClient();
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
	SERIAL objser = obj.getSerial32();

	//
        // The char cannot see itself for the first time ;)
        //
	if ( objser == getSerial32() )
		return false;

	//
	// Check if we can see it
	//
	if ( !canSee( obj ) )
		return false;

	//
	// Check if the object was sent before
	//
	SERIAL_SLIST::iterator it( find( sentObjects.begin(), sentObjects.end(), objser ) );

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
	SERIAL objser = obj.getSerial32();

        //
        // The char cannot see itself for the last time ;)
        //
	if ( objser == getSerial32() )
		return false;

	//
	// Check if we can see it
	//
	if ( canSee( obj ) )
		return false;

	//
	// Check if the object was sent before
	//
	SERIAL_SLIST::iterator it( find( sentObjects.begin(), sentObjects.end(), objser ) );

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
	P_CHAR pc_att=pointers::findCharBySerial(attackerserial);

	if ( ISVALIDPC(pc_att) && hasInRange(pc_att) )
    	{
    		if ( !npc )
        		sysmsg( TRANSLATE("You cannot hide while fighting.") );
        	return;
    	}

    	if ( IsHidden() )
    	{
    		if ( !npc )
        		sysmsg( TRANSLATE("You are already hidden") );
        	return;
    	}

    	if ( !checkSkill( HIDING, 0, 1000) )
    	{
    		if ( !npc )
        		sysmsg( TRANSLATE("You are unable to hide here.") );
        	return;
    	}

    	if ( IsGM() )
    	{
        	staticeffect( DEREF_P_CHAR(this), 0x37, 0x09, 0x09, 0x19);
        	playSFX( 0x0208 );
        	tempfx::add(this, this, tempfx::GM_HIDING, 1, 0, 0);
        	// immediate hiding overwrites the effect.
        	// so lets hide after 4 secs.
        	// 1 sec works fine now so changed to this.
        	return;
	}

	if ( !npc )
		sysmsg( TRANSLATE("You have hidden yourself well.") );

	hidden = HIDDEN_BYSKILL;
	teleport( TELEFLAG_NONE );
}

/*!
\author Xanathar & Luxor
\brief Hides a player by spell
*/
void cChar::hideBySpell(int32_t timer)
{
	if (timer == INVALID) timer = SrvParms->invisibiliytimer;
	tempfx::add(this, this, tempfx::SPELL_INVISIBILITY, 0,0,0, timer);
}

/*!
\brief Cures poison
\author Luxor
*/
void cChar::curePoison()
{
	poisoned = POISON_NONE;
	poisonwearofftime = uiCurrentTime;
	if (getClient() != NULL) impowncreate(getClient()->toInt(), this, 1);
}

/*!
\author Xanather
\brief Resurrects a char
\param healer Player that resurrected the char
*/
void cChar::resurrect( NXWCLIENT healer )
{
	NXWCLIENT ps=getClient();
	if( ps==NULL )
		return;

	if (dead)
	{
		dead=false;
		if(!npc || morphed)
			morph();
		hp= getStrength();
		stm= dx;
		mn=in;

		if (amxevents[EVENT_CHR_ONRESURRECT]) {
			g_bByPass = false;
			amxevents[EVENT_CHR_ONRESURRECT]->Call(getSerial32(), (healer!=NULL)? healer->currCharIdx() : INVALID );
			if (g_bByPass==true) return;
		}
		/*
		g_bByPass = false;
		runAmxEvent( EVENT_CHR_ONRESURRECT, getSerial32(), (healer!=NULL)? healer->toInt() : INVALID );
		if (g_bByPass==true)
			return;
		*/
		modifyFame(0);
		playSFX( 0x0214);
		setId( getOldId() );
		setColor( getOldColor() );
		attackerserial=INVALID;
		ResetAttackFirst();
		war=0;
		hunger=6;

		NxwItemWrapper si;
		si.fillItemWeared( this, false, false, true );
		for( si.rewind(); !si.isEmpty(); si++ )
		{
			P_ITEM pj=si.getItem();
			if(!ISVALIDPI(pj))
				continue;
			if( pj->layer == LAYER_NECK )
			{
				pj->layer = LAYER_BACKPACK;
				packitemserial=pj->getSerial32(); //Tauriel packitem speedup
			}
		}

		P_ITEM pj= pointers::findItemBySerial(robe);
		if( ISVALIDPI(pj))
			pj->Delete();

		P_ITEM pi = item::CreateFromScript( "$item_robe_1", this );
		if(ISVALIDPI(pi)) {
			pi->setCurrentName( "a resurrect robe" );
			pi->layer = LAYER_OUTER_TORSO;
			pi->setContainer(this);
			pi->dye=1;
		}
		teleport( TELEFLAG_SENDWORNITEMS | TELEFLAG_SENDLIGHT );
	}
		else
			if( healer!=NULL )
				healer->sysmsg( TRANSLATE("That person isn't dead") );
}

/*!
\author Xanathar
\brief Sets owner fields
\param owner new owner
*/
void cChar::setOwner(P_CHAR owner)
{
	setOwnerSerial32(owner->getSerial32());
	npcWander=WANDER_NOMOVE;
	tamed = true;
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

	P_ITEM phair = getHairItem();
	P_ITEM pbeard = getBeardItem();

	if (bBackup)
	{
		setOldId( getId() );
		setOldColor( getColor() );

		setRealName( getCurrentNameC() );
		if(ISVALIDPI(pbeard))
		{
			oldbeardstyle = pbeard->getId();
			oldbeardcolor = pbeard->getColor();
		}
		if(ISVALIDPI(phair))
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

	if(ISVALIDPI(pbeard))
	{
		if (beardstyle!=INVALID)
			pbeard->setId( beardstyle );
		if (beardcolor!=INVALID)
			pbeard->setColor( beardcolor );
	}

	if(ISVALIDPI(phair))
	{
		if (hairstyle!=INVALID)
			phair->setId( hairstyle );
		if (haircolor!=INVALID)
			phair->setColor( haircolor );
	}

	morphed = bBackup;

	teleport( TELEFLAG_SENDWORNITEMS );

}

/*!
\brief shifts a player in the body of a npc, usually used only by GMs
\author Luxor
\param pc the character to possess
*/
void cChar::possess(P_CHAR pc)
{
	if ( !IsOnline() )
		return;

	VALIDATEPC(pc);
	bool bSwitchBack = false;

	NXWSOCKET socket = getSocket();
	if ( socket == INVALID )
		return;

	if ( possessorSerial != INVALID ) { //We're in a possessed Char! Switch back to possessor
		P_CHAR pcPossessor = pointers::findCharBySerial( possessorSerial );
		if ( ISVALIDPC( pcPossessor ) ) {
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
		pc->possessorSerial = getSerial32();
		possessedSerial = pc->getSerial32();
	}

	//Network related stuff
	( bSwitchBack ) ? npc = 1 : pc->npc = 0;
	currchar[ socket ] = pc->getSerial32();
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
    int32_t xx,yy,zz;
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

	pContainer sb = reinterpret_cast<pContainer>(body->getBackpack()->findFirstType(ITYPE_SPELLBOOK));

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

	NXWCLIENT ps=getClient();

	char murderername[128];
	murderername[0] = '\0';

	if (amxevents[EVENT_CHR_ONBEFOREDEATH]) {
		g_bByPass = false;
		amxevents[EVENT_CHR_ONBEFOREDEATH]->Call(getSerial32(), INVALID);
		if (g_bByPass==true) return;
	}

	/*
	g_bByPass = false;
	runAmxEvent( EVENT_CHR_ONDEATH, getSerial32(), s );
	if (g_bByPass==true)
		return;
	*/
	if ( ps != NULL )
		unmountHorse();	//Luxor bug fix
	if (morphed)
		morph();

	dead = true;
	poisoned = POISON_NONE;
	poison = hp = 0;

	if( getOldId() == BODY_FEMALE)
	{
		switch(RandomNum(0, 3)) // AntiChrist - uses all the sound effects
		{
			case 0:	playSFX( 0x0150 ); break;// Female Death
			case 1:	playSFX( 0x0151 ); break;// Female Death
			case 2:	playSFX( 0x0152 ); break;// Female Death
			case 3:	playSFX( 0x0153 ); break;// Female Death
		}
	}
	else if ( getOldId()  == BODY_MALE)
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
		teleport( TELEFLAG_SENDWORNITEMS );
	}

	murdererSer = INVALID;

	//--------------------- reputation stuff
#ifdef SPAR_C_LOCATION_MAP
	PCHAR_VECTOR *pCV = pointers::getNearbyChars( this, VISRANGE*2, pointers::COMBATTARGET );
	PCHAR_VECTOR it( pCV->begin() ), end( pCV->end() );
	P_CHAR pKiller = 0;
	while( it != end )
	{
		pKiller = (*it);
		if( pKiller->npcaitype==NPCAI_TELEPORTGUARD )
		{
			pKiller->summontimer=(uiCurrentTime+(MY_CLOCKS_PER_SEC*20));
			pKiller->npcWander=WANDER_FREELY_CIRCLE;
			pKiller->setNpcMoveTime();
			pKiller->talkAll(TRANSLATE("Thou have suffered thy punishment, scoundrel."),0);
		}

		pKiller->targserial=INVALID;
		pKiller->timeout=0;

		P_CHAR pk_att = pointers::findCharBySerial( pKiller->attackerserial );
		if ( pk_att )
		{
			pk_att->ResetAttackFirst();
			pk_att->attackerserial = INVALID;
		}

		pKiller->attackerserial = INVALID;
		pKiller->ResetAttackFirst();

		if( pKiller->attackerserial == getSerial32() )
		{
			pKiller->attackerserial = INVALID;
			pKiller->ResetAttackFirst();
		}

		if( !pKiller->npc )
		{
			strncpy(murderername, pKiller->getCurrentNameC(), 48);

			if( pKiller->party != INVALID )
			{
				PCHAR_VECTOR *pcvParty = pointers::getNearbyChars( pKiller, VISRANGE, pointers::PARTYMEMBER );
				PCHAR_VECTOR partyIt( pcvParty->begin() ), partyEnd( pcvParty->end() );
				P_CHAR pMember = 0;
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
					murdererSer = pKiller->getSerial32();
					++pKiller->kills;
					pKiller->sysmsg(TRANSLATE("You have killed %i innocent people."), pKiller->kills);

					if (pKiller->kills==(unsigned)repsys.maxkills)
						pKiller->sysmsg(TRANSLATE("You are now a murderer!"));
					pKiller->updateFlag();

				if (SrvParms->pvp_log)
				{
						LogFile pvplog("PvP.log");
						pvplog.Write("%s was killed by %s!\n",getCurrentNameC(), pKiller->getCurrentNameC());
				}
			}   // was innocent

			if (pKiller->amxevents[EVENT_CHR_ONKILL])
				pKiller->amxevents[EVENT_CHR_ONKILL]->Call( pKiller->getSerial32(), getSerial32() );

				//pk->runAmxEvent( EVENT_CHR_ONKILL, pk->getSerial32(), pk->getClient()->toInt(), getSerial32(), s);
			} //PvP
		}//if !npc
		else
		{
			if (pKiller->amxevents[EVENT_CHR_ONKILL])
				pKiller->amxevents[EVENT_CHR_ONKILL]->Call( pKiller->getSerial32(), getSerial32() );
			if (pKiller->war)
				pKiller->toggleCombat(); // ripper
		}
		++it;
	}
#else
	P_CHAR pk = MAKE_CHAR_REF(0);

	NxwCharWrapper sc;
	sc.fillCharsNearXYZ( getPosition(), VISRANGE*2, false );
	for( sc.rewind(); !sc.isEmpty(); sc++ )
	{
		pk = sc.getChar();
		if(!ISVALIDPC(pk) || pk->targserial!=getSerial32() )
			continue;

		if (pk->npcaitype==NPCAI_TELEPORTGUARD)
		{
			pk->summontimer=(uiCurrentTime+(MY_CLOCKS_PER_SEC*20));
			pk->npcWander=WANDER_FREELY_CIRCLE;
			pk->setNpcMoveTime();
			pk->talkAll(TRANSLATE("Thou have suffered thy punishment, scoundrel."),0);
		}

		pk->targserial=INVALID;
		pk->timeout=0;

		P_CHAR pk_att=pointers::findCharBySerial(pk->attackerserial);
		if (ISVALIDPC(pk_att))
		{
			pk_att->ResetAttackFirst();
			pk_att->attackerserial=INVALID; // lb crashfix
		}

		pk->attackerserial=INVALID;
		pk->ResetAttackFirst();

		if (!pk->npc)
		{
			strncpy(murderername, pk->getCurrentNameC(), 48);

			NxwCharWrapper party;
			party.fillPartyFriend( pk, VISRANGE, true );
			for( party.rewind(); !party.isEmpty(); party++ )
			{
				P_CHAR fr=party.getChar();
				if( ISVALIDPC(fr) )
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
					murdererSer = pk->getSerial32();
					pk->kills++;
					pk->sysmsg(TRANSLATE("You have killed %i innocent people."), pk->kills);

					if (pk->kills==(unsigned)repsys.maxkills)
						pk->sysmsg(TRANSLATE("You are now a murderer!"));
					pk->updateFlag();

				if (SrvParms->pvp_log)
				{
						LogFile pvplog("PvP.log");
						pvplog.Write("%s was killed by %s!\n",getCurrentNameC(), pk->getCurrentNameC());
				}
			}   // was innocent

			if (pk->amxevents[EVENT_CHR_ONKILL])
				pk->amxevents[EVENT_CHR_ONKILL]->Call( pk->getSerial32(), getSerial32() );

				//pk->runAmxEvent( EVENT_CHR_ONKILL, pk->getSerial32(), pk->getClient()->toInt(), getSerial32(), s);
			} //PvP
		}//if !npc
		else
		{
			if (pk->amxevents[EVENT_CHR_ONKILL])
				pk->amxevents[EVENT_CHR_ONKILL]->Call( pk->getSerial32(), getSerial32() );
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

		P_ITEM pi_j=weared.getItem();

		if(!ISVALIDPI(pi_j))
			continue;

		if ((pi_j->type==ITYPE_CONTAINER) && (pi_j->getPosition().x==26) && (pi_j->getPosition().y==0) &&
			(pi_j->getPosition().z==0) && (pi_j->getId()==0x1E5E) )
		{
			endtrade(pi_j->getSerial32());
		}
	}

	//--------------------- corpse & ghost stuff

	bool hadHumanBody=HasHumanBody();
	int32_t corpseid = (getId() == BODY_FEMALE)? BODY_DEADFEMALE : BODY_DEADMALE;

	if( ps!=NULL )
		morph( corpseid, 0, 0, 0, 0, 0, NULL, true);

	if (!npc)
	{
		pItem pDeathRobe = cItem::addByID( ITEMID_DEATHSHROUD, 1, "a death shroud", 0, getPosition());
        	if (pDeathRobe)
		{
			robe = pDeathRobe->getSerial32();
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
	sprintf(szCorpseName, "corpse of %s", getCurrentNameC());

	pItem pCorpse = cItem::addByID( ITEMID_CORPSEBASE, 1, szCorpseName, getOldColor(), getPosition());
	if (!ISVALIDPI(pCorpse))
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
		pCorpse->setOwnerSerial32(getSerial32());
		pCorpse->more4 = char( SrvParms->playercorpsedecaymultiplier&0xff ); // how many times longer for the player's corpse to decay
	}

	pCorpse->amount = getOldId(); // Amount == corpse type
	pCorpse->morey = hadHumanBody;

	pCorpse->carve=carve;               //store carve section - AntiChrist

	pCorpse->setSecondaryName(getCurrentNameC());

	pCorpse->type = ITYPE_CONTAINER;

	pCorpse->more1 = 0;
	pCorpse->dir = dir;
	pCorpse->corpse = 1;
	pCorpse->setDecayTime();

	pCorpse->murderer = string(murderername);
	pCorpse->murdertime = uiCurrentTime;
	pCorpse->Refresh();


	//--------------------- dropping items to corpse

	P_ITEM pBackPack = getBackpack();
	if (!ISVALIDPI(pBackPack))
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

		P_ITEM pi_j=si.getItem();
		if( !ISVALIDPI(pi_j) )
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
		Location lj = pi_j->getPosition();
		lj.y = RandomNum(85,160);
		lj.x = RandomNum(20,70);
		pi_j->setPosition( lj );
		pi_j->Refresh();
		//General Lee
	}

        if ( !npc )
		teleport( TELEFLAG_SENDWORNITEMS );

	pCorpse->Refresh();

	if (amxevents[EVENT_CHR_ONAFTERDEATH])
	{
		g_bByPass = false;
		amxevents[EVENT_CHR_ONAFTERDEATH]->Call(getSerial32(), pCorpse->getSerial32() );
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
//	npcmovetime = uiCurrentTime;
	if ( npcWander == WANDER_FOLLOW )
		npcmovetime = uint32_t( uiCurrentTime + ( float( npcFollowSpeed * MY_CLOCKS_PER_SEC ) ) );
	else
		npcmovetime = uint32_t( uiCurrentTime + ( float( npcMoveSpeed * MY_CLOCKS_PER_SEC ) ) );
}

/*!
\brief unequip items that you can nop longer wear/use
\author LB
*/
void cChar::checkEquipement()
{
	char temp2[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	P_ITEM pi;

	if (npc) return;

	Location charpos = getPosition();

	NxwItemWrapper si;
	si.fillItemWeared( this, false, false, true );
	for( si.rewind(); !si.isEmpty(); si++ ) {

		pi=si.getItem();
		if(!ISVALIDPI(pi))
			continue;
		if (((pi->st > getStrength()) || !pi->checkItemUsability(this, ITEM_USE_CHECKEQUIP)) && !pi->isNewbie())//if strength required > character's strength, and the item is not newbie
		{
			if( strncmp(pi->getCurrentNameC(), "#", 1) )
				pi->getName(temp2);
			else
				strcpy(temp2,pi->getCurrentNameC());

			if( pi->st > getStrength()) sysmsg(TRANSLATE("You are not strong enough to keep %s equipped!"), temp2);
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
				NXWCLIENT ps=sw.getClient();
				if(ps!=NULL ) {
					wornitems(ps->toInt(), this );
					senditem(ps->toInt(), pi);
				}
			}
		}
	}
}

void cChar::showLongName( P_CHAR showToWho, bool showSerials )
{
	VALIDATEPC( showToWho );
	NXWSOCKET socket = showToWho->getSocket();
	if (socket < 0 || socket > now) return;

	char temp[TEMP_STR_SIZE];
 	char temp1[TEMP_STR_SIZE];

	*(temp1)='\0';

	if( fame >= 10000 )
	{ // adding Lord/Lady to title overhead
		switch ( getId() )
		{
			case BODY_FEMALE :
				if ( strcmp(::title[9].other,"") )
				{
					sprintf(temp,"%s ",::title[9].other);
					strcat(temp1,temp);
				}
				break;
			case BODY_MALE	:
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
		sprintf( temp, "%s (%s)",::title[8].other, getCurrentNameC());
		strcat( temp1, temp );
	}
	else
		strcat( temp1, getCurrentNameC() );

	if ( showToWho->CanSeeSerials() || showSerials )
	{
		sprintf( temp, " [%x]", getSerial32() );
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
	if (tamed && npcaitype==NPCAI_PETGUARD && getOwnerSerial32()==showToWho->getSerial32() && showToWho->guarded)
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

	Guilds->Title( socket, DEREF_P_CHAR(this) );

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

	SendSpeechMessagePkt(socket, getSerial32(), 0x0101, 6, color, 0x0003, sysname, temp1);
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
	if( this->regenTimerOk( STAT_HP ) ) {
		if (hp < getStrength() && (hunger > 3 || SrvParms->hungerrate == 0)) {
			hp++;
			update[ 0 ] = true;
		}

		this->updateRegenTimer( STAT_HP );
	}

	//STAMINA REGEN
	if( this->regenTimerOk( STAT_STAMINA )) {
		if (stm < dx) {
			stm++;
			update[ 2 ] = true;
		}

		this->updateRegenTimer( STAT_STAMINA );
	}

	//MANA REGEN
	if( this->regenTimerOk( STAT_MANA ) )
	{
		if (mn < in)
		{
			mn += 3;
			update[ 1 ] = true;
		}
		else if (med)
		{
			if( !npc )
				sysmsg(TRANSLATE("You are at peace."));
			med = 0;
		}

		uint32_t manarate = this->getRegenRate( STAT_MANA, VAR_REAL );
		if(SrvParms->armoraffectmana)
		{
			if (med)
				manarate += uint32_t( calcDef(0) / 10.0 ) - uint32_t( skill[MEDITATION]/222.2 );
			else
				manarate += uint32_t( calcDef(0) / 5.0 );
		}
		else
		{
			if(med)
				manarate -= uint32_t( skill[MEDITATION]/222.2 );
		}
                manarate = qmax( 1, manarate );
		this->setRegenRate( STAT_MANA, manarate, VAR_EFF );
		this->updateRegenTimer( STAT_MANA );

	}
	if ( hp <= 0 )
		Kill();
	else
		for( uint32_t i = 0; i < 3; i++ )
			if( update[ i ] )
				updateStats( i );
}

void checkFieldEffects(uint32_t currenttime, P_CHAR pc, char timecheck );

void target_castSpell( NXWCLIENT ps, P_TARGET t )
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
				case POISON_WEAK:
					poisontime= uiCurrentTime + ( 15 * MY_CLOCKS_PER_SEC );
					// between 0% and 5% of player's hp reduced by racial combat poison resistance
					hp -= int32_t(
							qmax( ( ( hp ) * RandomNum( 0, 5 ) ) / 100, 3 ) *
							( (100 - Race::getPoisonResistance( race, POISON_WEAK ) ) / 100 )
						     );
					break;
				case POISON_NORMAL:
					poisontime = uiCurrentTime + ( 10 * MY_CLOCKS_PER_SEC );
					// between 5% and 10% of player's hp reduced by racial combat poison resistance
					hp -= int32_t(
							qmax( ( ( hp ) * RandomNum( 5, 10 ) ) / 100, 5 ) *
							( (100 - Race::getPoisonResistance( race, POISON_NORMAL ) ) / 100 )
						      );
					break;
				case POISON_GREATER:
					poisontime = uiCurrentTime+( 10 * MY_CLOCKS_PER_SEC );
					// between 10% and 15% of player's hp reduced by racial combat poison resistance
					hp -= int32_t(
							qmax( ( ( hp ) * RandomNum( 10,15 ) ) / 100, 7 ) *
							( (100 - Race::getPoisonResistance( race, POISON_GREATER ) ) / 100 )
						     );
					break;
				case POISON_DEADLY:
					poisontime = uiCurrentTime + ( 5 * MY_CLOCKS_PER_SEC );
					// between 15% and 20% of player's hp reduced by racial combat poison resistance
					if ( hp <= (getStrength()/4) ) {
						stm = qmax( stm - 6, 0 );
						updateStats( STAT_STAMINA );
					}
					hp -= int32_t(
							qmax( ( ( hp ) * RandomNum( 15, 20 ) ) / 100, 6) *
							( (100 - Race::getPoisonResistance( race, POISON_DEADLY ) ) / 100 )
						     );
					break;
				default:
					ErrOut("checkPoisoning switch fallout for char with serial [%u]\n", getSerial32() );
					poisoned = POISON_NONE;
					return;
				}
				if ( hp < 1 )
				{
					Kill();
					if ( !npc )
						sysmsg( TRANSLATE("The poison has killed you.") );
				}
				else
				{
					updateStats( STAT_HP );
					if ( poisontxt <= uiCurrentTime  )
					{
						emotecolor = 0x0026;
						switch ( poisoned )
						{
						case POISON_WEAK:
							emoteall( TRANSLATE("* %s looks a bit nauseous *"), 1, getCurrentNameC() );
							break;
						case POISON_NORMAL:
							emoteall( TRANSLATE("* %s looks disoriented and nauseous! *"), 1, getCurrentNameC());
							break;
						case POISON_GREATER:
							emoteall( TRANSLATE("* %s is in severe pain! *"), 1, getCurrentNameC());
							break;
						case POISON_DEADLY:
							emoteall( TRANSLATE("* %s looks extremely weak and is wrecked in pain! *"), 1, getCurrentNameC());
							break;
						}
						poisontxt = uiCurrentTime + ( 10 * MY_CLOCKS_PER_SEC );
					}
				}
			}
			else
			{
				poisoned = POISON_NONE;
				impowncreate( getSocket(), this, 1 ); // updating to blue stats-bar ...
				if ( !npc )
					sysmsg( TRANSLATE( "The poison has worn off." ) );
			}
		}
	}
}

void cChar::do_lsd()
{
	if (rand()%15==0)
	{
		NXWSOCKET socket = getSocket();

		int c1 = 0,c2 = 0,ctr = 0,xx,yy,icnt=0;
		int8_t zz;

		Location charpos = getPosition();

		NxwItemWrapper si;
		si.fillItemsNearXYZ( charpos, VISRANGE, false );
		for( si.rewind(); !si.isEmpty(); si++ ) {

			P_ITEM pi=si.getItem();
			if(!ISVALIDPI(pi))
				continue;

			uint16_t color=pi->getColor(); // fetch item's color and covert to 16 bit
			if (rand()%44==0)
				color+= pi->getPosition().x  - pi->getPosition().y;
			else
				color+= charpos.x + charpos.y;
			color+= rand()%3; // add random "noise"
			ctr++;

			// lots of color consistancy checks
			color=color%0x03E9;
			WORD2DBYTE( color, c1, c2 );
			if (color<0x0002 || color>0x03E9 )
				color=0x03E9;

			if( ((color&0x4000)>>14)+((color&0x8000)>>15) )
				color =0x1000 +rand()%255;

			if (rand()%10==0)
				zz= pi->getPosition().z + rand()%33;
			else
				zz= pi->getPosition().z;
			if (rand()%10==0)
				xx= pi->getPosition().x + rand()%3;
			else
				xx= pi->getPosition().x;
			if (rand()%10==0)
				yy= pi->getPosition().y + rand()%3;
			else
				yy= pi->getPosition().y;
			WORD2DBYTE(color, c1, c2);
			if (distFrom(pi)<13) if (rand()%7==0)
			{
				icnt++;
				if (icnt%10==0 || icnt<10)
					senditem_lsd(socket, DEREF_P_ITEM(pi),c1,c2,xx,yy,zz); // attempt to cut packet-bombing by this thing
			}
		}// end of if item

		if (rand()%33==0)
		{
			if (rand()%10>3)
				client->playSFX(0x00F8, true);
			else
			{
				int snd=rand()%19;
				if (snd>9)
					client->playSFX((0x01<<8)|((snd-10)%256), true);
				else
					client->playSFX(246+snd, true);
			}
		}
	}

}

/*
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
	regens[stat].timer= uiCurrentTime+ regens[stat].rate_eff*MY_CLOCKS_PER_SEC;
}

/*!
\author Luxor
\brief checks a skill for success (with sparring check)
\return true if success
\param sk skill
\param low low bound
\param high high bound
\todo document pcd parameter
\todo backport from Skills::
*/
const bool cChar::checkSkillSparrCheck(Skill sk, int32_t low, int32_t high, P_CHAR pcd)
{
	return Skills::CheckSkillSparrCheck(DEREF_P_CHAR(this),sk, low, high, pcd);
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

	Location charpos= getPosition();

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
				cPacketSendDeleteObj pk(pc_i->getSerial32());
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
                        cPacketSendUpdatePlayer pk(this, ndir, flag, hi_color );
                        ps_i->sendPacket(&pk);
		}
	}
}




