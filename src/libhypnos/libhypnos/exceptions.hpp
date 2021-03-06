/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator (Utility Library)                              |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in LICENSE file.               |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPNOS_EXCEPTIONS_H__
#define __LIBHYPNOS_EXCEPTIONS_H__

#include "libhypnos/commons.hpp"
#include "libhypnos/hypstl/string.hpp"

namespace nLibhypnos {
	/*!
	\class eException exceptions.h "libhypnos/exceptions.h"
	\brief Base class for Hypnos' exceptions
	
	This class is inherited by all the specific exceptions thrown by Hypnos emulator
	(and by libhypmul), which can be used by the exception handler to find all the
	exception which wasn't handled in other ways (at least the ones thrown by Hypnos
	and not by external libraries).
	*/
	class eException {
	protected:		
		eException() { }
		virtual ~eException() { }
	};

	/*!
	\class eOutOfBound exceptions.h "libhypnos/exceptions.h"
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
		\param aMax Value to assign to \c max attribute
		\param aRequested Value to assign to \c requested attribute
		*/
		eOutOfBound(uint32_t aMax, uint32_t aRequested)
			: max(aMax), requested(aRequested)
		{ }
	};
	
	/*!
	\class eInvalidIP exceptions.h "libhypnos/exceptions.h"
	\brief Exception thrown when trying to convert a string which isn't a 
		valid dotted-decimal IP.
	*/
	class eInvalidIP : public eException {
	public:
		const string invalidIP; //!< String which isn't a valid IP
		
		/*!
		\brief Constructor
		\param aInvalidIP Value to assign to \c invalidIP attribute
		*/
		eInvalidIP(string aInvalidIP)
			: invalidIP(aInvalidIP)
		{ }
	};
}

#endif
