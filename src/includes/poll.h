  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#ifndef __CPOLL_
#define __CPOLL_


#include "gamedate.h"

class cPoll
{
	public:
				cPoll();
				~cPoll();
	private:
		uint32_t		serial;
	public:
		uint32_t		getSerial();
		void		setSerial( const uint32_t serial );
	private:
		std::string	question;
	public:
		std::string	getQuestion();
		void		setQuestion( const std::string &question );
	private:
		uint32_t		runForDays;
	public:
		uint32_t		getRunForDays();
		void		setRunForDays( const uint32_t days );
	
	typedef map< uint32_t, std::string > mPollOption	;
	typedef mPollOption::iterator	 miPollOption	;
	
	private:
		mPollOption	pollOptions;
	public:
		std::string	getPollOption( const uint32_t option );
		void		setPollOption( const uint32_t option, const std::string &optionText );
	
	typedef map< uint32_t, cGameDate > mPollVoter;
	typedef mPollVoter::iterator miPollVoter;
	
	private:
		mPollVoter	pollVoters;
	public:
		bool		castVote( const uint32_t voterSerial, const uint32_t option );
};

#endif

