/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator (Utility Library)                              |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in LICENSE file.               |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __LIBHYPNOS_CVARIANT_H__
#define __LIBHYPNOS_CVARIANT_H__

#include "commons.h"
#include "exceptions.h"

// Only if not included by the hypnos complete sources defines the pointers
#ifndef __TYPEDEFS_H__
typedef class cChar *pChar;
typedef class cItem *pItem;
typedef class cClient *pClient;
#endif

namespace nLibhypnos {

/*!
\class cVariant cvariant.h "libhypnos/cvariant.h"
\brief Variant type class
\author Flameeyes

The cVariant type is, as the name states, a variant type. An instance of this
class can contain different types of pointers, as well as integers (signed or
unsigned) of any size, booleans, strings and vectors.

The way this use to accomplish the above features is to save the stored type
identifier AND the cctual content of the value passed, using the
cVariant::ptr union to share the memory between the different pointers, and
pointers for other types (bools, integers and compound types).

cVariant instances can take their content and transform it into other types
in a very simple way. And then, can return a translated type on-the-fly without
change the actual data inside them.

*/
class cVariant
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
		vtVector	//!< cVariantVector value
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
	cVariant();
	~cVariant();
	
	/*!
	\brief Constructor with value
	\param aval Value to assign to the variant
	*/
	inline cVariant(const bool &aval)
	{ cVariant(); *this = aval; }

	//! \copydoc cVariant::cVariant(const bool&)
	inline cVariant(const std::string &aval)
	{ cVariant(); *this = aval; }
	
	//! \copydoc cVariant::cVariant(const bool&)
	inline cVariant(const uint64_t aval)
	{ cVariant(); *this = aval; }
	
	//! \copydoc cVariant::cVariant(const bool&)
	inline cVariant(const int64_t aval)
	{ cVariant(); *this = aval; }
	
	//! \copydoc cVariant::cVariant(const bool&)
	inline cVariant(void *aval)
	{ cVariant(); *this = aval; }
	
	//! \copydoc cVariant::cVariant(const bool&)
	inline cVariant(pChar aval)
	{ cVariant(); *this = aval; }
	
	//! \copydoc cVariant::cVariant(const bool&)
	inline cVariant(pItem aval)
	{ cVariant(); *this = aval; }
	
	//! \copydoc cVariant::cVariant(const bool&)
	inline cVariant(pClient aval)
	{ cVariant(); *this = aval; }
	
	cVariant &operator =(const std::string &astr);
	cVariant &operator =(const bool &aval);
	cVariant &operator =(const uint64_t &aval);
	cVariant &operator =(const int64_t &aval);
	cVariant &operator =(void *aptr);
	cVariant &operator =(pChar apc);
	cVariant &operator =(pItem api);
	cVariant &operator =(pClient api);
	
	//@{
	/*!
	\brief Integer assignments signatures
	
	In this section you'll found assigments operators which are used to
	assign arbitrary length integers, without threw ambiguity between
	signatures.
	*/
	
	#define op_assign_uint(dim) \
		inline cVariant &operator =(const uint##dim##_t &aval) \
		{ *this = (uint64_t)aval; return *this; }
	
	#define op_assign_int(dim) \
		inline cVariant &operator =(const int##dim##_t &aval) \
		{ *this = (int64_t)aval; return *this; }
		
	op_assign_uint(32)
	op_assign_uint(16)
	op_assign_uint(8)
	
	op_assign_int(32)
	op_assign_int(16)
	op_assign_int(8)
	
	//@}
	
	cVariant operator -() const;
	
	cVariant operator +(const cVariant &param) const;
	cVariant operator -(const cVariant &param) const;
	
	//! Addiction operator
	cVariant operator +=(const cVariant &param)
	{ return (*this = *this + param); }
	
	//! Subtraction operator
	cVariant operator -=(const cVariant &param)
	{ return (*this = *this - param); }
	
	//! Prefixed increment operator
	inline cVariant operator ++()
	{ return operator += ((uint64_t)1); }
	
	//! Prefixed decrement operator
	inline cVariant operator --()
	{ return operator -= ((uint64_t)1); }
	
	cVariant operator ++(int unused);
	cVariant operator --(int unused);
	bool operator ==(const cVariant &param) const;
	bool operator <(const cVariant &param) const;
	bool operator >(const cVariant &param) const;
	
	//! Not equal operator
	inline cVariant operator !=(const cVariant &param) const
	{ return ! (*this == param); }
	
	//! Minor or equal operator
	inline cVariant operator <=(const cVariant &param) const
	{ return ! ( *this > param ); }
	
	//! Major or equal operator
	inline cVariant operator >=(const cVariant &param) const
	{ return ! ( *this < param ); }
//@}

//@{
/*!
\name Conversions
\brief Functions used to return a fixed-type variable

All these functions has a bool* parameter defaulted NULL. If this is not NULL,
the pointed bool will be set to true if the conversion is done correctly, else
to false.

In this section you can also find the operators used for the automatic cast of
a cVariant to one of his usable types.
*/
	std::string toString(bool *result = NULL) const;
	bool toBoolean(bool *result = NULL) const;
	pChar toPChar(bool *result = NULL) const;
	pItem toPItem(bool *result = NULL) const;
	pClient toPClient(bool *result = NULL) const;
	void *toPVoid(bool *result = NULL) const;
	uint64_t toUInt64(bool *result = NULL) const;
	uint32_t toUInt32(bool *result = NULL) const;
	uint16_t toUInt16(bool *result = NULL) const;
	uint8_t toUInt8(bool *result = NULL) const;
	int64_t toSInt64(bool *result = NULL) const;
	int32_t toSInt32(bool *result = NULL) const;
	int16_t toSInt16(bool *result = NULL) const;
	int8_t toSInt8(bool *result = NULL) const;

	operator std::string() const
	{ return toString(); }
	
	operator bool() const
	{ return toBoolean(); }
	
	operator pChar() const
	{ return toPChar(); }
	
	operator pItem() const
	{ return toPItem(); }
	
	operator pClient() const
	{ return toPClient(); }
	
	operator void*() const
	{ return toPVoid(); }
	
	operator uint64_t() const
	{ return toUInt64(); }
	
	operator uint32_t() const
	{ return toUInt32(); }
	
	operator uint16_t() const
	{ return toUInt16(); }
	
	operator uint8_t() const
	{ return toUInt8(); }
	
	operator int64_t() const
	{ return toSInt64(); }
	
	operator int32_t() const
	{ return toSInt32(); }
	
	operator int16_t() const
	{ return toSInt16(); }
	
	operator int8_t() const
	{ return toSInt8(); }
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
	
	void clear();

private:
	void recalcUIntSize(const uint64_t &val);
	void recalcSIntSize(const int64_t &val);

protected:
	VariantTypes assignedType;	//!< Type assigned to the variant
	IntegerSizes integerSize;	//!< Size of integer variant
	
	void *pointer;			//!< Pointer to the store value
};

/*!
\class eInvalidInteger cvariant.h "libhypnos/cvariant.h"
\brief Exception thrown when a variant is set to an integer type but hasn't the
	integer size set to a valid one.
*/
class eInvalidInteger : public eException
{
public:
	const cVariant *var;	//!< Invalid variant variable
	
	eInvalidInteger(const cVariant *aVar)
		: var(aVar)
	{ }
};

}

#endif
