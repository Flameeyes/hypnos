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
*/

#include "settings.h"

#define SETTING(type, name, default) \
	type val##name = default; \
	type get##name() \
	{ return val##name; }
	
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
//@}

/*!
\brief Settings flags

This variable stores all the boolean settings as bits. The flags are used or-ing
the flags found in <b>Flags constants</b> group.
*/
uint64_t flags = 
	flagServerBookSystem | flagServerTradeSystem | flagServerBountySystem |
	flagServerHungerSystem | flagServerPopupHelp | flagServerUOAssist | 
	flagServerPlayerDeletePC | flagServerShowPCNames | flagActionsEquipOnDClick;

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
}

namespace Hunger {
	//! Is the Hunger system enabled?
	bool isEnabled()
	{ return flags & flagHungerSystemEnabled; }
	
	//!< How often you get hungrier in seconds
	SETTING(uint16_t, HungerRate, 6000);
	
	//! How much health is lost when you are starving
	SETTING(uint16_t, HungetDamage, 2);
	
	//! How often you lose health when you are starving
	SETTING(uint16_t, HungerDamageRate, 10);
}

namespace Actions {
	bool shouldEquipOnDClick()
	{ return flags & flagActionsEquipOnDClick; }
	
	bool canUseItemsWhenInvisible()
	{ return flags & flagActionsUseInvisible; }
	
	//! How much karma is gained for bounty hunting? \todo Set the default
	SETTING(uint16_t, BountyKarmaGain, 0);
	
	//! How much fame is gained for bounty hunting? \todo Set the default
	SETTING(uint16_t, BountyFameGain, 0);
}

namespace Skills {
	bool canStealthOnHorse()
	{ return flags & flagSkillsStealthOnHorse; }
	
	//! Maximum stat gain in a (real) day \todo Set the default
	SETTING(uint16_t, StatDailyLimit, 0)
	
	//! Defending player must have N skill points more than the attacker for the attacker to gain skill (in PvP only).
	SETTING(uint16_t, LimitPlayerSparring, 0);

	//! How much karma is lost snooping? \todo Set the default
	SETTING(uint16_t, SnoopKarmaLoss, 0);
	
	//! How much fame is lost snooping? \todo Set the default
	SETTING(uint16_t, SnoopFameLoss, 0);
	
	//! How much karma is lost stealing? \todo Set the default
	SETTING(uint16_t, StealKarmaLoss, 0);
	
	//! How much fame is lost stealing? \todo Set the default
	SETTING(uint16_t, StealFameLoss, 0);
	
	//! Stealth necessary to pick up an item and remain hidden.
	//! If INVALID pc is always unhid while picking up items from the ground
	SETTING(uint16_t, StealthToTakeItemsWhileHid, 960);
	
	//! Stealth necessary to drop an item and remain hidden.
	//! If INVALID pc is always unhid while dropping items on the ground
	SETTING(uint16_t, StealthToDropItemsWhileHid, 960);

	//! Maximum skill an npc trainer can teach to a PC
	SETTING(uint16_t, MaximumSkillTraining, 250);
}

namespace Logging {
	//! Gets the absolute path of the logs directory
	SETTING(std::string, LogPath, "");
}

} // namespace nSettings
