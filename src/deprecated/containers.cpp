  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "common_libs.h"
#include "containers.h"
#include "basics.h"
#include "inlines.h"

CONTINFOGUMPMAP contInfoGump;
CONTINFOMAP contInfo;

void loadcontainers()
{

	cScpIterator*	iter = NULL;
	std::string	script1,
			script2;
	int32_t 		gump = INVALID;
	BasicPosition	uprleft = {INVALID,INVALID};
	BasicPosition	dwnrght = {INVALID,INVALID};
	uint32_vector	*vet = new uint32_tVECTOR;

	int cont=0;

	int loopexit=0;
	do
	{
		safedelete(iter);
		iter = Scripts::Containers->getNewIterator("SECTION CONTAINER %i", cont++);
		if( iter==NULL ) continue;

		gump = INVALID;
		uprleft.x = INVALID;
		uprleft.y = INVALID;
		dwnrght.x = INVALID;
		dwnrght.y = INVALID;
		vet->clear();

		do
		{

			iter->parseLine(script1, script2);
			if ( script1[0]!='}' && script1[0]!='{' )
			{
				if	( "ID" == script1 )
					vet->push_back( str2num( script2 ) );
				else if ( "GUMP" == script1 )
					gump = str2num( script2 );
				else if ( "X1" == script1 )
					uprleft.x= str2num( script2 );
				else if ( "Y1" == script1 )
					uprleft.y= str2num( script2 );
				else if ( "X2" == script1 )
					dwnrght.x= str2num( script2 );
				else if ( "Y2" == script1 )
					dwnrght.y= str2num( script2 );
				else
					WarnOut("[ERROR] wrong line ( %s ) parsed on containers.xss", script1.c_str() );
			}
		}
		while ( script1[0] !='}' && ++loopexit < MAXLOOPS );

		if( (gump!=INVALID) && (uprleft.x!=INVALID) && (dwnrght.x!=INVALID) && (uprleft.y!=INVALID) && (dwnrght.y!=INVALID) )
		{
			cont_gump_st dummy;

			dummy.downright = dwnrght;
			dummy.upperleft = uprleft;
			dummy.gump	= gump;

			contInfoGump[gump] = dummy;

			CONTINFOGUMPMAP::iterator iter( contInfoGump.find(gump) );
			if( iter != contInfoGump.end() )
			{
				uint32_vector::iterator ids( vet->begin() ), end( vet->end() );
				for(; ids != end; ++ids )
					contInfo[(*ids)] = iter;
			}
			else
				ConOut("[ERROR] on parse of containers.xss" );
		}
		else
			ConOut("[ERROR] on parse of containers.xss" );
	}
	while ( script1 != "EOF" && ++loopexit < MAXLOOPS );

	safedelete(iter);

//	ConOut("\n");
//	for(CONTINFOMAP::iterator debug=contInfo.begin(); debug!=contInfo.end(); debug++ )
//		ConOut( "id %i ha gump %i \n ", debug->first, (debug->second)->second.gump);



}

