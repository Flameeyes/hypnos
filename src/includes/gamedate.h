/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#ifndef __CGAMEDATE__
#define __CGAMEDATE__

class cGameDate
{
	enum eDateFormat
	{
		YMD,
		DMY
	};

	private:
		static	char		dateSeparator;
		static	char		timeSeparator;
		static	char		dateTimeSeparator;
		static	eDateFormat	dateFormat;
	public:
		static	char		getDateSeparator();
		static	void		setDateSeparator( char separator );
		static	char		getTimeSeparator();
		static	void		setTimeSeparator( char separator );
		static	char		getDateTimeSeparator();
		static	void		setDateTimeSeparator( char separator );
	public:
					cGameDate();
					cGameDate( const cGameDate &copy );
					~cGameDate();
	private:
		uint16_t			year;
		uint8_t			month;
		uint8_t			day;
		uint8_t			hour;
		uint8_t			minute;
	public:
		uint16_t			getYear();
		uint8_t			getMonth();
		uint8_t			getDay();
		uint8_t			getHour();
		uint8_t			getMinute();
		void			setYear( uint16_t newYear );
		void			setMonth( uint8_t newMonth );
		void			setDay( uint8_t newDay );
		void			setHour( uint8_t newHour );
		void			setMinute( uint8_t newMinute );
		std::string		toDateString();
		std::string		toDateString( eDateFormat format );
		std::string		toTimeString();
		std::string		toString();
		std::string		toString( eDateFormat format );
		void			fromString( const std::string& arg );
		void			fromString( const std::string& arg, eDateFormat format );
	private:
		void			setDefaultDate();
		void			setDefaultYear();
		void			setDefaultMonth();
		void			setDefaultDay();
		void			setDefaultTime();
		void			setDefaultHour();
		void			setDefaultMinute();
};

#endif
