/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypnos/muls/mmappedfile.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

namespace nLibhypnos {
template<class MUL> {

	/*!
	\brief Default constructor for mmapped files
	*/
	tMMappedFile::tMMappedFile()
	{
		array = NULL;
	}
	
	/*!
	\brief Constructor for mmapped files
	\param filename Valid filename of the file to open.
	\param offset Offset in the file to start mmapping
	\param length Part of the file to mmap. If 0 all the file will be
		mmapped.
	
	\note The parameter are passed to the open() and mmap() functions. If
		a subclass (i.e.: the verdata file reader) needs to check an
		offset before the file can be mmapped, \b must call the default
		constructor which does nothing.
	*/
	tMMappedFile::tMMappedFile(std::string filename, uint32_t offset, uint32_t length)
	{
		array = NULL;
		open(filename);
		mmap(offset, length);
	}
	
	/*!
	\brief Destructor for mmaped files
	
	This function closes the file descriptor and unmap the file from the
	memory.
	*/
	tMMappedFile::~tMMappedFile()
	{
		munmap(array);
		close(fd);
	}
	
	/*!
	\brief Open a file to be mmapped
	\param filename Name of the file to open
	
	\note The mmapped files are all opened read only.
	*/
	void tMMappedFile::open(std::string filename)
	{
		fd = open(filename.c_str(), O_RDONLY);
		if ( fd == -1 )
		{
			/*!
			\todo Here we should check for which error is set in
			errno and then throw the right exception to the
			exception handler.
			*/
		}
	}
	
	/*!
	\brief Actually mmap the file in the memory
	\param offset Offset to start the mmap to
	\param length Length of the mmapped area. If 0, all the file will be
		mmaped (minus the offset if present)
	
	This function does all the magic about the mmapping of the files, and
	is called by the parameterized constructor. Subclasses which wants to
	open the file and read something from that before mmap (to load offsets
	or length) can call it if it wasn't called by the constructor.
	*/
	void tMMappedFile::mmap(uint32_t offset, uint32_t length)
	{
		if ( array )
		{
			/*!
			\todo Here we should throw an exception for a try to
			mmap a file more than once, which is illegal.
			*/
			return;
		}
		
		if ( ! length ) // Get file size only if no length is passed
		{
			struct stat info;
			int ret = fstat(fd, &info);
			if ( ret == -1 )
			{
				/*!
				\todo Here we should check for which error is set in
				errno and then throw the right exception to the
				exception handler
				*/
				return;
			}
			
			length = info.st_size - offset;
		}
		
		// And here we mmap the file
		array = (MUL*)mmap(NULL, length, PROT_READ, MAP_SHARED, fd, offset);
	}
};
}
