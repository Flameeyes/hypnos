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
\brief House Related Stuff
*/
#ifndef _HOUSE_H_
#define _HOUSE_H_

extern std::map< uint32_t, pChar > houses;

int 	add_hlist(int c, int h, int t);
void buildhouse( NXWCLIENT ps, P_TARGET t );
int 	check_house_decay();
void 	deedhouse(NXWSOCKET  s, pItem pi);
int 	del_hlist(int c, int h);
int		on_hlist(pItem pi, unsigned char s1, unsigned char s2, unsigned char s3, unsigned char s4, int *li);
bool house_speech( pChar pc, NXWSOCKET socket, std::string &talk);
void 	killkeys(uint32_t serial);
void 	mtarget(int s, int a1, int a2, int a3, int a4, char b1, char b2, char *txt);


void target_houseOwner( NXWCLIENT ps, P_TARGET t );
void target_houseEject( NXWCLIENT ps, P_TARGET t );
void target_houseBan( NXWCLIENT ps, P_TARGET t );
void target_houseFriend( NXWCLIENT ps, P_TARGET t );
void target_houseUnlist( NXWCLIENT ps, P_TARGET t );
void target_houseLockdown( NXWCLIENT ps, P_TARGET t );
void target_houseRelease( NXWCLIENT ps, P_TARGET t );
void target_houseSecureDown( NXWCLIENT ps, P_TARGET t );



#endif
