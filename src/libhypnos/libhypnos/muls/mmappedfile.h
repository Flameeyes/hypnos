/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPNOS_MMAPPDEFILE_H__
#define __LIBHYPNOS_MMAPPDEFILE_H__

#include "libhypnos/commons.h"

#if ! defined(HAVE_MMAP) && ! defined(WIN32)
# error "Your platform miss a way to memory map a file, or we don't know " \
	"how to do that on your platform. Please contact Flameeyes at " \
	"flameeyes@users.berlios.de with the description of your platform data."
#endif

namespace nLibhypnos {

/*!
\class tplMMappedFile mmappedfile.h "libhypnos/mmappedfile.h"
\brief Memory Mapped File
\author Flameeyes

This class is an abstraction to allow access to memory mapped files. A memory
mapped file is a file which is accessible directly by memory without need to
use the f* functions.

To have the best performance with mmapped files, they need to be not too big and
with a fixed-length record internal structure. This is a lot useful when working
with Ultima Online's index files, like gumpsidx.mul or multi.idx. These files
has a simple internal structure which referes to other bigger files usually with
variable-length records.

Because we're working with fixed-length and position records, a MMappedFile is
a template, which needs a type to use as internal structure to the file, so that
the access is done directly via this structure.

To do that, we need a byte-aligned structure, also said packed structure, that
we can cast the memory area to (actually we cast the memory area to a pointer to
that structure, so we have an array of structures). To get a packet strucutre,
we defined a PACK_NEEDED macro which must be added after the structure
(or class) declaration, which put the right attribute to it when using GCC. For
Borland compiler we need to set the makefile to always use byte-aligned
structures unless we can find a way to do that on BCC. Help on that will be
useful.

At least under Linux and MacOSX, mmapped files can be shared between two
programs which are loaded in the same system. This is on by default.

\note The libhypmul library subclasses this to add the functions which access
	the data inside the file itself.
\note The data is read in binary form from the file, so if we're in a different
	architecture with a different endianness, we can't directly get the
	attributes of the internal data structure, but we must provide some
	methods which fixes the endianness of the file to be the same of the
	system.
*/
template<typename MUL> class tplMMappedFile {
protected:
	MUL *array;	//!< Pointer to the mmapped file
	uint32_t size;	//!< Size of the mmap in bytes
	int fd;		//!< Descriptor of the mmapped file
#ifdef WIN32
	std::string fn;	//!< Name of the file (needed for Windows mmapping)
#endif
	
	void open(std::string filename);
	void mmap(uint32_t offset = 0, uint32_t length = 0);
public:
	tplMMappedFile();
	tplMMappedFile(std::string filename, uint32_t offset = 0, uint32_t length = 0);
	virtual ~tplMMappedFile();
	
	virtual uint32_t getCount() const
	{ return size / sizeof(MUL); }
};

}

#endif
