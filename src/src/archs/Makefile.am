if WIN32
	WIN32SRCS = ntservice.cpp win32registry.cpp
else
	WIN32SRCS = 
endif

noinst_LIBRARIES = libarchs.a
libarchs_a_SOURCES =  signals.cpp console.cpp tinterface.cpp $(WIN32SRCS)
