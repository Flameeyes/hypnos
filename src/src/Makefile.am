bin_PROGRAMS = hypnos
hypnos_LDADD = $(top_builddir)/src/skills/libskills.la \
	$(top_builddir)/libhypnos/libhypnos.la $(top_builddir)/src/networking/libnetworking.la \
	$(top_builddir)/src/objects/cnpc/libcnpc.la $(top_builddir)/src/objects/citem/libcitem.la \
	$(top_builddir)/src/objects/cchar/libcchar.la $(top_builddir)/src/objects/libobjects.la \
	$(top_builddir)/src/extras/libextras.la $(top_builddir)/src/encryption/libencryption.la \
	$(top_builddir)/src/backend/libbackend.la $(top_builddir)/src/archetypes/libarchetypes.la
hypnos_SOURCES = area.cpp calendar.cpp console.cpp data.cpp effects.cpp\
	gamedate.cpp gmpages.cpp hypnos.cpp logsystem.cpp magic.cpp map.cpp\
	menu.cpp misc.cpp newbies.cpp npcai.cpp npcs.cpp oldmenu.cpp\
	particles.cpp pointer.cpp settings.cpp\
	sndpkg.cpp spawn.cpp speech.cpp sregions.cpp structs.cpp\
	targeting.cpp telport.cpp timing.cpp tmpeff.cpp trade.cpp walking.cpp

SUBDIRS = archs backend encryption extras objects skills networking archetypes
