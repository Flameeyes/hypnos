/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Inline functions of misc utility
*/

#ifndef __INLINES_H__
#define __INLINES_H__

#include "items.h"
#include "chars.h"
#include "basics.h"


#define TIMEOUT(X) (((X) <= uiCurrentTime) || overflow)
// Macros & Templates by Xan :

template<typename T> inline T qmax(T a, T b) { a > b ? a : b; }
template<typename T> inline T qmin(T a, T b) { a < b ? a : b; }

// this delete is safe from multiple deletes of same ptr
template<typename T> inline void safedelete(T*& p) { delete p; p = NULL; }
template<typename T> inline void safedeletearray(T*& p) { delete[] p; p = NULL; }
template<typename T> inline void qswap(T& a, T& b) { T dummy; dummy = a; a = b; b = dummy; }

#define charsysmsg(PC) if (PC->getClient()!=NULL) PC->getClient()->sysmsg

#define DBYTE2WORD(A,B) (((A)<<8) + ((B)&0xFF))
#define WORD2DBYTE1(A)  ((char)((A)>>8))
#define WORD2DBYTE2(A)  ((char)((A)&0xFF))
#define WORD2DBYTE(A,B,C) { B = WORD2DBYTE1(A); C = WORD2DBYTE2(A); }


inline bool chance(int percent) { return ( (rand()%100) < percent); }

inline int calcCharFromPtr(unsigned char *p)
{
	int serial;
	if((serial=LongFromCharPtr(p)) < 0) return INVALID;
	if (ISVALIDPC(pointers::findCharBySerial(serial))) return (DEREF_P_CHAR(pointers::findCharBySerial(serial)));
	else return (INVALID);
}

inline int calcItemFromPtr(unsigned char *p)
{
	int serial;
	if((serial=LongFromCharPtr(p)) < 0) return INVALID;
	if (ISVALIDPI(pointers::findItemBySerial(serial))) return (DEREF_P_ITEM(pointers::findItemBySerial(serial)));
	else return (INVALID);
}

inline int calcItemFromSer(int ser) // Aded by Magius(CHE) (2)
{
	if (ISVALIDPI(pointers::findItemBySerial(ser))) return (DEREF_P_ITEM(pointers::findItemBySerial(ser)));
	else return (INVALID);
}

inline int calcCharFromSer(int serial)
{
	if (ISVALIDPC(pointers::findCharBySerial(serial))) return (DEREF_P_CHAR(pointers::findCharBySerial(serial)));
	else return (INVALID);
}

inline void SetTimerSec( TIMERVAL *timer, const short seconds)
{
	*timer=seconds * MY_CLOCKS_PER_SEC + uiCurrentTime;
}

inline bool isCharSerial( long ser ) { return ( ser > 0 && ser <  0x40000000 ); }
//ndEndy 0 is not a char serial, see curr_charSerial note
inline bool isItemSerial( long ser ) { return ( /*ser >= 0 && */ser >= 0x40000000 ); }

#define SETSOCK(A) g_nCurrentSocket = A;

inline std::string toString(int value)
{
	char s[21];
	snprintf(s, 20, "%d", value);
	s[19]=0x00;

	return std::string(s);
}

inline std::string toString(double value)
{
	char s[21];
	snprintf(s, 20, "%f", value);
	s[19]=0x00;

	return std::string(s);
}

/*
\brief Convert a string to a wstring
\author Endymion
\param from the source string
\param to the dest wstring
*/
inline void string2wstring( string from, wstring& to )
{
	to.erase();
	string::iterator iter( from.begin() ), end( from.end() );
	for( ; iter!=end; iter++ ) {
		to+=static_cast<wchar_t>(*iter);
	}
}

/*
\brief Convert a wstring to a string
\author Endymion
\param from the source wstring
\param to the dest string
*/
inline void wstring2string( wstring from, string& to )
{
	to.erase();
	wstring::iterator iter( from.begin() ), end( from.end() );
	for( ; iter!=end; iter++ ) {
		to+=static_cast<char>(*iter);
	}
}

/*!
\brief play sound
\param goldtotal Total of gold "moved"
\return soundsfx to play
*/
inline UI16 goldsfx(UI16 goldtotal)
{
	UI16 sound;

	if (goldtotal==1)
		sound = 0x0035;
	else if (goldtotal<6)
		sound = 0x0036;
	else
		sound = 0x0037;

	return sound;
}

/*!
\brief count the number of bit set
\return int number of bit set
\param number the number
\remark is this really usefull ?
*/
inline const int numbitsset( int number )
{
	int bitsset = 0;

	while( number )
	{
		bitsset += number&0x1;
		number >>= 1;
	}
	return bitsset;
}

/*!
\brief Converts an ip-string to a long value
\param ip ip-string
\param ris integer to store the ip to
\return true if the string is a valid ip, else false

\note this is not a "clean" function, because uses goto
but it works great!

\author Flameeyes
*/
bool ip2long(char *ip, int &ris)
{
  char buffer[16], *a, *b, *c;
  bool ret = true;

  a = strchr(buffer, '.');
  if ( ! a ) { ret = false; goto end; }
  *(a++) = 0;

  b = strchr(a, '.');
  if ( ! b ) { ret = false; goto end; }
  *(b++) = 0;

  c = strchr(b, '.');
  if ( ! c ) { ret = false; goto end; }
  *(c++) = 0;

  ris = ( atoi(a) << 24 ) + ( atoi(b) << 16 ) + ( atoi(c) << 8 ) + atoi(c);

  end:
    if ( a ) *(--a) = '.';
    if ( b ) *(--b) = '.';
    if ( c ) *(--c) = '.';

    return ret;
}

/*!
\brief Converts a long value into an ip-string
\param l long value
\param out output string - must be prealloccated
\return pointer to the out string

\author Flameeyes
*/
char *long2ip(int l, char *out)
{
  char a = l >> 24, b = ( l >> 16 ) & 0xFF,
       c = ( l >> 8 ) & 0xFF, d = l & 0xFF;
  sprintf(out, "%u.%u.%u.%u", a, b, c, d);
  return out;
}

#endif //__INLINES_H__
