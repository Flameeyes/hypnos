
# set the include path found by configure
INCLUDES= $(all_includes)

# the library search path.
lib_LTLIBRARIES = libhypnos.la
libhypnos_la_SOURCES = cvariant.cpp mmappedfile.cpp strings.cpp prefix.c
SUBDIRS = muls
libhypnos_la_LIBADD = $(top_builddir)/libhypnos/muls/libhypmul.la
