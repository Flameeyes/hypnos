#include <cstdio>
#include <iostream>
#include "gumpidx.h"

FILE *fp;

void findData(uint16_t gumpID)
{
	cGumpIDX buffer;
	
	if ( fseek(fp, gumpID*sizeof(cGumpIDX), SEEK_SET) == -1 )
	{
		std::cerr << "Error seeking file" << std::endl;
		return;
	}
	
	size_t count = fread(&buffer, 1, sizeof(cGumpIDX), fp);
	if ( count != sizeof(cGumpIDX) )
	{
		std::cerr << "Error reading file (read " << count << " bytes instead of " << sizeof(cGumpIDX) << ")" << std::endl;
		return;
	}
	
	std::cout << "Find out data: " << std::endl
		<< "\tLookup : " << std::hex << buffer.getLookup() << std::endl
		<< "\tSize : " << buffer.getSize() << std::endl
		<< "\tHeight : " << buffer.getHeight() << std::endl
		<< "\tWidth : " << buffer.getWidth() << std::endl
		<< std::endl;
}

int main(int argc, char *argv[])
{
	std::cout << "Size of cGumpIDX " << sizeof(cGumpIDX) << std::endl;

	if ( argc < 3 ) return -1;
	
	int which = atoi(argv[2]);
	
	fp = fopen(argv[1], "r");
	if ( ! fp )
	{
		std::cerr << "Error opening file " << argv[1] << std::endl;
		return -1;
	}
	
	for(int i = which; i >= 0; i--)
		findData(i);
	
	fclose(fp);
	
	return 0;
}
