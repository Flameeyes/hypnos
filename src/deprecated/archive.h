  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/*!
\file
\brief Archive stuff
*/

#ifndef __ARCHIVE_H__
#define __ARCHIVE_H__


typedef std::map< uint32_t, pObject > OBJECT_MAP;

class cAllObjects {

protected:

	friend class cAllObjectsIter;
	friend class cAllCharsIter;
	friend class cAllItemsIter;


	static OBJECT_MAP all; //!< all objects

	static uint32_t current_char_serial; //!< current char serial
	static uint32_t current_item_serial; //!< current item serial

public:

	cAllObjects();
	~cAllObjects();

	uint32_t getNextCharSerial();
	uint32_t getNextItemSerial();
	void updateCharSerial( uint32_t ser );
	void updateItemSerial( uint32_t ser );
	void clear();

};


extern cAllObjects objects;

class cAllObjectsIter {

public:

	OBJECT_MAP::iterator curr; //current, can be deleted safaly
	OBJECT_MAP::iterator next; //next for save delete of current

public:

	cAllObjectsIter( );
	~cAllObjectsIter();

	void rewind();
	bool IsEmpty();
	pObject getObject();
	uint32_t getSerial();

	cAllObjectsIter& operator++(int);

};


class cAllCharsIter : public cAllObjectsIter{

private:

public:

	cAllCharsIter(  );
	~cAllCharsIter();

	void rewind();
	bool IsEmpty();
	pChar getChar();

	cAllCharsIter& operator++(int);

};

class cAllItemsIter: public cAllObjectsIter {

private:

public:

	cAllItemsIter(  );
	~cAllItemsIter();

	void rewind();
	bool IsEmpty();
	pChar getItem();


	cAllItemsIter& operator++(int);

};

#endif
