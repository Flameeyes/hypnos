/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypnos/muls/mmappedfile.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_WINBASE_H
#include <winbase.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

namespace nLibhypnos {

	/*!
	\brief Default constructor for mmapped files
	\param recordsize Size of a single mmapped record (used by getCount())
	*/
	cMMappedFile::cMMappedFile(uint16_t recordsize)
		: recSize(recordsize), array(NULL)
	{
	}
	
	/*!
	\brief Constructor for mmapped files
	\param recordsize Size of a single mmapped record (used by getCount())
	\param filename Valid filename of the file to open.
	\param offset Offset in the file to start mmapping
	\param length Part of the file to mmap. If 0 all the file will be
		mmapped.
	
	\note The parameter are passed to the open() and mmap() functions. If
		a subclass (i.e.: the verdata file reader) needs to check an
		offset before the file can be mmapped, \b must call the default
		constructor which does nothing.
	*/
	cMMappedFile::cMMappedFile(uint16_t recordsize, string filename, uint32_t offset, uint32_t length)
		: recSize(recordsize), array(NULL)
	{
		open(filename);
		mmap(offset, length);
	}
	
	/*!
	\brief Destructor for mmaped files
	
	This function closes the file descriptor and unmap the file from the
	memory.
	*/
	cMMappedFile::~cMMappedFile()
	{
		#ifdef HAVE_MUNMAP
		munmap(array, getSize());
		#elif defined (WIN32)
		UnmapViewOfFile(array);
		#endif
		close(fd);
	}
	
	/*!
	\brief Open a file to be mmapped
	\param filename Name of the file to open
	
	\note The mmapped files are all opened read only.
	*/
	void cMMappedFile::open(string filename)
	{
		fd = ::open(filename.c_str(), O_RDONLY);
		if ( fd == -1 )
		{
			/*!
			\todo Here we should check for which error is set in
			errno and then throw the right exception to the
			exception handler.
			*/
		}
		
		#ifdef WIN32
		fn = filename;
		#endif
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
	void cMMappedFile::mmap(uint32_t offset, uint32_t length)
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
		#ifdef HAVE_MMAP
		array = ::mmap(NULL, length, PROT_READ, MAP_SHARED, fd, offset);
		#elif defined(WIN32)
		HANDLE hMapObject = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READ, 0, SHMEMSIZE, fn.c_str());
		if ( ! hMapObject )
		{
			/*!
			\todo Here we should throw an exception of unable-to-mmap
			*/
			array = NULL;
			return;
		}
		
		array = MapViewOfFile(hMapObject, FILE_MAP_READ, 0, offset, length);
		if ( ! array )
			/*!
			\todo Here we should throw an exception of unable-to-mmap
			*/
			CloseHandle(hMapObject);
		#endif
		
		size = length;
	}
}
