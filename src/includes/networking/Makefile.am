noinst_HEADERS = cclient.hpp exceptions.hpp misccommand.hpp network.hpp received.hpp \
	sent.hpp tkiller.hpp tpllistener.hpp tracreceiver.hpp tuoreceiver.hpp

if HAVE_PCH

noinst_PCH = $(addsuffix .gch, $(noinst_HEADERS))

all: $(noinst_PCH)

%.h.gch: %.h
	$(CC) $< -o $@
%.hpp.gch: %.hpp
	$(CC) -x c++-header $(CPPFLAGS) $< -o $@
endif
