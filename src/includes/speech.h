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
class cSpeech
{
	std::wstring unicode_text;	//warning: wchar is not guaranteed to be a 16 bit char in all systems
        uint8_t mode;	//0=say,2=emote,8=whisper,9=yell
	uint16_t color;
	uint16_t font;
        char[4] language;
        pChar speaker;
        inline char operator[](int i)
        { return unicode_text[i] & 0xff; }
        cSpeech(char* buffer); //Only from packet buffer, because it updates endian from packet to machine
        std::string toString();	//!< returns a normal char-based string obtained truncating unicode to ascii values
        std::string toGhost();	//!< returns a randomized "ooooOOoo" based on current string content (not unicode)
}


void responsevendor(NXWSOCKET  s, int vendor);


void char2wchar (const char* str);


#endif
