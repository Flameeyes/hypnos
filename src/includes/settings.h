/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Server settings access
*/

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "common_libs.h"

/*!
\author Flameeyes
\brief Access to server settings

This namespace contains all the method to access the server settings.
All the settings variables are stored in the settings.cpp file and then
there won't be inline methods. This should be anyway managed by the compiler.

\note Namespace inside that hasn't the n prefix, because nSettings is a special
	namespace.
\note Groups inside namespaces aren't namespaces themselves to avoid long
	function calls, but are saved in different elements inside the XML
	file, so one can simply delete the base element to have the default
	values.
*/
namespace nSettings {
	/*!
	\brief Server related settings
	*/
	namespace Server {
		bool isEnabledBookSystem();		//!< Is the book system enabled? [Default: true]
		bool isEnabledTradeSystem();		//!< Is the trade system enabled? [Default: true]
		bool isEnabledBountySystem();		//!< Is the bounty system enabled? [Default: true]

		bool isEnabledPopupHelp();		//!< popup help (since T2A) is enabled [Default:true]
		bool isEnabledUOAssist();		//!< UO Assist enabled? [Default: true]
		uint32_t getAllowedAssistVersion();	//!< Returns allowed version of UO assist [Default: \b missing ]

		uint8_t getMaximumPCs();		//!< Maximum characters for account [Default: 5]
		bool canPlayersDeletePCs();		//!< Can a player delete one of his characters? [Default: true]

		uint16_t getBankMaxItems();		//!< Maximum items a player can put in a bank. 0 = Unlimited [Default: 0]
		uint8_t getMaximumItemsOnTile();	//!< Maximum number of items in a single world tile  [Default: \b missing]

		bool hasInstantGuards();		//!< Should guards instantly teleport over new criminals? [Default: false]

		bool shouldShowPCNames();		//!< Should the playing characters' names be shown on single click? [Default: true]

		uint32_t getDelaySkills();		//!< Delay between two skill uses
		uint32_t getDelayObjects();		//!< Delay between two object uses
		uint32_t getDecayTimer();		//!< Decay timer

		uint32_t getWeightPerStr();		//!< How much weight can take with one str point? [Default: 4]
	}
	
	namespace MsgBoards {
		uint16_t getMaxPosts();			//!< Maximum post possible [Default: 128]
		uint16_t getMaxEntries();		//!< Maximum entries in ESCORTS [Default: 256]
		uint8_t getMessageRetention();		//!< Maximum number of days to maintain a post [Default: 30]
		uint32_t getEscortInitExpire();		//!< Number of seconds until an escorting quest expires [Default: 1 day]
		uint32_t getEscortActiveExpire();	//!< Don't know actually.. [Default: 1/2 hour]
	}
	
	//! Hunger System settings
	namespace Hunger {
		bool isEnabled();			//!< Is the Hunger system enabled? [Default: true]
		uint16_t getHungerRate();		//!< How often you get hungrier in seconds [Default: 6000]
		uint16_t getHungerDamage();		//!< How much health is lost when you are starving  [Default: 2]
		uint16_t getHungerDamageRate();		//!< How often you lose health when you are starving [Default: 10]
	}
	
	//! Actions related settings
	namespace Actions {
		bool shouldEquipOnDClick();		//!< Should an item be equipped on double click? [Default: true]
		bool canUseItemsWhenInvisible();	//!< When invisible (by spell) can a player use items? [Default: false]
	}
	
	//! Reputation system settings
	namespace Reputation {
		uint16_t getBountyKarmaGain();		//!< How much karma is gained for bounty hunting? [Default: \b missing ]
		uint16_t getBountyFameGain();		//!< How much fame is gained for bounty hunting? [Default: \b missing ]
		
		SuspectAction getPolymorphAction();
		SuspectAction getHelpingGreyAction();
		SuspectAction getHelpingCriminalAction();
		SuspectAction getLootingAction();
		
		uint16_t getStealKarmaLoss();	//!< How much karma is lost stealing? [Default: \b missing ]
		uint16_t getStealFameLoss();	//!< How much fame is lost stealing? [Default: \b missing ]
		SuspectAction getStealAction();
		
		uint16_t getSnoopKarmaLoss();	//!< How much karma is lost snooping? [Default: \b missing ]
		uint16_t getSnoopFameLoss();	//!< How much fame is lost snooping? [Default: \b missing ]
		SuspectAction getSnoopAction();
		
		uint16_t getChopFameLoss();
		uint16_t getChopKarmaLoss();
		SuspectAction getChopAction();
	}

	//! Skill related settings
	namespace Skills {
		uint16_t getStatDailyLimit();		//!< Maximum stat gain in a (real) day [Default: \b missing ]
		uint16_t getLimitPlayerSparring();	//!< Defending player must have N skill points more than the attacker for the attacker to gain skill (in PvP only). [Default: 0]
		uint16_t getMaximumSkillTraining();	//!< maximum skill an npc trainer can teach to a PC [Default: 300]
		bool isEnabledSkillBySkillCap();	//!< Recent uo clients allow setting of a skillcap for each skill. If enabled allows server side support for that [Default: true]

		//@{
		/*!
		\name Hiding and Stealth
		*/
			bool canStealthOnHorse();	//!< Can a character stealth on an horse? [Default: false ]
			uint16_t getStealthToTakeItemsWhileHid();
				//!< Stealth necessary to pick up an item and remain hidden.
				//!< If INVALID pc is always unhid while picking up items from the ground [Default: 960]
			uint16_t getStealthToDropItemsWhileHid();
				//!< Stealth necessary to drop an item and remain hidden.
				//!< If INVALID pc is always unhid while dropping items on the ground [Default: 960]
		//@}
		
		//@{
		/*!
		\name Thievery
		*/
			uint32_t getSnoopDelay();
		//@}
	}
	
	//! Log system settings
	namespace Logging {
		std::string getLogPath();		//!< Gets the absolute path of the logs directory
	}
	
	//! Jails related stuff
	namespace Jails {
		bool isJailAccountLevel();		//!< Is the jail valid account-level, or character-level? [Default: true]
	}
};

#endif
