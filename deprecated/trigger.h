  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#ifndef __TRIGGER_H__
#define __TRIGGER_H__

#include "scp_parser.h"

class cTriggerContext
{
		char m_szFailMsg[50];
		char m_szDisableMsg[50];
		int m_nColor1, m_nColor2;

		int m_nTriggerType;

		NXWSOCKET  m_socket;
		pItem m_pi;
		pItem m_piEnvoked; //!< the envoker item
		pItem m_piAdded; //!< the added item
		pItem m_piNeededItem;
		pChar m_pcNpc;
		pChar m_pcCurrChar;
		pChar m_pcAdded;
		bool m_bStop;
		int m_nNumber;
		cScpIterator* m_iter;

		void parseIfCommand(char* cmd, char* par, int comparevalue);
		void parseIAddCommand(char* par);
		void parseMaxDurCommand(pItem pi, char* par);
		void parseDurCommand(pItem pi, char* par);
		void parseLine(char* cmd, char* par);
		void init(int number, NXWSOCKET  s, int trigtype, uint16_t id = 0);
		void checkPtrsValidity();

	public:
		cTriggerContext(int number, NXWSOCKET  s, pItem itm, int trigtype);
		cTriggerContext(int number, NXWSOCKET  s, pChar itm, int trigtype);
		~cTriggerContext();
		void exec();
};

//Trigger routines
void triggernpc(NXWSOCKET  ts,int ti, int ttype);  // trigger.cpp --- Changed by Magius(CHE) §
bool checkenvoke( uint16_t eid );

#define TRIGTYPE_DBLCLICK 1
#define TRIGTYPE_WALKOVER 2
#define TRIGTYPE_NPCWORD 3
#define TRIGTYPE_NPCTARG 4
#define TRIGTYPE_ENVOKED 64
#define TRIGTYPE_NPCENVOKED 65
#define TRIGTYPE_TARGET 128

//@{
/*!
\name Entry points
*/
void triggerItem(NXWSOCKET ts, pItem pi, int eventType);
void triggerNpc(NXWSOCKET ts, pChar pc, int eventType);
void triggerTile(NXWSOCKET ts);
//@}

extern bool g_bStepInTriggers;

#endif //__TRIGGER_H__
