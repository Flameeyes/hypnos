/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "basics.h"
#include "inlines.h"

/*!
\author Luxor
\brief Returns distance between two points.
*/
const double dist( const Location a, const Location b, bool countZ )
{
        int16_t xDiff = a.x - b.x;
        int16_t yDiff = a.y - b.y;
	double distance = hypot( abs( xDiff ), abs( yDiff ) );
	if ( !countZ || a.z == b.z )
		return distance;
	
	double distZ = abs( a.z - b.z );
	return hypot( distance, distZ );
}

/*!
\brief Returns a random number between bounds
\return int the number
\param nLowNum lower bound
\param nHighNum higher bound
*/
int RandomNum(int nLowNum, int nHighNum)
{
	if (nHighNum - nLowNum + 1)
		return ((rand() % (nHighNum - nLowNum + 1)) + nLowNum);
	else
		return nLowNum;
}

/*!
\brief fills an integer array with tokens extracted from a string
\author Xanathar
\return int the number of number read from the string
\param str the string
\param array the array
\param maxsize the size of array
\param defval -1 -> the default value for uninitialized items
\param base the base for number conversion
*/
int fillIntArray(char* str, int *array, int maxsize, int defval, int base)
{
	int i=0;
	char tmp[1048];
	char *mem;

	if (strlen(str) > 1024) {
		mem = new char[strlen(str)+5];
	} else mem = tmp;  //xan -> we avoid dyna-alloc for strs < 1K
	strcpy(tmp, str);

	char *s;
	char *delimiter = " ";

	if (base != baseInArray) for (i = 0; i < maxsize; i++) array[i] = defval;

	i = 0;

	s = strtok(tmp,delimiter);

	while ((s!=NULL)&&(i < maxsize)) {
		if (base == baseInArray) {
			array[i] = str2num(s, array[i]);
			i++;
		}
		else {
			array[i++] = str2num(s, base);
		}
		s = strtok(NULL, delimiter);
	}

	if (mem != tmp) safedeletearray(mem);
	return i;
}

/*!
\brief reads a line splitted (read2 clone)
\author Xanathar
\param F the file to read from
\param script1 where we'll put the first token of the string
\param script2 where we'll put the rest of the string
*/
void readSplitted(FILE* F, char* script1, char* script2)
{
	char str[1024];
	fgets(str, 1000, F);
	str[1000] = '\0';
	str[strlen(str)-1] = '\0';

	int ln = strlen(str)-1;
	if (ln>0) {
		if ((str[ln]== '\r')||(str[ln]== '\n')) {
			str[ln] = '\0';
		} // remove CR+LF :]
	}

	int i=0;
	script1[0]=0;
	script2[0]=0;

	while((str[i]!=0) && (str[i]!=' ') && (str[i]!='=') && (i<1024) )
	{
		i++;
	}

	strncpy(script1, str, i);
	script1[i]=0;
	if ((script1[0]!='}') && (str[i]!=0)) strcpy(script2,(char*)( str+i+1));
}

/*!
\brief return current local time
\return char* the time (time_str)
\param time_str a string with enough memory allocated
*/
char *RealTime(char *time_str)
{
	struct tm *curtime;
	time_t bintime;
	time(&bintime);
	curtime = localtime(&bintime);
	strftime(time_str, 256, "%A %B %d %I:%M:%S %p %Y %z", curtime);
	return time_str;
}
