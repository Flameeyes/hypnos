/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declaration of cNPC class
*/

#ifndef __CNPC_H__
#define __CNPC_H__

#include "objects/cchar.h"

/*!
\brief This class represent a non-playing character.

In this class there are members used only for NPCs, and not for playing
characters.
*/
class cNPC : public cChar
{
public:
	enum {
		evtNpcOnDispel = evtChrMax,
		evtNpcOnTransfer,
		evtNpcOnHeartBeat,
		evtNpcOnCheckNPCAI,
		evtNpcMax
	};
	
	cNPC();
	cNPC(uint32_t serial);
	virtual ~cNPC();

	virtual const std::string getPopupHelp() const;

protected:
	pChar owner;	//! NPC's owner
	virtual const uint16_t eventsNumber() const
	{ return evtNpcMax; }

public:
	//! Gets NPC's owner
	inline pChar getOwner() const
	{ return owner; }

	void setOwner(pChar pc);

protected:
	//! Heartbeat for NPCs
	void heartbeat();
public:
	bool updateFlag();
	void simpleAttack(pChar pc);

//@{
/*!
\name Npc Vendor
\note fist draft.. need to make map with id used in receive for make faster
*/

public:
	std::vector<vendor_item>*	vendorItemsSell; //!< all items sold by vendor
	std::vector<vendor_item>*	vendorItemsBuy; //!< all items sold by vendor

//@}

//@{
/*!
\name Flags
*/

public:
	static const uint64_t flagCanTrain		= 0x0000000100000000ull;
	static const uint64_t flagShopKeeper	= 0x0000000200000000ull;
	static const uint64_t flagDispellable	= 0x0000000400000000ull;
	static const uint64_t flagWaterWalk		= 0x0000000800000000ull; //!< Char walks on water

	inline const bool canTrain() const
	{ return flags & flagCanTrain; }

	inline void setCanTrain(bool on = true)
	{ setFlag(flagCanTrain, on); }

	inline const bool isShopKeeper() const
	{ return flags & flagShopKeeper; }

	inline void setShopKeeper(bool on = true)
	{ setFlag(flagShopKeeper, on); }

	inline const bool isDispellable() const
	{ return flags & flagDispellable; }

	inline void setDispellable(bool on = true)
	{ setFlag(flagDispellable, on); }

	inline const bool canWaterWalk() const
	{ return flags & flagWaterWalk; }

	inline void setCanWaterWalk(bool on = true)
	{ setFlag(flagWaterWalk, on); }

//@}

//@{
/*!
\name Stabling
*/
protected:
	pNPC stablemaster;	//!< The stablemaster
public:
	//! Check if char is stabled
	inline const bool isStabled() const
	{ return stablemaster; }

	//! Get the character's stablemaster
	inline const pNPC getStablemaster() const
	{ return stablemaster; }

	void stable( pNPC sm );
	void unStable();
//@}

//@{
/*!
\name Quests
*/
public:
	void createEscortQuest();		// Uses this npc to carry out an escort quest. Publishes it on regional msgboards
        void clearedEscordQuest(pPC pc);        // pc has brought to destination this npc, so gets the reward :D
        void removepostEscortQuest();		// quest was never accepted, so removing message
        void deleteEscortQuest();		// deleting npc used for quest
//@}

//@{
/*!
\name AI
*/
private:
	void beginCasting(pChar target, magic::SpellId spell);
	void checkAI();
//@}
};

#endif
