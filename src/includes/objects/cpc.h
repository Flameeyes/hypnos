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
//@{
/*!
\name Trainer
*/
public:
	pChar	trainer;		//!< NPC training the char
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
};

#endif