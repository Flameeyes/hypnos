/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#ifndef __CMDS_H
#define __CMDS_H

#include "common_libs.h"

enum PrivLevel
{
	PRIVLEVEL_ADMIN = 255,
	PRIVLEVEL_GM	= 200,
	PRIVLEVEL_SEER	= 150,
	PRIVLEVEL_CNS	= 100,
	PRIVLEVEL_PLAYER = 50,
	PRIVLEVEL_GUEST = 0,
};


/*
\brief Declaration of cCommand Class
*/

class cCommand
{
private:
	std::string cmd_name;
	int8_t cmd_level;  

public:
	cCommand( std::string& cmd_name, int8_t cmd_number, void/*AmxFunction*/* callback );
	int8_t getCommandLevel(pCommand cmd);
	void call( std::string params );
};

/*
\brief Declaration of cCallCommand Class
 cCallCommand object is created every time a command is executed and goes in CALLCMDMAP. 
 it is destroyed when the command has finished
 All parameters given by the char who call the command goes in this obj.
*/
class cCallCommand 
{

private:

	static uint32_t current_serial;
	std::map< uint32_t, cCallCommand* > callcommand_map;

public:

	cCallCommand(std::string all_params);
	~cCallCommand();
	std::string all_params;
	cCallCommand* findCallCommand(uint32_t cmd);
	uint32_t addCallCommand(cCallCommand* called_command);	
	void delCommand(uint32_t cmd);
	//std::vector< string >* single_param;

};
 
/*
\brief Declaration of cCommandMap Class
*/
class cCommandMap {

private:

	static CommandMap command_map; //!< all commands
public:
	
	cCommandMap();
	pCommand addGmCommand(std::string name, int8_t number ,void/*AmxFunction*/* callback);
	static bool Check(std::string& text);
	pCommand findCommand(std::string name);
};

#endif
