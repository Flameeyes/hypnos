libincludedir = $(includedir)/libhypnos/muls
libinclude_HEADERS = cliloc.hpp gumpidx.hpp indexedfile.hpp mmappedfile.hpp mulfiles.hpp \
	multi.hpp multiidx.hpp tiledata.hpp

if HAVE_PCH

libinclude_PCH = $(addsuffix .gch, $(libinclude_HEADERS))

all: $(libinclude_PCH)

%.h.gch: %.h
	$(CC) $< -o $@
%.hpp.gch: %.hpp
	$(CC) -x c++-header $(CPPFLAGS) $< -o $@
endif
