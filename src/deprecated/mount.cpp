  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "common_libs.h"
#include "network.h"
#include "sndpkg.h"
#include "npcai.h"
#include "set.h"
#include "inlines.h"
#include "utils.h"
#include "range.h"

std::map< int32_t, int32_t > mountinfo;

void loadmounts()
{

	cScpIterator* iter = NULL;
	std::string script1;
	std::string script2;
	int32_t id=INVALID;
	int32_t anim=INVALID;
	int mount=0;

	do
	{
		safedelete(iter);
		iter = Scripts::Mountable->getNewIterator("SECTION MOUNT %i", mount++);
		if( iter == 0 ) continue;

		id=INVALID;
		anim=INVALID;

		do
		{
			iter->parseLine(script1, script2);
			if	( script1 == "ANIM" )	anim = str2num(script2);
			else if ( script1 == "ID" )
			{
				id = str2num(script2);
				//ConOut("Mount %d", id );
			}
		}
		while ( script1[0] != '}' );

		if((id!=INVALID) && (anim!=INVALID))
			mountinfo[anim]=id;
	}
	while ( iter != 0 );

	safedelete(iter);

}

