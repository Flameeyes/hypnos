/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief SQLite backend definition
*/

#include "common_libs.h"
#include <sqlite.h>
#include <zthread/Mutex.h>

/*!
\brief SQLite access class
 
 This class provides methods to access the underlying SQLite library and his
 database with savegames.
*/
class cSQLite
{
public:
	/*!
	\brief SQLite Query class
	 
	 This class proviced methods to access the results of a SQLite query.
	 After the cSQLite class has returned one instance of this class, the
	 thread can be unlocked.
	*/
	class cSQLiteQuery
	{
	protected:
		static const uint8_t flagArchiving = 0x01;
			//!< Should we archive the read records?
		static const uint8_t flagStarted   = 0x02;
			//!< We already started fetching rows?
		
		void cleanArchive();
	public:
		typedef std::map<std::string,std::string> tRow;
		cSQLiteQuery(sqlite_vm *nvm);
		~cSQLiteQuery();
		bool fetchRow();
		
		//! Gets the column names
		inline stringVector getColumns() const
		{ return columnNames; }
		
		//! Gets the last row fetched
		inline tRow getLastRow() const
		{ return thisRow; }
		
		/*!
		\brief Gets the row at index
		\param i Index of the row to get
		\return Array of strings with values
		*/
		inline tRow getRow(int i) const
		{ return totalResults[i]; }
		
		//! Fetch all the rows in the result set
		inline void fetchAll()
		{ while(fetchRow()); }
		
		/*!
		\brief Gets the number of rows fetched
		\return The number of rows fetched at the moment
		\note Valid only if archiving is on.
		*/
		inline const int getNumRows()
		{ return totalResults.size(); }
		
		/*!
		\brief Sets the archiving on or off
		\param on If true, set the archiving on, else set it off
		\note If we already started fetching rows, we cannot
		 turn archiving on, but we still can turn it off.
		 In this case, the cleanArchive() function will be
		 called
		*/
		inline void setArchiving(bool on)
		{
			if ( on && ! flags & flagStarted )
				flags |= flagArchiving;
			else
			{
				flags &= ~flagArchiving;
				if ( flags & flagStarted )
					cleanArchive();
			}
		}
	protected:
		sqlite_vm *vm;			//!< SQLite compiled VM
		uint8_t flags;			//!< Flags for the query
		
		tRow thisRow;			//!< Current row
		stringVector columnNames;       //!< Name of the columns
		std::vector<tRow> totalResults;
			//!< Vector which has all the rows fetched
			//!   only if archiving is on
	};
	//! Pointer to a SQLite Query
	typedef cSQLiteQuery* pSQLiteQuery;

	static const uint32_t flagAborted = 0x00000001;
		//!< There was a fatal error?
	
	cSQLite(std::string name);
	~cSQLite();
	
	//! Returns true if a fatal error aborted the database connection
	inline const bool isAborted() const
	{ return flags & flagAborted; }
	
	pSQLiteQuery execQuery(std::string query);
protected:
	inline void setFlag(uint32_t flag, bool on = true)
	{
		if ( on )
			flags |= flag;
		else
			flags &= ~flag;
	}
	
	cSQLite *litedb;	//!< Pointer to access the SQLite database
	uint32_t flags;		//!< Flags for the access
	ZThread::Mutex mutex;	//!< Mutex for threaded access to the database
};

extern cSQLite *globalDB;
