  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
 

#include "common_libs.h"
#include "calendar.h"
#include "gamedate.h"
#include "basics.h"

char cGameDate::dateSeparator = '-';
char cGameDate::timeSeparator = ':';
char cGameDate::dateTimeSeparator = ' ';
cGameDate::eDateFormat cGameDate::dateFormat = YMD;

char cGameDate::getDateSeparator()
{
	return dateSeparator;
}

void cGameDate::setDateSeparator( char separator )
{
	dateSeparator = separator;
}

char cGameDate::getTimeSeparator()
{
	return timeSeparator;
}

void cGameDate::setTimeSeparator( char separator )
{
	timeSeparator = separator;
}

char cGameDate::getDateTimeSeparator()
{
	return dateTimeSeparator;
}

void cGameDate::setDateTimeSeparator( char separator )
{
	dateTimeSeparator = separator;
}

cGameDate::cGameDate()
{
	setDefaultDate();
}

cGameDate::cGameDate( const cGameDate &copy )
{
	year 	= copy.year;
	month	= copy.month;
	day	= copy.day;
	hour	= copy.hour;
	minute	= copy.minute;
}

cGameDate::~cGameDate()
{
}

void	cGameDate::setDefaultDate()
{
	setDefaultYear()	;
	setDefaultMonth()	;
	setDefaultDay()		;
	setDefaultTime()	;
}

void	cGameDate::setDefaultYear()
{
	year 	= Calendar::g_nYear;
}

void	cGameDate::setDefaultMonth()
{
	month	= Calendar::g_nMonth;
}

void	cGameDate::setDefaultDay()
{
	day	= Calendar::g_nDay;
}

void	cGameDate::setDefaultTime()
{
	setDefaultHour();
	setDefaultMinute();
}

void	cGameDate::setDefaultHour()
{
	hour	= Calendar::g_nHour;
}

void	cGameDate::setDefaultMinute()
{
	minute	= Calendar::g_nMinute;
}

uint16_t cGameDate::getYear()
{
	return year;
}

uint8_t cGameDate::getMonth()
{
	return month;
}

uint8_t cGameDate::getDay()
{
	return day;
}

uint8_t cGameDate::getHour()
{
	return hour;
}

uint8_t cGameDate::getMinute()
{
	return minute;
}

void cGameDate::setYear( uint16_t newYear )
{
	year = newYear;
}

void cGameDate::setMonth( uint8_t newMonth )
{
	month = newMonth;
}

void cGameDate::setDay( uint8_t newDay )
{
	day = newDay;
}

void cGameDate::setHour( uint8_t newHour )
{
	hour = newHour;
}

void cGameDate::setMinute( uint8_t newMinute )
{
	minute = newMinute;
}

std::string cGameDate::toDateString( eDateFormat format )
{
	char temp[11];
	switch( format )
	{
		case YMD :
			sprintf( temp, "%04d%c%02d%c%02d", year, dateSeparator, month, dateSeparator, day );
			break;
		case DMY :
			sprintf( temp, "%02d%c%02d%c%04d", day, dateSeparator, month, dateSeparator, year );
			break;
	}
	return std::string(temp);
}

std::string cGameDate::toDateString()
{
	return toDateString( dateFormat );
}

std::string cGameDate::toTimeString()
{
	char temp[6];
	sprintf( temp, "%02d%c%02d", hour, timeSeparator, minute );
	return std::string(temp);
}

std::string cGameDate::toString()
{
	return toString( dateFormat );
}

std::string cGameDate::toString( eDateFormat format )
{
	return toDateString( format ) + dateTimeSeparator + toTimeString();
}

void cGameDate::fromString( const std::string& arg )
{
	fromString( arg, dateFormat );
}

void cGameDate::fromString( const std::string& arg, eDateFormat format )
{
	LOGICAL success = false;

	if( !arg.empty() )
	{
		uint32_t start = 0;
		uint32_t index = arg.find_first_of( dateSeparator );

		switch( format )
		{
			case YMD:
				if( index == start + 4 )
				{
					setYear( (uint16_t) str2num( const_cast<char*>( arg.substr( start, 4 ).c_str() ) ) );
					start = index + 1;
					index = arg.find_first_of( dateSeparator, start );
					if( index == start + 2 )
					{
						setMonth( (uint8_t) str2num( const_cast<char*>( arg.substr( start, 2 ).c_str() ) ) );
						start = index + 1;
						index = arg.find_first_of( dateTimeSeparator, start );
						if( index == start + 2 )
						{
							setDay( (uint8_t) str2num( const_cast<char*>( arg.substr( start, 2 ).c_str() ) ) );
							start = index + 1;
							index = arg.find_first_of( timeSeparator, start );
							if( index == start + 2 )
							{
								setHour( (uint8_t) str2num( const_cast<char*>( arg.substr( start, 2 ).c_str() ) ) );
								start = index + 1;
								setMinute( (uint8_t) str2num( const_cast<char*>( arg.substr( start, 2 ).c_str() ) ) );
								success = true;
							}
						}
					}
				}
				break;
			case DMY:
				if( index == start + 2 )
				{
					setDay( (uint8_t) str2num( const_cast<char*>( arg.substr( start, 2 ).c_str() ) ) );
					start = index + 1;
					index = arg.find_first_of( dateSeparator, start );
					if( index == start + 2 )
					{
						setMonth( (uint8_t) str2num( const_cast<char*>( arg.substr( start, 2 ).c_str() ) ) );
						start = index + 1;
						index = arg.find_first_of( dateTimeSeparator, start );
						if( index == start + 4 )
						{
							setYear( (uint16_t) str2num( const_cast<char*>( arg.substr( start, 4 ).c_str() ) ) );
							start = index + 1;
							index = arg.find_first_of( timeSeparator, start );
							if( index == start + 2 )
							{
								setHour( (uint8_t) str2num( const_cast<char*>( arg.substr( start, 2 ).c_str() ) ) );
								start = index + 1;
								setMinute( (uint8_t) str2num( const_cast<char*>( arg.substr( start, 2 ).c_str() ) ) );
								success = true;
							}
						}
					}
				}
				break;
		}
	}

	if( !success )
		setDefaultDate();
}

