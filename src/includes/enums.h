/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Enum types
\author Flameeyes

I created this file because in many place I'm including headers in header
files only to access their enums (like cmsgboard.h in cchar.h). Here we
create less dependencies, also if they are externed from their original
content.
For this, I'll add a \\see tag on all the enums to find the original context.
*/

#ifndef __ENUMS_H__
#define __ENUMS_H__

/*!
\brief Message Board's Post's Type

\see cMsgBoard
*/
enum PostType { ptLocalPost = 0, ptRegionalPost, ptGlobalPost };

/*!
\brief Message Board's Quest's Type

\see cMsgBoard
*/
enum QuestType { qtEscort = 0xFF, qtBoundy = 0xFE, qtItem = 0xFD, qtInvalid = 0x0 };

/*!
\brief Layer index as used by client

\see cEquippable
\see cBody
*/
enum Layer {
	layNone,		//!< Not equipped
	layWeapon1H = 1,	//!< One handed weapon layer
	layWeapon2H,		//!< Two handed weapon, shield, or misc layer
	layShoes,		//!< Shoes layer
	layPants,		//!< Pants layer
	layShirt,		//!< Shirt layer
	layHat,			//!< Helm/Hat layer
	layGloves,		//!< Gloves layer
	layRing,		//!< Ring layer
	layUnused,		//!< Unused layer
	layNeck,		//!< Neck layer
	layHair,		//!< Hair layer
	layWaist,		//!< Waist (half apron) layer
	layTorsoInner,		//!< Torso (inner) (chest armor) layer
	layBracelet,		//!< Bracelet layer
	layUnused2,		//!< Unused (backpack?) layer
	layBeard,		//!< Facial Hair (Beard) layer
	layTorsoMiddle,		//!< Torso (middle) (sircoat, tunic, full apron, sash) layer
	layEarrings,		//!< Earrings layer
	layArms,		//!< Arms layer
	layBack,		//!< Back (cloack) layer
	layBackpack,		//!< Backpack layer
	layTorsoOuter,		//!< Torso (outer) (robe) layer
	layLegsOuter,		//!< Legs (outer) (skirt/kilt) layer
	layLegsInner,		//!< Legs (inner) (leg armor) layer
	layMount,		//!< Mount (horse, ostarard, etc) layer
	layNPCBuyR,		//!< NPC Buy Restock container layer (see cNPC::restock)
	layNPCBuyN,		//!< NPC Buy no-restock container layer (see cNPC::bought)
	laySell,		//!< NPC Buy Sell container layer (see cNPC::sell)
	layBank			//!< PC Bank Box (see cPC::bankBox)
};

/*!
\brief Type of crypto-algorithms

\see cAccount
*/
enum CryptoType {
	cryptoPlain,	//! Plain text
	cryptoMD5,	//! MD5 hash
	cryptoSHA1	//! SHA1 hash
};

#endif
