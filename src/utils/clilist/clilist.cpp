/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator - Utilities Package                            |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypnos/muls/cliloc.hpp"

#ifdef HAVE_IOSTREAM
	#include <iostream>
	using std::cerr;
	using std::cout;
	using std::endl;
#elif defined HAVE_IOSTREAM_H
	#include <iostream.h>
#endif

#ifdef HAVE_IOMANIP
	#include <iomanip>
	using std::setw;
	using std::setfill;
#elif defined HAVE_IOMANIP_H
	#include <iomanip.h>
#endif

int main(int argc, char *argv[])
{
	if ( argc != 2 )
	{
		cerr << "You need to supply the filename for the cliloc to list." << endl
			<< "Usage: clilist clilocfile" << endl;
		
		return -1;
	}
	
	nLibhypnos::nMULFiles::fCliloc cli(argv[1]);
	
	for(constCliIterator it = cli.getEntries().begin(); it != cli.getEntries().end(); it++ )
	{
		cout << "0x" << std::hex << setw(8)
			<< setfill('0') << (*it).first << '\t' << (*it).second
			<< endl;
	}
}
