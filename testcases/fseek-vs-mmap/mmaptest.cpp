#include <iostream>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "gumpidx.h"

cGumpIDX *gumpidx;
struct stat info;

void findData(uint16_t gumpID)
{
	static int count = info.st_size / sizeof(cGumpIDX);
	if ( gumpID >= count )
	{
		std::cerr << "Index " << gumpID << "out of bound." << std::endl;
		return;
	}
	
	std::cout << "Find out data: " << std::endl
		<< "\tLookup : " << std::hex << gumpidx[gumpID].getLookup() << std::endl
		<< "\tSize : " << gumpidx[gumpID].getSize() << std::endl
		<< "\tHeight : " << gumpidx[gumpID].getHeight() << std::endl
		<< "\tWidth : " << gumpidx[gumpID].getWidth() << std::endl;
	
}

int main(int argc, char *argv[])
{
	std::cout << "Size of cGumpIDX " << sizeof(cGumpIDX) << std::endl;

	if ( argc < 3 ) return -1;
	
	int which = atoi(argv[2]);
	
	int fd = open(argv[1], O_RDONLY);
	if ( fd == -1 )
	{
		std::cerr << "Error opening file " << argv[1] << std::endl;
		return -1;
	}
	
	fstat(fd, &info);
	
	gumpidx = (cGumpIDX*)mmap(NULL, info.st_size, PROT_READ, MAP_SHARED, fd, 0);
	
	for(int i = which; i >= 0; i--)
		findData(i);
	
	munmap(gumpidx, info.st_size);
	close(fd);
	
	return 0;
}
