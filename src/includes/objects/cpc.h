/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __OBJECTS_CPC_H__
#define __OBJECTS_CPC_H__

#include "objects/cchar.h"
#include "extras/jails.h"

/*!
\brief This class represent a playing character.

In this class there are members used only for PCs, and not for NPCs.
*/
class cPC : public cChar
{
protected:
	pAccount account;
public:
	enum {
		evtPcOnFlagChange = evtChrMax,
		evtPcMax
	};
	
	cPC(pAccount acc);
	cPC(uint32_t serial, pAccount acc);
	virtual ~cPC();
	
	inline pAccount getAccount() const
	{ return account; }
	
	uint16_t calcAtt();
	
protected:
	//! Heartbeat for PCs
	void heartbeat();
	void do_lsd();		//!< called by pc heartbeat when under lsd effects
	virtual uint16_t eventsNumber() const
	{ return evtPcMax; }

	/*!
	\brief Effect done when moving as a gm
	\li \c 0 None
	\li \c 1 Flamestrike (standard)
	\li \c 2-6 Different sparkles
	*/
	uint8_t gmMoveEff;
public:
	virtual const std::string getPopupHelp() const;
	
	bool updateFlag();

	//! Makes a character temporary grey
	//!\todo Rewrite!
	inline void setGrey()
	{ /*tempfx::add(this, this, tempfx::tmpfxGrey, 0, 0, 0, 0x7FFF);*/ }
        
	void deadAttack (pChar victim);  //if dead and trying to attack someone...
	
	bool isGM();	//!< Returns true if is a gm or gm-like player
	void doGmEffect();

//@{
/*!
\name Trainer
*/
public:
	pNPC	trainer;		//!< NPC training the char
	uint8_t	trainingplayerin;	//!< Index in skillname of the skill the NPC is training the player in

public:
	inline const bool	isBeingTrained() const
	{ return trainer; }

	inline const pNPC getTrainer() const
	{ return trainer; }

	inline const uint8_t	getSkillTaught() const
	{ return trainingplayerin; }
//@}

//@{
/*!
\name Flags
*/
public:
	static const uint64_t flagCanViewSerials	= 0x0000000100000000ull; //!< Char can view serial codes
	static const uint64_t flagPageAble		= 0x0000000200000000ull;
	static const uint64_t flagViewHouseIcon		= 0x0000000400000000ull;
	static const uint64_t flagAllMove		= 0x0000000800000000ull;
	static const uint64_t flagCanSnoopAll		= 0x0000001000000000ull;
	static const uint64_t flagBroadcast		= 0x0000002000000000ull;

	inline const bool canBroadcast() const
	{ return flags & flagBroadcast; }

	inline void setBroadcast(bool set = true)
	{ setFlag(flagBroadcast, set); }

	inline const bool canViewHouseIcon() const
	{ return flags & flagViewHouseIcon; }

	inline void setViewHouseIcon(bool set = true)
	{ setFlag(flagViewHouseIcon, set); }

	inline const bool canSeeSerials() const
	{ return flags & flagCanViewSerials; }

	inline void setCanSeeSerials(bool set = true)
	{ setFlag(flagCanViewSerials, set); }

	inline const bool canSnoop() const
	{ return flags & flagCanSnoopAll; }

	inline void setSnoopAll(bool set = true)
	{ setFlag(flagCanSnoopAll, set); }

	inline const bool canAllMove() const
	{ return flags & flagAllMove; }

	inline void setAllMove(bool set = true)
	{ setFlag(flagAllMove, set); }

//@}

//@{
/*!
\name Creation
\brief Creation date and time related
*/
protected:
	//! Day since EPOCH this character was created on
	uint32_t creationday;
public:
	//! Set the creation day of a character
	inline void setCreationDay(uint32_t day)
	{ creationday = day; }

	//! Get the creation day of a character
	inline const uint32_t getCreationDay() const
	{ return creationday; }
//@}

public:
	void sayHunger();
	
//@{
/*!
\name Reputation
*/
protected:
	Karma reputation;
	uint16_t kills;		//!< PvP Kills
	uint16_t deaths;	//!< Number of deaths
public:
	void setMurderer();
	void setInnocent();
	void setCriminal();
	void makeCriminal();
//@}

//@{
/*!
\name Jail stuff
\see nJails namespace
*/
protected:
	nJails::cJailInfo *jailInfo;		//!< Pointer to the instance which describes the jail
	nJails::sJailPositions *jailPositions;	//!< Pointer to a struct with original and jailed positions
public:
	//! Gets the pointer to the jail info
	inline nJails::cJailInfo *getJailInfo() const
	{ return jailInfo; }
	
	//! Gets the pointer to the jail positions struct
	inline nJails::sJailPositions *getJailPositions() const
	{ return jailPositions; }
	
	/*!
	\brief Sets pointers to jail stuff
	\param info New jailInfo pointer
	\param positions New jailPositions pointer
	\note info should always be NULL if jail is account-level
	*/
	inline void setJail(nJails::cJailInfo *info, nJails::sJailPositions *positions)
	{
		jailInfo = info;
		jailPositions = positions;
	}
//@}

};

#endif
