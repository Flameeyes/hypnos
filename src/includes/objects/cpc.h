/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declaration of cPC class
*/

#ifndef __CPC_H__
#define __CPC_H__

class cPC;
typedef cPC *pPC;

/*!
\brief This class represent a playing character.

In this class there are members used only for PCs, and not for NPCs.
*/
class cNPC
{
public:
	inline cNPC() : cChar()
	{ }

	inline cNPC(UI32 serial) : cChar(serial)
	{ }

protected:
	//! Heartbeat for PCs
	void		heartbeat();

	UI32		lastNpcCheck;
		//!< timestamp used to reduce # of checkNPC calls in checkAuto() to 1 (Sparhawk)

	Location 	workloc;	//!< work location (not used for now)
	Location 	homeloc;	//!< home location (not used for now)
	Location 	foodloc;	//!< food location (not used for now)
	TIMERVAL	nextAiCheck;

	SI32		hidamage;	//!< NPC Hi Damage
	SI32		lodamage;	//!< NPC Lo Damage

	SI32		npcaitype;	//!< NPC ai

	TIMERVAL	npcmovetime; // Next time npc will walk
	char		npcWander; // NPC Wander Mode
	char		oldnpcWander; // Used for fleeing npcs
	SI32		fleeTimer;
	R32		npcMoveSpeed; // Used to controll npc walking speed
	R32		npcFollowSpeed; // Used to controll npc walking speed when following a target (npcwander = 1)
	SI32		fx1; //NPC Wander Point 1 x
	SI32		fx2; //NPC Wander Point 2 x
	SI32		fy1; //NPC Wander Point 1 y
	SI32		fy2; //NPC Wander Point 2 y
	signed char	fz1; //NPC Wander Point 1 z

	SI32		magicsphere; // for npc ai

	SERIAL		spawnserial; //!< Spawned by dinamic
	SERIAL		spawnregion; //!< Spawned by scripted

	SI08		npc_type;
		//!< currently only used for stabling, (type==1 -> stablemaster)
		//!< can be used for other npc types too of course

public:
	void		setNpcMoveTime();
	void		npcSimpleAttack(pChar pc_target);

	inline const bool canTrain() const
	{ return flags & flagCanTrain; }

	inline void setCanTrain(bool set = true)
	{ setFlag(flagCanTrain, set); }
};

#endif