/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __TVARIANT_H__
#define __TVARIANT_H__

#include "common_libs.h"

/*!
\class tVariant tvariant.h "abstraction/tvariant.h"
\brief Variant type class
\author Flameeyes

The tVariant type is, as the name states, a variant type. An instance of this
class can contain different types of pointers, as well as integers (signed or
unsigned) of any size, booleans, strings and vectors.

The way this use to accomplish the above features is to save the stored type
identifier AND the cctual content of the value passed, using the
tVariant::ptr union to share the memory between the different pointers, and
pointers for other types (bools, integers and compound types).

tVariant instances can take their content and transform it into other types
in a very simple way. And then, can return a translated type on-the-fly without
change the actual data inside them.

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
		vtPClient,	//!< pClient value
		vtPVoid,	//!< void pointer value
		vtVector	//!< tVariantVector value
	};
	
	//! Integer types sizes
	enum IntegerSizes {
		isNotInt,	//!< The variable is not an integer
		is8,		//!< 8-bit integer
		is16,		//!< 16-bit integer
		is32,		//!< 32-bit integer
		is64		//!< 64-bit integer
	};

//@{
/*!
\name Constructors and operators
*/
	tVariant();
	~tVariant();
	
	/*!
	\brief Constructor with value
	\param astr String to assign to the variant
	*/
	inline tVariant(const std::string &astr)
	{ tVariant(); *this = astr; }
	
	/*!
	\brief Constructor with value
	\param aval Boolean to assign to the variant
	*/
	inline tVariant(const bool &aval)
	{ tVariant(); *this = aval; }
	
	/*!
	\brief Constructor with value
	\param aval unsigned to assign to the variant
	*/
	inline tVariant(const uint32_t &aval)
	{ tVariant(); *this = aval; }
	
	/*!
	\brief Constructor with value
	\param aval integer to assign to the variant
	*/
	inline tVariant(const int32_t &aval)
	{ tVariant(); *this = aval; }
	
	/*!
	\brief Constructor with value
	\param aptr void * to assign to the variant
	*/
	inline tVariant(void *aptr)
	{ tVariant(); *this = aptr; }
	
	/*!
	\brief Constructor with value
	\param apc pChar to assign to the variant
	*/
	inline tVariant(pChar apc)
	{ tVariant(); *this = apc; }
	
	/*!
	\brief Constructor with value
	\param api pItem to assign to the variant
	*/
	inline tVariant(pItem api)
	{ tVariant(); *this = api; }
	
	/*!
	\brief Constructor with value
	\param aclient pClient to assign to the variant
	*/
	inline tVariant(pClient aclient)
	{ tVariant(); *this = aclient; }
	
	tVariant &operator =(const std::string &astr);
	tVariant &operator =(const bool &aval);
	tVariant &operator =(const uint32_t &aval);
	tVariant &operator =(const int32_t &aval);
	tVariant &operator =(void *aptr);
	tVariant &operator =(pChar apc);
	tVariant &operator =(pItem api);
	tVariant &operator =(pClient api);
	
	tVariant operator -() const;
	
	tVariant operator +(const tVariant &param) const;
	tVariant operator -(const tVariant &param) const;
	
	//! Addiction operator
	tVariant operator +=(const tVariant &param)
	{ return (*this = *this + param); }
	
	//! Subtraction operator
	tVariant operator -=(const tVariant &param)
	{ return (*this = *this - param); }
	
	//! Prefixed increment operator
	inline tVariant operator ++()
	{ return (*this += 1); }
	
	//! Prefixed decrement operator
	inline tVariant operator --()
	{ return (*this += 1); }
	
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

//@{
/*!
\name Conversions
\brief Functions used to return a fixed-type variable

All these functions has a bool* parameter defaulted NULL. If this is not NULL, the
pointed bool will be set to true if the conversion is done correctly, else to false
*/
	std::string toString(bool *result = NULL) const;
	bool toBoolean(bool *result = NULL) const;
	pChar toPChar(bool *result = NULL) const;
	pItem toPItem(bool *result = NULL) const;
	pClient toPClient(bool *result = NULL) const;
	void *toPVoid(bool *result = NULL) const;
	uint32_t toUInt32(bool *result = NULL) const;
	uint16_t toUInt16(bool *result = NULL) const;
	uint8_t toUInt8(bool *result = NULL) const;
	int32_t toSInt32(bool *result = NULL) const;
	int16_t toSInt16(bool *result = NULL) const;
	int8_t toSInt8(bool *result = NULL) const;
//@}

//@{
/*!
\name Type Juggling
\brief These are functions to change the type of a variable

All these functions return a bool which represent the success or not of the conversion
*/

	bool convertInString();
	bool convertInBoolean();
	bool convertInUInt();
	bool convertInSInt();
	bool convertInPChar();
	bool convertInPItem();
	bool convertInPClient();
	bool convertInPVoid();

//@}

	inline const bool isNull() const
	{ return assignedType == vtNull; }

private:
	void recalcUIntSize();
	void recalcSIntSize();

protected:
	VariantTypes assignedType;	//!< Type assigned to the variant
	IntegerSizes integerSize;	//!< Size of integer variant
	
	void *pointer;			//!< Pointer to the store value
	
	union {
		uint32_t uintvalue,		//!< Value of unsigned integers (any size) or boolean
		int32_t sintvalue,		//!< Value of signed integers (any size)
		std::string *strpointer,	//!< Pointer to the string object
		tVariantVector *vectorptr,	//!< Pointer to the vector object
		void *ptrvalue,			//!< Value of the void pointers
	} value;
	
};

#endif
