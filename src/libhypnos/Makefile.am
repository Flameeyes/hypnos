SUBDIRS = muls libhypnos

# set the include path found by configure
INCLUDES= $(all_includes)

if BINRELOC
	BRSRCS = prefix.c
else
	BRSRCS = 
endif

# the library search path.
lib_LTLIBRARIES = libhypnos.la
libhypnos_la_SOURCES = cvariant.cpp strings.cpp $(BRSRCS)
EXTRA_libhypnos_la_SOURCES = prefix.c
libhypnos_la_LIBADD = $(top_builddir)/libhypnos/muls/libhypmul.la $(CPPLIB)
libhypnos_la_LDFLAGS = -module -no-undefined
