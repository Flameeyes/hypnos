noinst_HEADERS = alchemy.hpp begging.hpp extractionskills.hpp fishing.hpp music.hpp \
	skills.hpp

if HAVE_PCH

noinst_PCH = $(addsuffix .gch, $(noinst_HEADERS))

all: $(libinclude_PCH)

%.h.gch: %.h
	$(CC) $< -o $@
%.hpp.gch: %.hpp
	$(CC) -x c++-header $(CPPFLAGS) $< -o $@
endif
