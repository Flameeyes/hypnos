/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator - Utilities Package                            |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypnos/muls/cliloc.h"

#include <iostream>
#include <iomanip>

int main(int argc, char *argv[])
{
	if ( argc != 2 )
	{
		std::cerr << "You need to supply the filename for the cliloc to list." << std::endl
			<< "Usage: clilist clilocfile" << std::endl;
	}
	
	nLibhypnos::nMULFiles::fCliloc cli(argv[1]);
	
	for(register int i = 0; i < cli.getCount(); i++)
	{
		std::cout << "0x" << std::hex << std::setw(8)
			<< std::setfill('0') << i << '\t' << cli.getEntry(i)
			<< std::endl;
	}
}
