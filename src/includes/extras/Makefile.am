noinst_HEADERS = bounty.hpp jails.hpp loginserver.hpp motd.hpp

if HAVE_PCH

noinst_PCH = $(addsuffix .gch, $(noinst_HEADERS))

all: $(libinclude_PCH)

%.h.gch: %.h
	$(CC) $< -o $@
%.hpp.gch: %.hpp
	$(CC) -x c++-header $(CPPFLAGS) $< -o $@
endif

