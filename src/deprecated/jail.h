/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file jail.h
\brief Definition of classes cPrison for manage jail
\todo complete documentation
*/
#ifndef __JAIL_H__
#define __JAIL_H__

 /*
\class cJailed
\author Endymion

the jailed
*/
class cJailed
{
public:
	cJailed();
	~cJailed();
	
	uint32_t	serial;		//!< serial of char jailed
	Location oldpos;	//!< old position
	std::string	why;	//!< guilty description
	uint32_t	sec;		//!< duration of jail
	uint32_t	timer;		//!< timer for jail
	uint32_t	cell;		//!< cell

};

typedef std::vector< cJailed > JAILEDVECTOR;

/*
\class cPrisonCell
\author Endymion

a cell of the prison
*/
class cPrisonCell
{
public:
	cPrisonCell();
	~cPrisonCell();

	uint32_t serial;		//!< serial of the cell
	Location pos;		//!< position of the cell
	bool free;		//!< free

};

	
typedef std::vector<cPrisonCell> PRISONCELLVECTOR;
typedef std::vector<cJailed> JAILEDVECTOR;

/*
\class cPrison
\author Endymion

manage the jail
*/
namespace prison {

	extern PRISONCELLVECTOR cells;	//!< list of cells
	extern JAILEDVECTOR jailed;	//!< list of jailed people

	void archive();
	void safeoldsave();
	void jail( pChar jailer, pChar pc, uint32_t secs );
	void release( pChar releaser, pChar pc );
	void checkForFree();
	void freePrisonCell( uint32_t cell );
	void addCell( uint32_t serial, uint32_t x, uint32_t y, uint32_t z );
	void standardJailxyz (int jailnum, int& x, int& y, int& z);

};

#endif
