SUBDIRS = muls hypstl

libincludedir = $(includedir)/libhypnos
libinclude_HEADERS = commons.hpp exceptions.hpp strings.hpp cvariant.hpp prefix.hpp \
	types.hpp

if HAVE_PCH

libinclude_PCH = $(addsuffix .gch, $(libinclude_HEADERS))

all: $(libinclude_PCH)

%.h.gch: %.h
	$(CC) $< -o $@
%.hpp.gch: %.hpp
	$(CC) -x c++-header $(CPPFLAGS) $< -o $@
endif
