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
\brief AMX Var Server
\todo document it
*/

#ifndef __AMXVARSERVER_H__
#define __AMXVARSERVER_H__

#include "common_libs.h"

typedef enum
{
	AMXVARSRV_OK = 0,
	AMXVARSRV_UNKNOWN_VAR,
	AMXVARSRV_DUPLICATE_VAR,
	AMXVARSRV_WRONG_TYPE,
	AMXVARSRV_ACCESS_DENIED
} AMXVARSRV_ERROR;

typedef enum
{
	AMXVARSRV_UNDEFINED		= 0,
	AMXVARSRV_INTEGER		= 1,
	AMXVARSRV_bool		= 2,
	AMXVARSRV_STRING		= 3,
	AMXVARSRV_INTEGERVECTOR	= 4,
	AMXVARSRV_SCRIPTID		= 5
} AMXVARSRV_DATATYPE;

class amxVariable
{
	public:
		virtual				~amxVariable() {}
		virtual	AMXVARSRV_DATATYPE	getType();
		virtual int32_t			getSize( const int32_t index = -1 );
};

class amxIntegerVariable : public amxVariable
{
	private:
		int32_t				value;
	public:
						amxIntegerVariable( const int32_t initialValue = 0 );
						~amxIntegerVariable();
		AMXVARSRV_DATATYPE		getType();
		int32_t				getValue();
		void				setValue( const int32_t newValue );
		int32_t				getSize();
};

class amxIntegerVector : public amxVariable
{
	private:
		vector< int32_t >			value;
	public:
						amxIntegerVector( const int32_t size, const int32_t initialValue = 0 );
						~amxIntegerVector();
		AMXVARSRV_DATATYPE		getType();
		int32_t				getValue( const uint32_t index );
		void				setValue( const uint32_t index, const int32_t newValue );
		int32_t				getSize( const int32_t index = -1 );
};

class amxStringVariable : public amxVariable
{
	private:
		std::string			value;
	public:
						amxStringVariable( const std::string& initialValue = "");
						~amxStringVariable();
		AMXVARSRV_DATATYPE		getType();
		std::string			getValue();
		void				setValue( const std::string& newValue = "");
		int32_t				getSize();
};

class amxScriptIdVariable : public amxVariable
{
	private:
		uint32_t	value; //!< script id serial ( is a number into core, string only on save and load )
	public:
						
		amxScriptIdVariable( char* initialValue );
		amxScriptIdVariable( uint32_t initialValue = INVALID );
		~amxScriptIdVariable();
	
		AMXVARSRV_DATATYPE getType();
		uint32_t getValue();
		void setValue( char* newValue );
		void setValue( uint32_t initialValue = INVALID );
		int32_t getSize();
};

typedef pair< int32_t, amxVariable* >		amxVariablePair;
typedef map< int32_t, amxVariable* > 		amxVariableMap;
typedef amxVariableMap::iterator		amxVariableMapIterator;

typedef pair< int32_t, amxVariableMap >		amxObjectVariablePair;
typedef map< int32_t, amxVariableMap >		amxObjectVariableMap;
typedef amxObjectVariableMap::iterator		amxObjectVariableMapIterator;

class amxVariableServer
{
	private:
		amxObjectVariableMap		varMap;
		int32_t				error;
		bool				mode;
	public:
						amxVariableServer();
						~amxVariableServer();
		bool				inUserMode();
		bool				inServerMode();
		void				setUserMode();
		void				setServerMode();
		int32_t				getError();
		int32_t				firstVariable( const uint32_t serial );
		int32_t				nextVariable( const uint32_t serial, const int32_t previous );
		AMXVARSRV_DATATYPE		typeOfVariable( const uint32_t serial, const int32_t variable );
		//
		//	Integer variable
		//
		bool				insertVariable( const uint32_t serial, const int32_t variable, const int32_t value );
		bool				updateVariable( const uint32_t serial, const int32_t variable, const int32_t value );
		bool				selectVariable( const uint32_t serial, const int32_t variable, int32_t& value );
		//
		//	String variable
		//
		bool				insertVariable( const uint32_t serial, const int32_t variable, const std::string& value );
		bool				updateVariable( const uint32_t serial, const int32_t variable, const std::string& value );
		bool				selectVariable( const uint32_t serial, const int32_t variable, std::string& value );
		//
		//	Integer vectors
		//
		bool 			insertVariable( const uint32_t serial, const int32_t variable, const int32_t size, const int32_t value );
		bool				updateVariable( const uint32_t serial, const int32_t variable, const int32_t index, const int32_t value );
		bool				selectVariable( const uint32_t serial, const int32_t variable, const int32_t index, int32_t& value );
		//
		//	Type aspecific methods
		//
		bool				deleteVariable( const uint32_t serial );
		bool				deleteVariable( const uint32_t serial, const int32_t variable );
		bool				existsVariable( const uint32_t serial, const int32_t variable, const int32_t type );
		int32_t				countVariable();
		int32_t				countVariable( const uint32_t serial );
		int32_t				countVariable( const uint32_t serial, const SERIAL type );
		bool				copyVariable( const uint32_t fromSerial, const SERIAL toSerial );
		bool				moveVariable( const uint32_t fromSerial, const SERIAL toSerial );
		void				saveVariable( const uint32_t serial, FILE * stream );
		int32_t				size( const uint32_t serial, const int32_t variable, const int32_t index = -1 );
};

extern amxVariableServer amxVS;
#endif
