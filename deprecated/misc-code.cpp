void location2xyz(int loc, int& x, int& y, int& z)
{
	int  loopexit=0;
	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	char script1[1024];
	char script2[1024];


	sprintf(temp, "SECTION LOCATION %i", loc);
	iter = Scripts::Location->getNewIterator(temp);

	if ((iter!=NULL))
	{
		do
		{
			iter->parseLine(script1, script2);
			if (!(strcmp(script1,"X")))
			{
				x=str2num(script2);
			}
			else if (!(strcmp(script1,"Y")))
			{
				y=str2num(script2);
			}
			else if (!(strcmp(script1,"Z")))
			{
				z=str2num(script2);
			}
		}
		while ( (strcmp(script1,"}")) && (++loopexit < MAXLOOPS) );
	}
	safedelete(iter);
}

int strtonum(int countx, int base)
{
	char *err= NULL;
	int n;

	if(comm[countx] == NULL)
		return 0;

	n= strtol((char*)comm[countx], &err, base);

	if(*err != '\0')	// invalid character found
	{
		WarnOut("error in strtonum: %c invalid digit for base %2d\n", *err, base);
		return 0;
	}

	return n;
}

/*!
\brief converts hex string comm[countx] to int
\param countx the line of comm array to convert
*/
int hexnumber(int countx)
{
	// sscanf is an ANSI function to read formated data from a string.
	if (comm[countx] == NULL)
		return 0;

	int i;
	sscanf((char*)comm[countx], "%x", &i);

	return i;
}

/*!
\todo write documentation
*/
int whichbit( int number, int bit )
{
	int i, setbits = 0, whichbit = 0, intsize = sizeof(int) * 8;

	for( i=0; i<intsize; i++ )
	{
		if( number & 0x1 ) setbits++;

		if( setbits == bit )
		{
			whichbit = i+1;
			break;
		}
		number >>= 1;
	}

	return whichbit;
}

