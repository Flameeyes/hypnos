  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "common_libs.h"
#include "sndpkg.h"
#include "rcvpkg.h"
#include "srvparms.h"
#include "amx/amxcback.h"
#include "debug.h"
#include "npcai.h"
#include "set.h"


#include "basics.h"
#include "inlines.h"
#include "classes.h"
#include "skills.h"
#include "nox-wizard.h"

void profileStuff( NXWCLIENT ps, cPacketCharProfileReq& p )
{

	if( ps==NULL ) return;
	pChar pc= ps->currChar();
	VALIDATEPC( pc );
	
	pChar who=pointers::findCharBySerial( p.chr.get() );
	VALIDATEPC( who );

	if( p.update ) { //update profile
		if( ( who->getSerial()!=pc->getSerial32() ) && !pc->IsGMorCounselor() ) 
			return; //lamer fix
		who->profile = p.profile;
	}
	else { //only send
		cPacketCharProfile resp;
		resp.chr=p.chr;
		resp.title+= who->getCurrentName();
		resp.staticProfile = who->staticProfile;
		resp.profile = &who->profile;
		resp.send( ps );

	}
					
}

