/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator (Utility Library)                              |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in LICENSE file.               |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPNOS_COMMONS_H__
#define __LIBHYPNOS_COMMONS_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// Seems like MinGW have trouble handling that...
#ifdef WIN32
#undef malloc
#endif

/*!
\file
\brief Common abstraction for libhypnos

This file contains some common abstraction code for both libhypnos and
Hypnos Server.

I splitted this out of common_libs to allow other utility programs of Hypnos'
suite to access the common defines without need of complete hypnos sources.
*/

#if defined(DOXYGEN)
	/*!
	\brief Defines a structure that needs the packaging
	
	This attribute is used for structs in \ref data.h which loasd the data
	raw from the files, and needs to not be padded.
	*/
	#define PACK_NEEDED
	
	//! Declare a function as deprecated
	#define DEPRECATED
	
	//! Define a function as pure (that don't use external source variables)
	#define PURE
	
	//! Define a function to conforms to the printf arguments formatting
	//! \see http://gcc.gnu.org/onlinedocs/gcc-3.3.3/gcc/Function-Attributes.html#Function%20Attributes
	#define PRINTF_LIKE(A,B) 

#elif defined( __GNUC__ ) || defined(__ICC)
// Intel C++ Compiler supports attributes (seems)
	#define PACK_NEEDED __attribute__ ((packed))
	#define DEPRECATED __attribute__ ((deprecated))
	#define PURE __attribute__ ((pure))
	#define PRINTF_LIKE(A,B) __attribute__ ((format (printf, A, B)))
#else
	#define PACK_NEEDED
	#define DEPRECATED
	#define PURE
	#define PRINTF_LIKE(A,B) ;
#endif

#ifdef HAVE_CASSERT
	#include <cassert>
#else
	#include <assert.h>
#endif

#ifdef HAVE_IOSFWD
	#include <iosfwd>
	using std::istream;
	using std::ostream;
	using std::ifstream;
	using std::ofstream;
#elif defined HAVE_IOSFWD_H
	#include <iosfwd.h>
#endif

#ifdef HAVE_ALGORITHM
	#include <algorithm>
	using std::find;
#elif defined HAVE_ALGORITHM_H
	#include <algorithm.h>
#elif defined HAVE_ALGO_H
	#include <algo.h>
#endif

#include "libhypnos/types.h"

#endif
