/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#ifndef __SPEECH_H__
#define __SPEECH_H__

/*!
\brief Unicode text container and converter.
\author Chronodt
\note it is called simply cSpeech because i had no heart to call it cUnicode,
	since it is not a true unicode support, but a "cut-down" one, just
	enough to work with UO protocol

Used in speech methods to hold spoken text. "unicode" conversion supported as
well as network to host byte order conversion and vice versa.

\paragraph sm Speech mode
\li \c 0x00 - Regular
\li \c 0x01 - Broadcast
\li \c 0x02 - Emote (adds *'s as part of text)
\li \c 0x06 - Label (You see: )
\li \c 0x07 - Emphasis (clears previous messages)
\li \c 0x08 - Whisper
\li \c 0x09 - Yell
\li \c 0x0a - spell
*/
class cSpeech
{
protected:
	unistring unicodeText;	//!< Unicode string to say
	uint8_t mode;		//!< type of speech (normal, emote, yell, etc etc)
				//!< \see \ref sm
	uint16_t color;		//!< Color of the speech
	uint16_t font;		//!< Font to use for the speech
	uint32_t language;	//!< Language code (is a 3byte text + null terminator, so it fits in a single uint32_t)
	pSerializable speaker;	//!< Character who's speaking. NOTE: everything with a serial can speak!
	bool packetByteOrder;	//!< Is the string in network-endian?
public:
//@{
/*!
\brief Constructors and operators
*/
	char operator[](int i); 			//!< gets 8-bit ascii char in location "i" in either byteorder
	cSpeech& operator= (std::string& s);		//!< Assignment operator from a non unicode string. Converts to 16bit-char and stores it
	cSpeech& operator= (cSpeech& s);		//!< assignment operator (copy operator)
	cSpeech(std::string& s); 			//!< non-unicode fill from string (defaults to packetByteorder = false)
	cSpeech(char* buffer, uint16_t size = 0);	//!< Unicode fill from packet buffer. Size is used only for not null-terminated strings, if it is 0 is ignored, else reads size bytes wherever \\0 is present or not. (defaults to packetByteorder = true)
//@}

	inline bool isPacketByteOrder()
	{ return packetByteOrder; }

	void setPacketByteOrder();				//!< Sets byteorder and swaps unicode_text to the protocol byteorder (if needed)
	void clearPackeByteOrder();				//!< Clears byteorder and swaps unicode_text if it is necessary to machine byte order

	//! sets byteorder without changing unicode_text
	inline void assignPacketByteOrder(bool byteorder)
	{ packetByteOrder = byteorder; }

	std::string toString();			//!< returns a normal char-based string obtained truncating unicode to ascii values
	std::string toGhost();			//!< returns a randomized "ooooOOoo" based on current string content (not unicode)

	//! Returns a read-only char* pointer to internal unicode_text, as a rawmode 8 bit-char buffer with a double 0 terminator
	inline const char* rawBytes() const
	{ return reinterpret_cast<const char*>(unicodeText.c_str()); }

	//! Returns a read-only char* pointer to internal unicode_text, as a null-terminated 16bit-char pointer. WARNING: remember to check for byteorder first
	inline const uint16_t* c_str() const
	{ return unicodeText.c_str(); }

	inline int size()
	{ return unicodeText.size(); }

	inline uint8_t getMode()
	{ return mode; }

	inline void setMode(const uint8_t newmode)
	{ mode = newmode; }

	inline uint16_t getColor()
	{ return color; }

	inline void setColor(const uint16_t newcolor)
	{ color = newcolor; }

	inline uint16_t getFont()
	{ return font; }

	inline void setFont(const uint16_t newfont)
	{ font = newfont; }

	inline uint32_t getLanguage()
	{ return language; }

	inline void setLanguage(uint32_t newlanguage)
	{
		language = newlanguage & 0xffffff00;	//ensure 0-termination (remeber, we are using this only when sending it back in network-endian, so the ending 0 is the lsb)
	}

	inline pSerializable getSpeaker()
	{ return speaker; }

	inline void setSpeaker(const pSerializable newSpeaker)
	{ speaker = newSpeaker; }

};

void responsevendor(pClient client, pChar pc_vendor);
void char2wchar (const char* str);

#endif
