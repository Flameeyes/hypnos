/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPMUL_EXCEPTIONS_H__
#define __LIBHYPMUL_EXCEPTIONS_H__

#include "common_libs.h"

namespace nLibhypnos {
	/*!
	\class eOutOfBound exceptions.h "libhypnos/muls/exceptions.h"
	\brief Exception thrown when trying to access an index out of the bound
		of a vector (or a file)
		
	This exception is threw when some code is trying to access an element
	of an array or a file which is greater than the size of the object
	itself.
	*/
	class eOutOfBound : public eException {
	public:
		const uint32_t max;	//!< Maximum index of the object (size-1)
		const uint32_t requested;	//!< Index requested for access
	
		/*!
		\brief Constructor
		\param pMax Value to assign to \c max attribute
		\param pRequested Value to assign to \c requested attribute
		*/
		eOutOfBound(uint32_t pMax, uint32_t pRequested)
			: max(pMax), requested(pRequested)
		{ }
	};
}

#endif
