  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "common_libs.h"
#include "titles.h"

#include "inlines.h"
#include "scripts.h"

void loadcustomtitle() // for custom titles
{
	int titlecount=0;
	char sect[512];
    cScpIterator* iter = NULL;
    char script1[1024];
    char script2[1024];

	for (int a=0; a<ALLSKILLS; a++)
	{
		title[a].fame[0] = 0;
		title[a].other[0] = 0;
		title[a].prowess[0] = 0;
		title[a].skill[0] = 0;
	}

	strcpy(sect,"SECTION SKILL");
    iter = Scripts::Titles->getNewIterator(sect);
	if (iter==NULL) return;

	int loopexit=0;
	do
	{
		iter->parseLine(script1, script2);
		if ((script1[0]!='}')&&(script1[0]!='{'))
		{
			if ( !strcmp(script1, "TITLE") ) {
				strcpy(title[titlecount].skill,script2);
				titlecount++;
			} else if ( !strcmp(script1, "SKIP") ) {
				strcpy(title[titlecount].skill," ");
				titlecount++;
			}
		}
	}
	while ((script1[0]!='}') && (++loopexit < MAXLOOPS) );
	safedelete(iter);

	script1[0]=0;
	titlecount=0;

	strcpy(sect,"SECTION PROWESS");
    iter = Scripts::Titles->getNewIterator(sect);
	if (iter==NULL) return;

	loopexit=0;
	do
	{
		strcpy(script1, iter->getEntry()->getFullLine().c_str());
		if ((script1[0]!='}')&&(script1[0]!='{'))
		{
			if ( !strcmp(script1, "NONE") ) ;
			else strcpy(title[titlecount].prowess,script1);
			titlecount++;
		}
	}
	while ((script1[0]!='}') && (++loopexit < MAXLOOPS) );

	safedelete(iter);

	script1[0]=0;
	titlecount=0;
	strcpy(sect,"SECTION FAME");

	iter = Scripts::Titles->getNewIterator(sect);
	if (iter==NULL) return;

	loopexit=0;
	do
	{
		strcpy(script1, iter->getEntry()->getFullLine().c_str());
		if ((script1[0]!='}')&&(script1[0]!='{'))
		{
			if ( !strcmp(script1, "NONE") ) ;
			else strcpy(title[titlecount].fame, script1);

			if (titlecount==23)
			{
				title[titlecount].fame[0] = '\0';
				strcpy(title[++titlecount].fame, script1);
			}

			titlecount++;
		}
	}
	while ((script1[0]!='}') && (++loopexit < MAXLOOPS) );
	safedelete(iter);

	script1[0]=0;
	titlecount=0;
	strcpy(sect,"SECTION OTHER");

	iter = Scripts::Titles->getNewIterator(sect);
	if (iter==NULL) return;

	loopexit=0;
	do
	{
		strcpy(script1, iter->getEntry()->getFullLine().c_str());
		if ((script1[0]!='}')&&(script1[0]!='{'))
		{
			if ( !strcmp(script1, "NONE") ) ;
			else strcpy(title[titlecount].other,script1);
			titlecount++;
		}
	}
	while ((script1[0]!='}') && (++loopexit < MAXLOOPS) );
	safedelete(iter);

}
