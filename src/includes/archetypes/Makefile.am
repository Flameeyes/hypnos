noinst_HEADERS = generic.hpp items.hpp

if HAVE_PCH

noinst_PCH = $(addsuffix .gch, $(noinst_HEADERS))

all: $(libinclude_PCH)

%.h.gch: %.h
	$(CC) $< -o $@
%.hpp.gch: %.hpp
	$(CC) -x c++-header $(CPPFLAGS) $< -o $@
endif
