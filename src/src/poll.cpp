  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "common_libs.h"
#include "poll.h"


cPoll::cPoll()
{
}

cPoll::~cPoll()
{
}

uint32_t cPoll::getSerial()
{
	return serial;
}

void cPoll::setSerial( const uint32_t serial )
{
	this->serial = serial;
}

std::string cPoll::getQuestion()
{
	return question;
}

void cPoll::setQuestion( const std::string &question )
{
	this->question = question;
}

uint32_t cPoll::getRunForDays()
{
	return runForDays;
}

void cPoll::setRunForDays( uint32_t days )
{
	this->runForDays = days;
}

std::string cPoll::getPollOption( const uint32_t option )
{
	if( !pollOptions.empty() )
	{
		miPollOption pOption( pollOptions.begin() );

		if( pOption != pollOptions.end() )
			return pOption->second;
	}
	return "";
}

void cPoll::setPollOption( const uint32_t option, const std::string &optionText )
{
	pollOptions[ option ] = optionText;
}

bool cPoll::castVote( const uint32_t voterSerial, const uint32_t option )
{
	if( pollVoters.find( voterSerial ) != pollVoters.end() )
	{
		pollVoters[ voterSerial ] = cGameDate();
		return true;
	}
	return false;
}
