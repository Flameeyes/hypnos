/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declaration of tVariant type
*/

#ifndef __TVARIANT_H__
#define __TVARIANT_H__

class tVariant;
typedef tVariant *pVariant;	//!< Pointer to a variant

#include "common_libs.h"

/*!
\brief Variant type class

This class is used both by commands and by 
*/
class tVariant
{
public:
	//! Type of acceptable values
	enum VariantTypes {
		vtNull,		//!< Null value
		vtString,	//!< String value
		vtBoolean,	//!< Boolean value
		vtUInt,		//!< unsigned integer value (see size for exact type)
		vtSInt,		//!< signed integer value (see size for exact type)
		vtPChar,	//!< pChar (and derived) value
		vtPItem,	//!< pItem (and derived) value
		vtPVoid		//!< void pointer value
	};
	
	//! Integer types sizes
	enum IntegerSizes {
		isNotInt,	//!< The variable is not an integer
		isBool,		//!< The variable is a boolean
		is8,		//!< 8-bit integer
		is16,		//!< 16-bit integer
		is32		//!< 32-bit integer
	};

//@{
/*!
\name Constructors and operators
*/
	tVariant();
	
	/*!
	\brief Constructor with value
	\param astr String to assign to the variant
	*/
	inline tVariant(const std::string &astr)
	{ tVariant(); *this = astr; }
	
	/*!
	\brief Constructor with value
	\param aval String to assign to the variant
	*/
	inline tVariant(const bool &aval);
	{ tVariant(); *this = aval; }
	
	/*!
	\brief Constructor with value
	\param aval String to assign to the variant
	*/
	inline tVariant(const uint32_t &aval);
	{ tVariant(); *this = aval; }
	
	/*!
	\brief Constructor with value
	\param aval String to assign to the variant
	*/
	inline tVariant(const int32_t &aval);
	{ tVariant(); *this = aval; }
	
	/*!
	\brief Constructor with value
	\param aptr String to assign to the variant
	*/
	inline tVariant(const void *aptr);
	{ tVariant(); *this = aptr; }
	
	/*!
	\brief Constructor with value
	\param apc String to assign to the variant
	*/
	inline tVariant(const pChar *apc);
	{ tVariant(); *this = apc; }
	
	/*!
	\brief Constructor with value
	\param api String to assign to the variant
	*/
	inline tVariant(const pItem *api);
	{ tVariant(); *this = api; }	
	
	tVariant &operator =(const std::string &astr);
	tVariant &operator =(const bool &aval);
	tVariant &operator =(const uint32_t &aval);
	tVariant &operator =(const int32_t &aval);
	tVariant &operator =(const void *aptr);
	tVariant &operator =(const pChar *apc);
	tVariant &operator =(const pItem *api);
	
	tVariant operator +(const tVariant &param) const;
	tVariant operator -(const tVariant &param) const;
	
	//! Addiction operator
	tVariant operator +=(const tVariant &param)
	{ *this = *this + param; }
	
	//! Subtraction operator
	tVariant operator -=(const tVariant &param)
	{ *this = *this - param; }
	
	//! Prefixed increment operator
	inline tVariant operator ++()
	{ return *this += 1; }
	
	//! Prefixed decrement operator
	inline tVariant operator --()
	{ return *this += 1; }
	
	tVariant operator ++(int unused);
	tVariant operator --(int unused);
	bool operator ==(const tVariant &param) const;
	bool operator <(const tVariant &param) const;
	bool operator >(const tVariant &param) const;
	
	//! Not equal operator
	inline tVariant operator !=(const tVariant &param) const
	{ return ! (*this == param); }
	
	//! Minor or equal operator
	inline tVariant operator <=(const tVariant &param) const
	{ return ! ( *this > param ); }
	
	//! Major or equal operator
	inline tVariant operator >=(const tVariant &param) const
	{ return ! ( *this < param ); }
	
	
//@}
protected:
	VariantTypes assignedType;	//!< Type assigned to the variant
	IntegerSizes integerSize;	//!< Size of integer variant
	std::string stringvalue;	//!< Used when the variant is a string
	uint32_t uintvalue;		//!< Used when the variant is an unsigned int (any size) or a boolean
	int32_t sintvalue;		//!< Used when the variant is a signed int (any size)
	void *ptrvalue;			//!< Used when the variant is a pointer (any type)
};

#endif
