/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declaration of nCommands namespace and Flameeyes's Commands system
*/

#ifndef __COMMANDS_H__
#define __COMMANDS_H__

//! Commands related stuff
namespace nCommands {

	/*!
	\brief Register a new user-defined command in the hash map
	\param command Name of the command to intercept
	\param handler Function handle to call
	\param force Force the overwrite of the command, so if there's
		already a command with the same name will be removed.
	\return True if the register is done, false if there's already a
		command with that name
	\warning Using the force parameter on a system command will cause
		the command to be available no more, until the emulator is
		reloaded without the forced overridden command
	\todo Find a way to avoid the warning above
	\note This function should be provided to the scripting interface,
		which should change the pFunctionHandle in a suitable
		type for accept new functions from the interface. To read
		the commands from the xml-scripts we should have a way
		to reload all the commands from scratch. This will be
		quite resource-spending, but will allow us to not reload
		the emulator to reload the commands.
	*/
	bool registerScriptedCommand(std::string command, pFunctionHandle handler, bool force = false);
	
	/*!
	\brief Unregister a user-defined command in the hash map
	\param command Name of the command to release
	\return True if the command can be unloaded, false if not found or
		not user-defined
	\note To disallow a system command either raise it's privlevel to
		over-owner in commands.xml or disable it also in commands.xml or
		override it with a null comamnd
	*/
	bool unregisterScriptedCommand(std::string command);
	
	/*!
	\brief Calls a command, either user-defined or system
	\param command Command to call
	\param quiet Doesn't show up messages for non-existant commands,
		erroneous command, or priviledged commands
	
	This function will call the given command, looking at it in the hash amp
	and then executing the instance of cCommand subclass.
	The parameter quiet can be used by scripts to avoid the user to see
	eventual errors in the script.
	*/
	void callCommand(std::string command, bool quiet = false);

	/*!
	\brief Base class for all commands
	
	This class must be subclassed by single commands classes to add 
	commands at the emulator's list.
	The special cScriptedCommand subclass is used for user-defined
	commands which executes a scripted function.
	*/
	class cCommand {
	public:
		/*!
		\brief Default constructor
		\param plevel Priviledge Level at which the command can be used
		*/
		cCommand(uint8_t plevel) :
			privLevel (plevel)
		{ }
		
		virtual bool parseParams(pClient client, std::string command) = 0;
		virtual void execute(pClient client) = 0;
	protected:
		cVariantVector params;	//!< Params used by the command
		uint8_t privLevel;	//!< Priviledge level at which the command can be used
	};
	
	class cScriptedCommand : cCommand {
	public:
		/*!
		\brief Default constructor with pFunctionHandle param
		\param plevel Level at which the command can be used (passed to cCommand)
		\param func Function to be called by the command
		*/
		cScriptedCommand(uint8_t plevel, pFunctionHandle func) :
			cCommand(plevel), hndl(func)
		{ }
		
		/*!
		\brief Dummy parse parameters
		\param client Client who sent the command
		\param command Command to be executed (with parameters)
		\return always true
		
		This function simply adds the client and the entire string to
		the parameters list the scripted function needs to parse the string
		by itself.
		*/
		void parserParams(pClient client, std::string command)
		{ params[0] = client; params[1] = cVariant(command);
		  return true; }
		
		/*!
		\brief Execute the command received
		\param client Client which uses the command
			(unused, passed to the called script function as first parameter)
		*/
		void execute(pClient client)
		{
			hndl->setParams(params);
			hndl->execute();
		}
	protected:
		pFunctionHandle hndl;
	};
	
};

#endif
