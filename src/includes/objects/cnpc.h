/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
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

class cNPC;
typedef cNPC *pNPC;

/*!
\brief This class represent a non-playing character.

In this class there are members used only for NPCs, and not for playing
characters.
*/
class cNPC : cChar
{
public:
	inline cNPC() : cChar()
	{ }

	inline cNPC() : cChar(serial)
	{ }

	virtual ~cNPC();

	virtual UI32 rtti()
	{ return rtti::cNPC; }

protected:
	pChar owner;	//! NPC's owner

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

};

#endif