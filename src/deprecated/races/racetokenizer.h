  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#ifndef __racetokenizer__
#define __racetokenizer__

class Tokenizer
{
	private:
		map	<uint32_t, string*> tokens;
		uint32_t	index;
	public:
		Tokenizer( const char* initialLine );
		bool	setIndex( uint32_t newValue );
		uint32_t 	count( void );
		string*	first( void );
		string*	next( void );
		string*	previous( void );
		string*	current( void );
};

#endif
