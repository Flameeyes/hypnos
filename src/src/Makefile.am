bin_PROGRAMS = hypnos
hypnos_LDADD = $(top_builddir)/src/skills/libskills.a \
	$(top_builddir)/libhypnos/libhypnos.la $(top_builddir)/src/networking/libnetworking.a \
	$(top_builddir)/src/objects/cnpc/libcnpc.a $(top_builddir)/src/objects/citem/libcitem.a \
	$(top_builddir)/src/objects/cchar/libcchar.a $(top_builddir)/src/objects/libobjects.a \
	$(top_builddir)/src/extras/libextras.a $(top_builddir)/src/encryption/libencryption.a \
	$(top_builddir)/src/backend/libbackend.a $(top_builddir)/src/archetypes/libarchetypes.a \
	$(WEFTS_LIBS)

hypnos_SOURCES = area.cpp calendar.cpp console.cpp data.cpp effects.cpp\
	gamedate.cpp gmpages.cpp hypnos.cpp logsystem.cpp magic.cpp map.cpp\
	menu.cpp misc.cpp newbies.cpp npcai.cpp npcs.cpp oldmenu.cpp\
	particles.cpp pointer.cpp settings.cpp\
	sndpkg.cpp spawn.cpp speech.cpp sregions.cpp structs.cpp\
	targeting.cpp telport.cpp timing.cpp tmpeff.cpp trade.cpp walking.cpp version.cpp

SUBDIRS = archs backend encryption extras objects skills networking archetypes
