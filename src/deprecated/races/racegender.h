/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __racegender__
#define __racegender__

#include "common_libs.h"

typedef enum { FEMALE, MALE, FEMALE_OR_MALE } GENDER;

class Gender
{
private:
	static	string genderNames[];
private:
	GENDER			value;
public:
	Gender( void );
	Gender( const char* initialGender );
	GENDER getValue( void );
	GENDER setValue( const GENDER newGender );
	GENDER setValue( const string& newGender );

	bool operator==( Gender that );
	bool operator==( GENDER that );
	bool operator==( int that );
	operator int() const;
	operator char*() const;
	operator string*() const;
};


#endif
