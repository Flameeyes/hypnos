/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Server settings load and use

In this file I'll write the functions to access to the server's settings,
like skills settings, working systems and so on.
To store the settings in a file, we'll use a settings.xml file.

Note about defined macros. Usually I hate them and I prefer not to use them,
but in this file we have a lot of identical code which change only the names
of the settings, so I defined the macros to have the code more clean (also
if it makes the error handling more difficult).
*/

#include <mxml.h>
#include "settings.h"
#include "logsystem.h"
#include "abstraction/tvariant.h"
#include "backend/strconstants.h"

#define SETTING(type, name, default) \
	type val##name = default; \
	type get##name() \
	{ return val##name; }
	
#define SECTION(sname) \
	if ( n->name() == #sname ) \
		sname::load(n);

#define XMLSETTING(sname, vartype, functype) \
	if ( n->name() == #sname ) \
	{ \
		tVariant v = n->data(); \
		bool result = false; \
		vartype temp = v.to##functype(&result); \
		if ( result ) val##sname = temp; \
		else LogWarning("Invalid value for parameter ##name"); \
	}

#define BOOLSETTING(sname, sflag) \
	if ( n->name() == #sname ) { \
		if ( ! n->hasAttribute("enabled") || \
			( n->getAttribute("enabled") != "false" && n->getAttribute("enabled") != "true" ) ) \
		{ \
			LogWarning("Invalid value for setting #sname"); \
		} else { \
			setFlag(sflag, n->getAttribute("enabled") == "true"); \
		} \
	}

namespace nSettings
{

//@{
/*!
\name Flags constants

The constants of this group are or-ed together to get the server boolean's settings.
*/
	//! Book system enabled
	static const uint64_t flagServerBookSystem 	= 0x0000000000000001;
	//! Trade system enabled
	static const uint64_t flagServerTradeSystem	= 0x0000000000000002;
	//! Bounty system enabled
	static const uint64_t flagServerBountySystem	= 0x0000000000000004;
	//! Hunger system enabled
	static const uint64_t flagHungerSystemEnabled	= 0x0000000000000008;
	//! Popup Help enabled
	static const uint64_t flagServerPopupHelp	= 0x0000000000000010;
	//! UO Assist allowed
	static const uint64_t flagServerUOAssist	= 0x0000000000000020;
	//! Players can delete PCs
	static const uint64_t flagServerPlayerDeletePC	= 0x0000000000000040;
	//! The server has instant guards
	static const uint64_t flagServerInstantGuards	= 0x0000000000000080;
	//! The PCs' names should be shown
	static const uint64_t flagServerShowPCNames	= 0x0000000000000100;
	//! Items should be equipped on double click
	static const uint64_t flagActionsEquipOnDClick	= 0x0000000000000200;
	//! Items can be used when invisible
	static const uint64_t flagActionsUseInvisible	= 0x0000000000000400;
	//! Stealth on horse
	static const uint64_t flagSkillsStealthOnHorse	= 0x0000000000000800;
	//! Use a skill by skill skillcap
	static const uint64_t flagSkillBySkillCap	= 0x0000000000001000;
	//! Is the jail account level
	static const uint64_t flagJailsAccountLevel	= 0x0000000000002000;
//@}

/*!
\brief Settings flags

This variable stores all the boolean settings as bits. The flags are used or-ing
the flags found in <b>Flags constants</b> group.
*/
uint64_t flags =
	flagServerBookSystem | flagServerTradeSystem | flagServerBountySystem |
	flagHungerSystemEnabled | flagServerPopupHelp | flagServerUOAssist |
	flagServerPlayerDeletePC | flagServerShowPCNames | flagActionsEquipOnDClick |
	flagSkillBySkillCap | flagJailsAccountLevel;

//! Sets a determined flag on or off
void setFlag(const uint64_t flag, bool on = true)
{
	if ( on )
		flags |= flag;
	else
		flags &= ~flag;
}

/*!
\brief Message Boards related settings

(obsolete calculation)
--------------------------------------------------------------------------------
Buffer Size = 2560
Therefore 0x3c max size = 2560 - 5 (0x3c header info) = 2550
2550 / 19 (item segment size per msg) = 134
Round down to 128 messages allowable on a message board (better safe than sorry)
--------------------------------------------------------------------------------

Outbound packets now can have any length, so the maxpost is only used to have a
REASONABLE sized packet to send (Chronodt 10/3/04)
*/
namespace MsgBoards {
	//! Maximum post possible
	SETTING(uint16_t, MaxPosts, 128);
	
	//! Maximum entries in ESCORST
	SETTING(uint16_t, MaxEntries, 256);
	
	//! Maximum days to maintain a post
	SETTING(uint8_t, MessageRetention, 30);
	
	//! Number of seconds until an escorting quest expries
	SETTING(uint32_t, EscortInitExpire, DAYSECONDS);
	
	//! \todo Find out what actually is this!
	SETTING(uint32_t, EscortActiveExpire, 1800);
	
	void load(MXML::Node *n)
	{
		do {
			XMLSETTING(MaxPosts, uint16_t, UInt16)
			else XMLSETTING(MaxEntries, uint16_t, UInt16)
			else XMLSETTING(MessageRetention, uint8_t, UInt8)
			else XMLSETTING(EscortInitExpire, uint32_t, UInt32)
			else XMLSETTING(EscortActiveExpire, uint32_t, UInt32)
			else LogWarning("Unknown node %s in settings.xml, ignoring", n->name().c_str() );
			n = n->next();
		} while(n);
	}
}

namespace Server {
	bool isEnabledBookSystem()
	{ return flags & flagServerBookSystem; }
	
	bool isEnabledTradeSystem()
	{ return flags & flagServerTradeSystem; }
	
	bool isEnabledBountySystem()
	{ return flags & flagServerBountySystem; }
	
	bool isEnabledPopupHelp()
	{ return flags & flagServerPopupHelp; }
	
	bool isEnabledUOAssist()
	{ return flags & flagServerUOAssist; }
	
	bool canPlayersDeletePCs()
	{ return flags & flagServerPlayerDeletePC; }

	bool hasInstantGuards()
	{ return flags & flagServerInstantGuards; }
	
	bool shouldShowPCNames()
	{ return flags & flagServerShowPCNames; }
	
	//! Allowed UO Assist version \todo Set the default
	SETTING(uint32_t, AllowedAssistVersion, 0);
	
	//! Maximum characters for account
	SETTING(uint8_t, MaximumPCs, 5);
	
	//! Maximum items in bank (0 = no limit)
	SETTING(uint16_t, BankMaxItems, 0);
	
	//! Maximum items on a single tile \todo Set the default
	SETTING(uint8_t, MaximumItemsOnTile, 0);
	
	//! Delay between two skill uses \todo Set the default
	SETTING(uint32_t, DelaySkills, 0);
	
	//! Delay between two object uses \todo Set the default
	SETTING(uint32_t, DelayObjects, 0);
	
	//! Decay timer \todo Set the default
	SETTING(uint32_t, DecayTimer, 0);
	
	//! How much weight can take with one str point?
	SETTING(uint32_t, WeightPerStr, 4);

	void load(MXML::Node *s)
	{
		MXML::Node *n = s->child();
		do {
			XMLSETTING(AllowedAssistVersion, uint32_t, UInt32)
			else XMLSETTING(MaximumPCs, uint8_t, UInt8)
			else XMLSETTING(BankMaxItems, uint16_t, UInt16)
			else XMLSETTING(MaximumItemsOnTile, uint8_t, UInt8)
			else XMLSETTING(DelaySkills, uint32_t, UInt32)
			else XMLSETTING(DelayObjects, uint32_t, UInt32)
			else XMLSETTING(DecayTimer, uint32_t, UInt32)
			else XMLSETTING(WeightPerStr, uint32_t, UInt32)
			else BOOLSETTING(BookSystem, flagServerBookSystem)
			else BOOLSETTING(TradeSystem, flagServerTradeSystem)
			else BOOLSETTING(BountySystem, flagServerBountySystem)
			else BOOLSETTING(PopupHelp, flagServerPopupHelp)
			else BOOLSETTING(UOAssist, flagServerUOAssist)
			else BOOLSETTING(ShowPCNames, flagServerShowPCNames)
			else BOOLSETTING(InstantGuards, flagServerInstantGuards)
			else BOOLSETTING(PlayersDeletePC, flagServerPlayerDeletePC)
			else LogWarning("Unknown node %s in settings.xml, ignoring", n->name().c_str() );
			n = n->next();
		} while(n);
	}
}

namespace Hunger {
	//! Is the Hunger system enabled?
	bool isEnabled()
	{ return flags & flagHungerSystemEnabled; }
	
	//!< How often you get hungrier in seconds
	SETTING(uint16_t, HungerRate, 6000);
	
	//! How much health is lost when you are starving
	SETTING(uint16_t, HungerDamage, 2);
	
	//! How often you lose health when you are starving
	SETTING(uint16_t, HungerDamageRate, 10);

	void load(MXML::Node *s)
	{
		if ( ! s->hasAttribute("enabled") ||
			( s->getAttribute("enabled") != "false" && s->getAttribute("enabled") != "true" ) )
		{
			LogWarning("Invalid value for setting HungerSystem");
		} else
			setFlag(flagHungerSystemEnabled, s->getAttribute("enabled") == "true");
			
		MXML::Node *n = s->child();
		do {
			XMLSETTING(HungerRate, uint16_t, UInt16)
			else XMLSETTING(HungerDamage, uint16_t, UInt16)
			else XMLSETTING(HungerDamageRate, uint16_t, UInt16)
			else LogWarning("Unknown node %s in settings.xml, ignoring", n->name().c_str() );
			n = n->next();
		} while(n);
	}
}

namespace Actions {
	bool shouldEquipOnDClick()
	{ return flags & flagActionsEquipOnDClick; }
	
	bool canUseItemsWhenInvisible()
	{ return flags & flagActionsUseInvisible; }
	
	//! Prevents permanent usage of objects, Sets the minimum time (seconds) between usage of objects
	SETTING(uint32_t, ObjectsDelay, 1);
	
	void load(MXML::Node *s)
	{
		MXML::Node *n = s->child();
		do {
			BOOLSETTING(EquipOnDClick, flagActionsEquipOnDClick)
			else BOOLSETTING(UseItemsWhenInvisible, flagActionsUseInvisible)
			else XMLSETTING(ObjectsDelay, uint32_t, UInt32)
			else LogWarning("Unknown node %s for nSettings::Actions namespace, ignoring", n->name().c_str() );
			n = n->next();
		} while(n);
	}
}

namespace Reputation {
	//! How much karma is gained for bounty hunting? \todo Set the default
	SETTING(uint16_t, BountyKarmaGain, 0);
	
	//! How much fame is gained for bounty hunting? \todo Set the default
	SETTING(uint16_t, BountyFameGain, 0);
	
	//! What to do after a polymorph
	SETTING(SuspectAction, PolymorphAction, saNormal);
	
	//! What to do after helping a grey
	SETTING(SuspectAction, HelpingGreyAction, saNormal);
	
	//! What to do after helping a criminal
	SETTING(SuspectAction, HelpingCriminalAction, saNormal);
	
	//! How much karma is lost stealing? \todo Set the default
	SETTING(uint16_t, StealKarmaLoss, 0);
	
	//! How much fame is lost stealing? \todo Set the default
	SETTING(uint16_t, StealFameLoss, 0);
	
	//! What to do after a steal
	SETTING(SuspectAction, StealAction, saNormal);
	
	//! How much karma is lost snooping? \todo Set the default
	SETTING(uint16_t, SnoopKarmaLoss, 0);
	
	//! How much fame is lost snooping? \todo Set the default
	SETTING(uint16_t, SnoopFameLoss, 0);
	
	//! What to do after a snoop
	SETTING(SuspectAction, SnoopAction, saNormal);
	
	//! How much karma is loss chopping a corpse? \todo Set the default
	SETTING(uint16_t, ChopKarmaLoss, 0);
	
	//! How much fame is loss chopping a corpse? \todo Set the default
	SETTING(uint16_t, ChopFameLoss, 0);
	
	//! What to do after a corpse chop
	SETTING(SuspectAction, ChopAction, saNormal);
	
	//! What to do after a loot
	SETTING(SuspectAction, LootingAction, saNormal);
	
	#define SUSPECT(A) \
		if ( n->name() == #A ) \
			var##A = nStrConstants::suspectAction(n->data());
	
	void load(MXML::Node *s)
	{
		MXML::Node *n = s->child();
		do {
			XMLSETTING(BountyKarmaGain, uint16_t, UInt16)
			else XMLSETTING(BountyFameGain, uint16_t, UInt16)
			else SUSPECT(PolymorphAction)
			else SUSPECT(HelpingGreyAction)
			else SUSPECT(HelpingCriminalAction)
			
			else XMLSETTING(StealKarmaLoss, uint16_t, UInt16)
			else XMLSETTING(StealFameLoss, uint16_t, UInt16)
			else SUSPECT(StealAction)
			
			else XMLSETTING(SnoopKarmaLoss, uint16_t, UInt16)
			else XMLSETTING(SnoopFameLoss, uint16_t, UInt16)
			else SUSPECT(SnoopAction)
			
			else XMLSETTING(ChopKarmaLoss, uint16_t, UInt16)
			else XMLSETTING(ChopFameLoss, uint16_t, UInt16)
			else SUSPECT(ChopAction)
			
			else SUSPECT(LootingAction)
			else LogWarning("Unknown node %s for nSettings::Reputation namespace, ignoring", n->name().c_str() );
			n = n->next();
		} while(n);
	}
}

namespace Skills {
	bool canStealthOnHorse()
	{ return flags & flagSkillsStealthOnHorse; }
	
	//! Maximum stat gain in a (real) day \todo Set the default
	SETTING(uint16_t, StatDailyLimit, 0)
	
	//! Defending player must have N skill points more than the attacker for the attacker to gain skill (in PvP only).
	SETTING(uint16_t, LimitPlayerSparring, 0);

	//! Stealth necessary to pick up an item and remain hidden.
	//! If INVALID pc is always unhid while picking up items from the ground
	SETTING(uint16_t, StealthToTakeItemsWhileHid, 960);
	
	//! Stealth necessary to drop an item and remain hidden.
	//! If INVALID pc is always unhid while dropping items on the ground
	SETTING(uint16_t, StealthToDropItemsWhileHid, 960);

	//! Maximum skill an npc trainer can teach to a PC
	SETTING(uint16_t, MaximumSkillTraining, 300);

	bool isEnabledSkillBySkillCap()
	{ return flags & flagSkillBySkillCap; }
	
	//! Maximum number of stealth steps to perform
	SETTING(uint32_t, MaxStealthSteps, 10);

	//! Prevents permanent skill usage.
	//! Sets the minimum time (seconds) between allowed skill usages
	SETTING(uint32_t, SkillDelay, 5);
	
	//! Prevents heavy macroing of snooping.
	//! Sets the minimum time (seconds) between snoops
	SETTING(uint32_t, SnoopDelay, 15);
	
	void loadHiding(MXML::Node *s)
	{
		MXML::Node *n = s->child();
		do {
			BOOLSETTING(StealthOnHorse, flagSkillsStealthOnHorse)
			else XMLSETTING(StealthToTakeItemsWhileHid, uint16_t, UInt16)
			else XMLSETTING(StealthToDropItemsWhileHid, uint16_t, UInt16)
			else XMLSETTING(MaxStealthSteps, uint8_t, UInt8)
			else BOOLSETTING(SkillbySkillCap, flagSkillBySkillCap)
			else LogWarning("Unknown node %s in settings.xml, ignoring", n->name().c_str() );
			n = n->next();
		} while(n);
	}
	
	void loadThievery(MXML::Node *s)
	{
		MXML::Node *n = s->child();
		do {
			XMLSETTING(SnoopDelay, uint32_t, UInt32)
			else LogWarning("Unknown node %s in settings.xml, ignoring", n->name().c_str() );
			n = n->next();
		} while(n);
	}
	
	void load(MXML::Node *s)
	{
		MXML::Node *n = s->child();
		do {
			if ( n->name() == "Hiding" ) loadHiding(n);
			else if ( n->name() == "Thievery" ) loadThievery(n);
			else XMLSETTING(StatDailyLimit, uint16_t, UInt16)
			else XMLSETTING(LimitPlayerSparring, uint16_t, UInt16)
			else XMLSETTING(MaximumSkillTraining, uint16_t, UInt16)
			else XMLSETTING(SkillDelay, uint32_t, UInt32)
			else LogWarning("Unknown node %s in settings.xml, ignoring", n->name().c_str() );
			n = n->next();
		} while(n);
	}
}

namespace Logging {
	//! Gets the absolute path of the logs directory
	SETTING(std::string, LogPath, "");
	
	void load(MXML::Node *s)
	{
		MXML::Node *n = s->child();
		do {
			XMLSETTING(LogPath, std::string, String)
			else LogWarning("Unknown node %s in settings.xml, ignoring", n->name().c_str() );
			n = n->next();
		} while(n);
	}
	
}

namespace Jails {
	bool isJailAccountLevel()
	{ return flags & flagJailsAccountLevel; }

	void load(MXML::Node *s)
	{
		MXML::Node *n = s->child();
		do {
			BOOLSETTING(StealthOnHorse, flagJailsAccountLevel)
			else LogWarning("Unknown node %s in settings.xml, ignoring", n->name().c_str() );
			n = n->next();
		} while(n);
	}
}

void load(std::istream &xmlfile)
{
	try {
		MXML::Document doc(xmlfile);
		
		MXML::Node *n = doc.main()->child();
		do {
			SECTION(MsgBoards)
			else SECTION(Server)
			else SECTION(Hunger)
			else SECTION(Actions)
			else SECTION(Reputation)
			else SECTION(Skills)
			else SECTION(Logging)
			else
				LogWarning("Unknown node %s in settings.xml, ignoring", n->name().c_str() );
			
			n = n->next();
		} while(n);
	} catch ( MXML::MalformedError e) {
		LogCritical("settings.xml file not well formed. Default loading");
	}
}

} // namespace nSettings
