noinst_HEADERS = console.hpp ntservice.hpp signals.hpp tinterface.hpp \
	win32registry.hpp

if HAVE_PCH

noinst_PCH = $(addsuffix .gch, $(noinst_HEADERS))

all: $(libinclude_PCH)

%.h.gch: %.h
	$(CC) $< -o $@
%.hpp.gch: %.hpp
	$(CC) -x c++-header $(CPPFLAGS) $< -o $@
endif
