/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __MUSIC_H__
#define __MUSIC_H__

namespace nSkills {
//@{
/*!
\name Musicianship stuff
*/
	void PeaceMaking(pClient client);
	void PlayInstrumentWell(pClient client, pItem pi);
	void PlayInstrumentPoor(pClient client, pItem pi);
	pItem getInstrument(pClient client);
	void target_provocation1( pClient client, pTarget t );
	void target_enticement1( pClient client, pTarget t );
	void target_provocation2( pClient client, pTarget t );
	void target_enticement2( pClient client, pTarget t );
//@}
};

#endif
