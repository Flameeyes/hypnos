#include <list>
#include <iostream>
#include <stdint.h>

static const uint64_t elements = 260000;

int main()
{
	std::cout << "Testing std::list behaviour." << std::endl
		<< "Filling a list of " << elements << " characters" << std::endl;

	std::list<char> l;
	for(register uint64_t i = 0; i < elements; i++ )
	{
		char c = 'a' + i%26;
		l.push_back(c);
	}

	for(std::list<char>::iterator it = l.begin(); it != l.end(); it++)
	{
		std::cout << (*it);
	}

	std::cout << std::endl;

	char c;
	//std::cin.get(c);
}

