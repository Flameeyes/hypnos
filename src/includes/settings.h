/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
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
\brief Access to server settings

This namespace contains all the method to access the server settings.
All the settings variables are stored in the settings.cpp file and then
there won't be inline methods. This should be anyway managed by the compiler.

\todo For now this is only declarations, not implementations. To have
	implementations we need the XML Parser by il_guru, so for now I'll
	put comments about the items here.
\note Namespace inside that hasn't the n prefix, because nSettings is a special
	namespace.
*/
namespace nSettings {
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
		uint32_t getMaxPosts();			//!< Maximum post possible [Default: 128]
		uint32_t getMaxEntries();		//!< Maximum entries in ESCORTS [Default: 256]
	}
	
	/*!
	\brief Server related settings
	*/
	namespace Server {
		bool isEnabledBookSystem();		//!< Is the book system enabled? [Default: true]
		bool isEnabledTradeSystem();		//!< Is the trade system enabled? [Default: true]
		bool isEnabledBountySystem();		//!< Is the bounty system enabled? [Default: true]
		bool isEnabledPopupHelp();		//!< popup help (since T2A) is enabled [Default:true]
		bool isEnabledUOAssist();		//!< UO Assist enabled? [Default: true]
                uint32_t getAllowedAssistVersion();	//!< Returns allowed version of UO assist [Default: \b missing]

		uint8_t getMaximumPCs();		//!< Maximum characters for account [Default: 5]
		bool canPlayersDeletePCs();		//!< Can a player delete one of his characters? [Default: true]

		uint16_t getBankMaxItems();		//!< Maximum items a player can put in a bank. 0 = Unlimited [Default: 0]
		
		bool hasInstantGuards();		//!< Should guards instantly teleport over new criminals? [Default: false]
		
		bool shouldShowPCNames();		//!< Should the playing characters' names be shown on single click? [Default: true]
		
		uint32_t getDelaySkills();		//!< Delay between two skill uses
		uint32_t getDelayObjects();		//!< Delay between two object uses
		uint32_t getDecayTimer();		//!< Decay timer
		
		uint32_t getWeightPerStr();		//!< How much weight can take with one str point? [Default: 4]
	}
	
	/*!
	\brief Actions related settings
	*/
	namespace Actions {
		bool shouldEquipOnDClick();		//!< Should an item be equipped on double click? [Default: true]
                bool canUseItemsWhenInvisible();	//!< When invisible (by spell) can a player use items? [Default: \b missing ]
		uint16_t getBountyKarmaGain();		//!< How much karma is gained for bounty hunting? [Default: \b missing ]
		uint16_t getBountyFameGain();		//!< How much fame is gained for bounty hunting? [Default: \b missing ]
	}
	
	/*!
	\brief Skill related settings
	*/
	namespace Skills {
		bool canStealthOnHorse();		//!< Can a character stealth on an horse? [Default: \b missing ]
		uint16_t getStatDailyLimit();		//!< Maximum stat gain in a (real) day [Default: \b missing ]

		uint16_t getLimitPlayerSparring();	//!< Defending player must have N skill points more than the attacker for the attacker to gain skill (in PvP only). [Default: 0]
		
		//@{
		/*!
		\name Stealing and Snooping
		*/
			uint16_t getSnoopKarmaLoss();	//!< How much karma is lost snooping? [Default: \b missing ]
			uint16_t getSnoopFameLoss();	//!< How much fame is lost snooping? [Default: \b missing ]
			uint16_t getStealKarmaLoss();	//!< How much karma is lost stealing? [Default: \b missing ]
			uint16_t getStealFameLoss();	//!< How much fame is lost stealing? [Default: \b missing ]
		//@}
	}
};

#endif
