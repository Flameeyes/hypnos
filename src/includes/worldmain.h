/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declaration of CWorldMain class
*/

#ifndef __WORLDMAIN_H_
#define __WORLDMAIN_H_


void split( std::string& souce, std::string& first, std::string& second );

class cStringFile {

public:

	FILE* f;

public:

	cStringFile( std::string& path, const char* mode );
	~cStringFile();

	void read( std::string& line );
	void read( std::string& l, std::string& r );
	void read( std::string& first, std::string& second, std::string& third );

	bool eof();
};



class CWorldMain  
{
private:

	FILE* iWsc;
	FILE* cWsc;
	FILE* jWsc;
	FILE* gWsc;

	bool isSaving;

	uint32_t itm_curr, chr_curr;

	void SaveChar( pChar pc );
	void loadChar();

	void SaveItem( pItem pi );
	void loadItem();

	void realworldsave();
public:

	CWorldMain();
	~CWorldMain();

	void loadNewWorld();
	void saveNewWorld();

	bool Saving();
};

void fprintWstring( FILE* f, char* name, wstring c );
wstring HexVector2UnicodeString( char* s );

extern class CWorldMain* cwmWorldState;


#endif // WORLDMAIN
