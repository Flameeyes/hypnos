/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declarations of classes for abstract event handling
*/

#ifndef __SCRIPTING_H__
#define __SCRIPTING_H__

#include "common_libs.h"
#include "typedefs.h"
#include "libhypnos/cvariant.h"

/*!
\class cScriptingEngine scripting.h "backend/scripting.h"
\brief Class for access to the dynamic loadded scripting engine
*/
class cScriptingEngine {
protected:
	static cScriptingEngine *engine;	//!< Pointer to the scripting engine used
public:
	static void initialize();
	
	static cScriptingEngine *getEngine()
	{ return engine; }

	/*!
	\class cFunctionHandle scripting.h "backend/scripting.h"
	\brief Handle to a scripting function
	*/
	class cFunctionHandle {
		friend class cScriptingEngine;
		public:
			/*!
			\brief Sets the params for the function to call
			\param aParams vector of params to set
			*/
			virtual void setParams(cVariantVector &aParams)
			{ params = aParams; }
			
			//! Executes the function
			virtual cVariant execute() = 0;
			
			inline bool isBypassed() const
			{ return bypassed; }
			
			virtual ~cFunctionHandle();
			
		protected:
			cFunctionHandle();
			cVariantVector params;
			bool bypassed;
	};

	cFunctionHandle *getFunction(std::string funcName);
	
protected:
	cScriptingEngine();
	virtual ~cScriptingEngine() { };
};

typedef cScriptingEngine::cFunctionHandle *pFunctionHandle;
typedef std::vector<pFunctionHandle> FunctionVector;

extern "C" {
	cScriptingEngine *initEngine();
	void quitEngine(cScriptingEngine *);
}

#endif
