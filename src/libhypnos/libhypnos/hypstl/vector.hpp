/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator (Utility Library)                              |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in LICENSE file.               |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPNOS_HYPSTL_VECTOR_HPP__
#define __LIBHYPNOS_HYPSTL_VECTOR_HPP__

#ifdef HAVE_VECTOR
	#include <vector>
	using std::vector;
#elif defined HAVE_VECTOR_H
	#include <vector.h>
#endif

typedef vector<uint32_t> uint32_vector;		//!< A vector of uint32_t
typedef vector<struct sPoint> PointVector;	//!< Vector of (x,y) points (uint16_t coords)

namespace nLibhypnos {
	typedef vector<class cVariant *> cVariantVector;	//!< Pointer to a variant instance
}

#ifdef HYPNOS_SOURCES
typedef vector<class cPC *> PCVector;		//!< Vector of playing characters
#endif

#endif
