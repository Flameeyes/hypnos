  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/*!
\file
\brief Target Related Stuff
*/

#ifndef __TARGET_H__
#define __TARGET_H__

#include "common_libs.h"

typedef void ( cClient::*processTarget ) ( pTarget );

class cTarget {

private:
	static uint32_t serial_current;

protected:
	Location loc;
	uint16_t model;
	uint32_t clicked;

public:
	uint32_t serial;
	bool type;

	uint32_t buffer[4];
	std::string buffer_str[4];
	processTarget code_callback;

	cTarget( bool selectLocation=true );
	virtual ~cTarget();

	void send( pClient client );
	void receive( pClient client );
	virtual bool isValid();
	virtual void error( pClient client );

	Location getLocation();
	uint32_t getClicked();
	uint16_t getModel();

};

class cObjectTarget : public cTarget {

public:

	cObjectTarget();
	~cObjectTarget();

	virtual bool isValid();
	virtual void error( pClient client );
};

class cCharTarget : public cObjectTarget {

public:

	cCharTarget();
	~cCharTarget();

	virtual bool isValid();
	virtual void error( pClient client );
};

class cItemTarget : public cObjectTarget {
	
public:

	cItemTarget();
	~cItemTarget();

	virtual bool isValid();
	virtual void error( pClient client );
};

class cLocationTarget : public cTarget {

public:

	cLocationTarget();
	~cLocationTarget();

	virtual bool isValid();
	virtual void error( pClient client );
};


void amxCallbackOld( pClient client, pTarget t );
void amxCallback( pClient client, pTarget t );


typedef enum {
	TARG_ALL = 0,
	TARG_OBJ,
	TARG_CHAR,
	TARG_ITEM,
	TARG_LOC
} TARG_TYPE;


pTarget createTarget( TARG_TYPE type );

class TargetLocation
{
private:
	pChar	m_pc;
	int	m_pcSerial;
	pItem	m_pi;
	int	m_piSerial;
	int	m_x;
	int	m_y;
	int	m_z;
	void   init(int x, int y, int z);
	void   init(pChar pc);
	void   init(pItem pi);

public:
	//!creates a target loc from a character
	TargetLocation(pChar pc) { init(pc); }
	//!creates a target loc from an item
	TargetLocation(pItem pi) { init(pi); }
	//!creates a target loc from a target net packet
	TargetLocation( pTarget pp );
	//!creates a target loc from a xyz position in the map
	TargetLocation(int x, int y, int z) { init(x,y,z); }

	//!recalculates item&char from their serial
	void revalidate();

	//!gets the targetted char if any, NULL otherwise
	inline pChar getChar() { return m_pc; }
	//!gets the targetted item if any, NULL otherwise
	inline pItem getItem() { return m_pi; }
	//!gets the XYZ location of this target location
	inline void getXYZ(int& x, int& y, int& z) { x = m_x; y = m_y; z = m_z; }
	//!extends a pItem data to pChar and x,y,z
	void extendItemTarget();
};

#endif
