#ifndef __AMXFILE_H__
#define __AMXFILE_H__

#include "nxwcommn.h"

typedef map< uint32_t, FILE* >	mAmxFile;
typedef mAmxFile::iterator	miAmxFile;


class amxFiles
{
	private:
		mAmxFile	files;
		uint32_t		lastHandleIssued;
	public:
				amxFiles();
				~amxFiles();
	public:
		int32_t		open( const std::string &name, const std::string &mode );
		LOGICAL		close( const int32_t handle );
		LOGICAL		eof( const int32_t handle );
		LOGICAL		write( const int32_t handle, const std::string &line );
		std::string	read( const int32_t handle );
};

extern amxFiles amxFileServer;

#endif
