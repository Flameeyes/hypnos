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
private:
	std::wstring unicode_text;	//warning: wchar is not guaranteed to be a 16 bit char on all systems
        uint8_t mode;	//0=say,2=emote,8=whisper,9=yell
	uint16_t color;
	uint16_t font;
        char[4] language;
        pChar speaker;
        bool packet_byteorder;
public:
	inline bool isPacketByteOrder()
	        { return packet_byteOrder}
        void setPacketByteOrder();				//!< Sets byteorder and swaps unicode_text to the protocol byteorder (if needed)
        void clearPackeByteOrder();  				//!< Clears byteorder and swaps unicode_text if it is necessary to machine byte order
        inline void assignPacketByteOrder(bool byteorder)	//!< sets byteorder without changing unicode_text
        	{ packet_byteorder = byteorder; }
        char operator[](int i);                           	//!< gets 8-bit ascii char in location "i" in either byteorder
        cSpeech& operator= (std::string s);     //!< Assignment operator from a non unicode string. Converts to unicode and stores it
        cSpeech& operator= (cSpeech& s);
        cSpeech(char* buffer, int size = 0); 	//!< Size is used only for not null-terminated strings, if it is 0 is ignored, else reads size bytes wherever \\0 is present or not
        std::string toString();	//!< returns a normal char-based string obtained truncating unicode to ascii values
        std::string toGhost();	//!< returns a randomized "ooooOOoo" based on current string content (not unicode)
}


void responsevendor(NXWSOCKET  s, int vendor);


void char2wchar (const char* str);


#endif
