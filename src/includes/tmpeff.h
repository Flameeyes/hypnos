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
\brief Temp Effect Related Stuff
*/

#ifndef __TMPEFF_H__
#define __TMPEFF_H__

namespace tempfx {
	
enum {
	MODE_START = 0,
	MODE_END,
	MODE_ON,
	MODE_OFF,
	MODE_REVERSE
};

bool	add(P_OBJECT src, P_OBJECT dest, int num, unsigned char more1 = 0, unsigned char more2 = 0, unsigned char more3 = 0,short dur=INVALID, int amxcback = INVALID);
bool	isSrcRepeatable(int num);
bool	isDestRepeatable(int num);
void	addTempfxCheck( SERIAL serial );

	
/*
\brief A Temp effect
\author Luxor
*/

class cTempfx {
private:
	SERIAL m_nSrc;
	SERIAL m_nDest;
	int32_t m_nNum;
	int32_t m_nMode;
	uint32_t m_nExpireTime;
	int32_t m_nAmxcback;
	int32_t m_nMore1;
	int32_t m_nMore2;
	int32_t m_nMore3;
	bool m_bDispellable;
	bool m_bSrcRepeatable;
	bool m_bDestRepeatable;
public:
	cTempfx( SERIAL nSrc, SERIAL nDest, int32_t num, int32_t dur, int32_t more1, int32_t more2, int32_t more3, int32_t amxcback );
	int getExpireTime() { return m_nExpireTime; }
	bool isDispellable() { return m_bDispellable; }
	void activate();
	void deactivate();
	bool isValid();
	int8_t checkForExpire();
	void executeExpireCode();
	void start();
	int getNum() { return m_nNum; }
	SERIAL getAmxCallback() { return m_nAmxcback; }
};

void tempeffectson();
void tempeffectsoff();
void checktempeffects();

	//End Luxor's temp effects system
}

#endif //__TMPEFF_H__
