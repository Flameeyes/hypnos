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

