/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file 
\brief Declaration of class cEquippable
*/

#ifndef __CEQUIPPABLE_H__
#define __CEQUIPPABLE_H__

#include "common_libs.h"
#include "objects/citem.h"

/*!
\brief Class for equippable items

This class is used to add support for equippable items, like weapons,
shields, armors and clothes.
This is an abstract class, thus no direct instance of this is possible.
Some special classes (like bankboxes and backpacks) can derive both from this
and from cContainer class.
*/
class cEquippable : public cItem
{
public:
	cEquippable();
	cEquippable(uint32_t newSerial);
	virtual ~cEquippable();

	enum Layers {
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
	
	virtual void setContainer(pObject obj);
	
	inline const Layers getLayer() const
	{ return layer; }
protected:
	Layers layer;
	static Layers findLayerFromID(uint16_t id);
	
};

#endif