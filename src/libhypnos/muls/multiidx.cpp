/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypnos/muls/multiidx.h"
#include "libhypnos/exceptions.h"

namespace nLibhypnos {
namespace nMULFiles {

/*!
\brief Constructor

This function simply calls the tplIndexFile constructor with the path given by
getMULpath() function and the filename \c multi.idx .

*/
fMultiIDX::fMultiIDX()
	: tplIndexFile<cMultiIDX>( nMULFiles::getMULpath() + "multi.idx" )
{
}

fMultiIDX::fMultiIDX(const string &filepath)
	: tplIndexFile<cMultiIDX>( filepath )
{
}

}}
