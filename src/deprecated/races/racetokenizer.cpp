  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "nxwcommn.h"

#include "racetokenizer.h"

Tokenizer::Tokenizer( const char* initialLine )
{
/*
	line = initialLine;
	tokenStart = 0;
	tokenLength = 0;
*/
	uint32_t	currentTokenId	= 1;
	uint32_t	tokenStart	= 0;
	uint32_t	tokenLength	= 0;
	string	line		= initialLine;
	uint32_t	lineSize 	= line.size();
	string* token		= 0;

	while ( tokenStart != lineSize )
	{
		while ( tokenStart < lineSize && isspace( line[tokenStart] ) )
			tokenStart++;
		while ( tokenStart + tokenLength < lineSize && !isspace( line[tokenStart+tokenLength] ))
			tokenLength++;

		if ( tokenStart != lineSize )
		{
			token = new string( line.substr( tokenStart, tokenLength ) );
			tokens[currentTokenId] = token;
			++currentTokenId;
		}
		tokenStart	+= tokenLength;
		tokenLength	= 0;
	}
	index = 1;
}

uint32_t Tokenizer::count( void )
{
	return tokens.size();
}

bool Tokenizer::setIndex( uint32_t newValue )
{
	bool returnValue = false;
	if ( newValue > 1 && newValue <= tokens.size() )
	{
		returnValue = true;
		this->index = newValue;
	}
	return returnValue;
}

string* Tokenizer::first( void )
{
	string* returnValue;

	if ( this->count() )
	{
		this->index = 1;
		returnValue = tokens[this->index];
	}
	else
		returnValue = new string( "" );

	return returnValue;
}

string* Tokenizer::next( void )
{
	string* returnValue;

	if ( this->index < this->count() )
	{
		++this->index;
		returnValue = tokens[this->index];
	}
	else
		returnValue = new string( "" );

	return returnValue;
}

string* Tokenizer::previous( void )
{
	string* returnValue;

	if ( this->index > 1 )
	{
		--this->index;
		returnValue = tokens[this->index];
	}
	else
		returnValue = new string( "" );

	return returnValue;
}

string* Tokenizer::current( void )
{
	string* returnValue;

	if ( this->index > 1 && this->index <= tokens.size() )
		returnValue = tokens[this->index];
	else
		returnValue = new string( "" );

	return returnValue;
}

