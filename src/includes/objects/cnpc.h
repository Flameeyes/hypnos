/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __OBJECTS_CNPC_H__
#define __OBJECTS_CNPC_H__

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
	
	enum NpcAi
	{
		NPCAI_GOOD		= 0,
		NPCAI_HEALER,
		NPCAI_EVIL,
		NPCAI_EVILHEALER,
		NPCAI_TELEPORTGUARD,
		NPCAI_BEGGAR,
		NPCAI_ORDERGUARD,
		NPCAI_CHAOSGUARD,
		NPCAI_BANKER,
		NPCAI_GUARD,
		NPCAI_TAMEDDRAGON,
		NPCAI_ATTACKSRED,
		NPCAI_PLAYERVENDOR	= 17,
		NPCAI_PETGUARD		= 32,
		NPCAI_MADNESS		= 50,
		NPCAI_DRAGON2		= 60,
		NPCAI_DRAGON1		= 96
	};
	
	enum WanderMode {
		WANDER_NOMOVE = 0,
		WANDER_FOLLOW,
		WANDER_FREELY_CIRCLE,
		WANDER_FREELY_BOX,
		WANDER_FREELY,
		WANDER_FLEE,
		WANDER_AMX
	};
	
	cNPC();
	cNPC(uint32_t serial);
	virtual ~cNPC();

	virtual const std::string getPopupHelp() const;
	uint16_t calcAtt();

protected:
	pChar owner;	//! NPC's owner
	virtual uint16_t eventsNumber() const
	{ return evtNpcMax; }

	uint16_t tamingNeeded; //!< Taming level required for tame this NPC
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
	static const uint64_t flagCanTrain	= 0x0000000100000000ull;
	static const uint64_t flagShopKeeper	= 0x0000000200000000ull;
	static const uint64_t flagDispellable	= 0x0000000400000000ull;
	static const uint64_t flagWaterWalk	= 0x0000000800000000ull; //!< Char walks on water

	inline bool canTrain() const
	{ return flags & flagCanTrain; }

	inline void setCanTrain(bool on = true)
	{ setFlag(flags, flagCanTrain, on); }

	inline bool isShopKeeper() const
	{ return flags & flagShopKeeper; }

	inline void setShopKeeper(bool on = true)
	{ setFlag(flags, flagShopKeeper, on); }

	inline bool isDispellable() const
	{ return flags & flagDispellable; }

	inline void setDispellable(bool on = true)
	{ setFlag(flags, flagDispellable, on); }

	inline bool canWaterWalk() const
	{ return flags & flagWaterWalk; }

	inline void setCanWaterWalk(bool on = true)
	{ setFlag(flags, flagWaterWalk, on); }

//@}

//@{
/*!
\name Stabling
*/
protected:
	pNPC stablemaster;	//!< The stablemaster
public:
	//! Check if char is stabled
	inline bool isStabled() const
	{ return stablemaster; }

	//! Get the character's stablemaster
	inline pNPC getStablemaster() const
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
protected:
	void beginCasting(pChar target, SpellId spell);
	void checkAI();
	NpcAi ai;

	class cPath*	path;			//!< current path
	void		walkNextStep();		//!< walk next path step
public:
	NpcAi getAIType() const
	{ return ai; }

	//! has a path set?
	inline bool hasPath() const
	{ return path; }

//@}
};

#endif
