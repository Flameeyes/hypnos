noinst_HEADERS = cboat.hpp cbook.hpp ccontainer.hpp cequippablecontainer.hpp \
	cequippable.hpp chouse.hpp cmap.hpp cmsgboard.hpp cmulti.hpp cweapon.hpp

if HAVE_PCH

noinst_PCH = $(addsuffix .gch, $(noinst_HEADERS))

all: $(libinclude_PCH)

%.h.gch: %.h
	$(CC) $< -o $@
%.hpp.gch: %.hpp
	$(CC) -x c++-header $(CPPFLAGS) $< -o $@
endif
