SUBDIRS = citem .

noinst_HEADERS = caccount.hpp cbody.hpp cchar.hpp ceventthrower.hpp citem.hpp \
	cnpc.hpp cobject.hpp cparty.hpp cpc.hpp cserializable.hpp

if HAVE_PCH

noinst_PCH = $(addsuffix .gch, $(noinst_HEADERS))

all: $(noinst_PCH)

%.h.gch: %.h
	$(CC) $< -o $@
%.hpp.gch: %.hpp
	$(CC) -x c++-header $(CPPFLAGS) $< -o $@
endif

