  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#ifndef __SPEECH_H__
#define __SPEECH_H__

//! Used in speech methods
//!\todo complete cSpeech and do the implementations
//!\note it is called simply cSpeech because i had no heart to call it cUnicode, since it is not a true unicode support, but a "cut-down" one, just enough to work with UO protocol
class cSpeech
{
protected:
	unistring unicodeText;	//!< Unicode string to say
        uint8_t mode;		//!< 0=say,2=emote,8=whisper,9=yell
	uint16_t color;		//!< Color of the speech
	uint16_t font;		//!< Font to use for the speech
	char language[4];	//!< Language code (null terminated)
	pSerializable speaker;	//!< Character who's speeching. NOTE: everything with a serial can speak!
	bool packetByteOrder;	//!< Is the string in network-endian?
public:
//@{
/*!
\brief Constructors and operators
*/
	char operator[](int i);			//!< gets 8-bit ascii char in location "i" in either byteorder
	cSpeech& operator= (std::string s);	//!< Assignment operator from a non unicode string. Converts to unicode and stores it
	cSpeech& operator= (cSpeech& s);	//!< assignment operator (copy operator)
        cSpeech(char* buffer, int size = 0); 	//!< Size is used only for not null-terminated strings, if it is 0 is ignored, else reads size bytes wherever \\0 is present or not
//@}
	
	inline bool isPacketByteOrder()
	{ return packetByteOrder; }
        
	void setPacketByteOrder();				//!< Sets byteorder and swaps unicode_text to the protocol byteorder (if needed)
        void clearPackeByteOrder();  				//!< Clears byteorder and swaps unicode_text if it is necessary to machine byte order
	
	//! sets byteorder without changing unicode_text
        inline void assignPacketByteOrder(bool byteorder)
	{ packetByteOrder = byteorder; }
        
	std::string toString();			//!< returns a normal char-based string obtained truncating unicode to ascii values
        std::string toGhost();			//!< returns a randomized "ooooOOoo" based on current string content (not unicode)
        
	//! Returns a read-only char* pointer to internal unicode_text, as a null-terminated 16bit-char string
	inline const char* rawBytes() const
	{ return reinterpret_cast<const char*>(unicodeText.c_str()); }
        
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

};

void responsevendor(pClient client, int vendor);
void char2wchar (const char* str);

#endif
