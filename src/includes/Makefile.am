SUBDIRS = archs backend encryption extras objects skills networking archetypes .

# Some include files should never be precompiled:
# typedefs.hpp is included by common_libs, should never be included by itself.
noinst_HEADERS = area.hpp clock.hpp commands.hpp  common_libs.hpp constants.hpp \
	data.hpp dirs.hpp effects.hpp enums.hpp gmpages.hpp hypnos.hpp inlines.hpp \
	logsystem.hpp magic.hpp mainloop.hpp map.hpp menu.hpp misc.hpp mount.hpp \
	newbies.hpp npcs.hpp oldmenu.hpp particles.hpp platform.hpp pointer.hpp \
	settings.hpp sndpkg.hpp spawn.hpp specialprops.hpp speech.hpp sregions.hpp \
	structs.hpp telport.hpp timing.hpp tmpeff.hpp trade.hpp version.hpp walking.hpp

if HAVE_PCH

noinst_PCH = $(addsuffix .gch, $(noinst_HEADERS))

all: $(noinst_PCH)

%.h.gch: %.h
	$(CC) $< -o $@
%.hpp.gch: %.hpp
	$(CC) -x c++-header $(CPPFLAGS) $< -o $@
endif
