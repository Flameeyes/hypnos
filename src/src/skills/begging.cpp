/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Begging methods implementation
*/

TIMERVAL Begging::timer;
UI32 Begging::range;
std::string Begging::text[3];

void Begging::initialize()
{
	timer = 300;
	range = 3;
	text[0] = "Could thou spare a few coins?";
	text[1] = "Hey buddy can you spare some gold?";
	text[2] = "I have a family to feed, think of the children.";
}
