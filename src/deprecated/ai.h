  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#ifndef __AI_H__
#define __AI_H__

#include "common_libs.h"
#include "objects/cchar.h"

/*!
\file
\author Luxor
\brief Header of Artificial Intelligence implementation
*/

//@{
/*!
\author Luxor
\name Fuzzy logic implementation for NPCs
*/

/*!
\author Luxor
\brief Base class of possible actions that would be chosen by an AI
*/
/*class cAction {
private:
	cAI *m_ai;
public:
	cAction( cAI* ai );
	virtual void run();
};*/

/*!
\author Luxor
*/
/*class cAttackAction : public cAction {
private:
	uint32_t m_target;
public:
	void run();
	inline pChar getTarget() { return pointers::findCharBySerial( m_target ); }
};*/

/*!
\author Luxor
*/
/*class cDefendAction : public cAction {
private:
	uint32_t m_attacker;
public:
	void run();
	inline pChar getAttacker() { return pointers::findCharBySerial( m_attacker ); }
};*/

/*!
\author Luxor
\brief Base class of AI implementation
*/
/*class cAI {
private:
	uint32_t m_pc;
	cAction *currAction;
public:
	cAI( cChar &pc, int32_t type );
	virtual void run();
	cAction* getAction()
	void setAction();
	inline bool hasAction() { return ( currAction != NULL ); }
	inline pChar getChar() { return pointers::findCharBySerial( m_pc ); }
};*/


//@}


//@{
/*!
\author Luxor
\name Pathfinding method implementation
*/
#include "common_libs.h"

#define MAX_PATH_INTERNAL_LOOPS 200
#define MAX_PATH_TOTAL_LOOPS 10000
#define Z_COST 1
#define STRAIGHT_COST 10
#define OBLIQUE_COST 14


/*!
\author Luxor
*/
struct path_node {
	uint32_t cost;
	Location pos;
	path_node *parentNode;
};

typedef slist<path_node*> NODE_LIST;
typedef slist<Location> LOCATION_LIST;

/*!
\brief Class implementation of a pathfinding algorithm similar to an A* one
\author Luxor
*/
class cPath {
public:
	cPath( Location startPos, Location finalPos, pChar pc = NULL );
	void exec();
	Location getNextPos();
	inline Location getFinalPos() { return m_finalPos; }
	bool targetReached();
	inline bool pathFound() { return m_pathFound; }
private:
	bool m_pathFound;
	uint32_t m_loops;
	uint32_t pc_serial;
	path_node* currNode;
	path_node* nextNode;
	path_node* create_node( Location pos, path_node* parentNode, uint32_t cost );
        std::queue<path_node> nodes_vector;
	uint8_t addReachableNodes( path_node* node );
	void dropToClosedList( path_node* node );
	void addToOpenList( Location pos, path_node* parentNode, uint32_t cost = STRAIGHT_COST );
	void addToOpenList( path_node* node );
	void addToClosedList( path_node* node );
	Location m_startPos;
        Location m_finalPos;
	NODE_LIST open_list;
	NODE_LIST closed_list;
	LOCATION_LIST path_list;
};

//@}


#endif //__AI_H__
