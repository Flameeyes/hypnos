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
class cPC : cChar
{
public:
	inline cPC() : cChar()
	{ }

protected:
	//! Heartbeat for PCs
	void		heartbeat();

public:
	bool		updateFlag();

	//! Makes a character temporary grey
	inline void setGrey()
	{ tempfx::add(this, this, tempfx::GREY, 0, 0, 0, 0x7FFF); }
        void deadAttack (pChar victim);  //if dead and trying to attack someone...

//@{
/*!
\name Trainer
*/
public:
	pNPC	trainer;		//!< NPC training the char
	UI08	trainingplayerin;	//!< Index in skillname of the skill the NPC is training the player in

public:
	inline const bool	isBeingTrained() const
	{ return trainer; }

	inline const pChar	getTrainer() const
	{ return trainer; }

	inline const UI08	getSkillTaught() const
	{ return trainingplayerin; }
//@}

//@{
/*!
\name Flags
*/

public:
	static const UI64 flagCanViewSerials	= 0x0000000100000000ull; //!< Char can view serial codes
	static const UI64 flagPageAble		= 0x0000000200000000ull;
	static const UI64 flagViewHouseIcon	= 0x0000000400000000ull;
	static const UI64 flagAllMove		= 0x0000000800000000ull;
	static const UI64 flagCanSnoopAll	= 0x0000001000000000ull;
	static const UI64 flagBroadcast		= 0x0000002000000000ull;

	inline const bool canBroadcast() const
	{ return flags & flagBroadcast; }

	inline void flagBroadcast(bool set = true)
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
	TIMERVAL creationday;
public:
	//! Set the creation day of a character
	inline void setCreationDay(TIMERVAL day)
	{ creationday = day; }

	//! Get the creation day of a character
	inline const TIMERVAL getCreationDay() const
	{ return creationday; }
//@}

//@{
//! \name Bank

	//! Counts the gold in the bankbox
	inline UI32 countBankGold()
	{
		return  getBankBox(bankGold) ?
			getBankBox(bankGold)->countItems(ITEMID_GOLD)
			: 0;
	}
//@}
};

#endif