/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2003 - NoX-Wizard Project                                  |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "backend/admincmds.h"

/*!
\brief Parses an administrative command
\param str Command to parse
\param outs Stream where to output the command's... output!
*/
void nAdminCommands::parseCommand(const std::string &str, std::ostream &outs)
{
	switch(str[0])
	{
	case '\x1B':
		keeprun=false;
		break;
	case 'Q':
	case 'q':
		lowlevelOutput(outs, "Immediate Shutdown initialized!\n");
		keeprun=false;
		break;
	case 'T':
	case 't':
		endtime=getClockmSecs()+(SECS*60*2);
		endmessage(0);
		break;
	case '#':
		if ( !cwmWorldState->Saving() )
		{
			cwmWorldState->saveNewWorld();
		}
		break;
	case 'W':
	case 'w':				// Display logged in chars
		lowlevelOutput(outs, "----------------------------------------------------------------\n");
		lowlevelOutput(outs, "Current Users in the World:\n");
		//!\todo Fix this with a better way
		j = 0;  //Fix bug counting ppl online.
		for (int i=0;i<now;i++)
		{
			pChar pc_i=cSerializable::findCharBySerial(currchar[i]);
			if(pc_i && clientInfo[i]->ingame) //Keeps NPC's from appearing on the list
			{
				lowlevelOutput(outs, "%i) %s [ %08x ]\n", j, pc_i->getCurrentName().c_str(), pc_i->getSerial());
				j++;
			}
		}
		lowlevelOutput(outs, "Total Users Online: %d\n", j);
		break;
	case 'r':
	case 'R':
		lowlevelOutput(outs, "Hypnos: Total server reload!");
		//! \todo Need to freeze and unfreeze all the clients here for the resync
		//! \todo Need to call a function exported by hypnos.h
		loadServer();
		break;
	case '?':
		lowlevelOutput(outs, "Console commands:\n");
		lowlevelOutput(outs, "\tQ - Shutdown the server.\n");
		lowlevelOutput(outs, "\tT - System Message: The server is shutting down in 2 minutes.\n");
		lowlevelOutput(outs, "\t# - Save world\n");
		lowlevelOutput(outs, "\tW - Display logged in characters\n");
		lowlevelOutput(outs, "\tR - Total server reload\n");
		lowlevelOutput(outs, "\tS - Toggle Secure mode %s\n", secure ? "[enabled]" : "[disabled]" );
		lowlevelOutput(outs, "\t? - Commands list (this)\n");
		lowlevelOutput(outs, "End of commands list.\n");
		break;
	default:
		lowlevelOutput(outs, "Key %c [%x] does not preform a function.\n",c,c);
		break;
	}
}
