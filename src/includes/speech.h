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
\brief Used in speech methods to hold spoken text. "unicode" conversion supported as well as network to host byte order conversion and vice versa
\author Chronodt
\note it is called simply cSpeech because i had no heart to call it cUnicode, since it is not a true unicode support, but a "cut-down" one, just enough to work with UO protocol

speech mode:
0x00 - Regular
0x01 - Broadcast
0x02 - Emote (adds *'s as part of text)
0x06  Label (You see: )
0x07 - Emphasis (clears previous messages)
0x08 - Whisper
0x09  Yell
0x0a  spell
*/
class cSpeech
{
protected:
	unistring unicodeText;	//!< Unicode string to say
	uint8_t mode;		//!< type of speech (normal, emote, yell, etc etc)
	uint16_t color;		//!< Color of the speech
	uint16_t font;		//!< Font to use for the speech
	char language[4];	//!< Language code (null terminated)
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

	inline const int size()
	{ return unicodeText.size(); }

	inline const uint8_t getMode()
	{ return mode; }

	inline void setMode(const uint8_t newmode)
	{ mode = newmode; }

	inline const uint16_t getColor()
	{ return color; }

	inline void setColor(const uint16_t newcolor)
	{ color = newcolor; }

	inline const uint16_t getFont()
	{ return font; }

	inline void setFont(const uint16_t newfont)
	{ font = newfont; }

	inline const char* getLanguage()
	{ return language; }

	inline void setLanguage(char* newlanguage)
	{
		strncpy(language, newlanguage, 3);
		language[3] = 0;
	}

	inline const pSerializable getSpeaker()
	{ return speaker; }

	inline void setSpeaker(const pSerializable newSpeaker)
	{ speaker = newSpeaker; }

};

void responsevendor(pClient client, pChar pc_vendor);
void char2wchar (const char* str);

#endif
