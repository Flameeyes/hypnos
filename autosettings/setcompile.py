# -*- coding: UTF-8 -*-

from xml.dom import *
from xml.dom.minidom import *

class Setting:
	name = ''
	type = 'string'
	default = ''
	description = 'A setting'

class Group:
	grName = ''
	settings = []

groups = []

def toCType(type):
	if type == 'string': return 'std::string'
	if type == 'uint32': return 'uint32_t'
	if type == 'uint16': return 'uint16_t'
	if type == 'uint8': return 'uint8_t'
	if type == 'int32': return 'int32_t'
	if type == 'int16': return 'int16_t'
	if type == 'int8': return 'int8_t'
	if type == 'flag': return 'flag'
	return None

def formatC(setting):
	if setting.type == 'string': return '"' + setting.default + '"'
	return setting.default

settings = parse('setdesc.xml')
assert settings.documentElement.tagName == 'autosettings'

n = settings.documentElement.firstChild

while n != None:
	if n.nodeType != Node.ELEMENT_NODE:
		n = n.nextSibling
		continue
	
	assert n.tagName == "group"
	g = Group()
	g.grName = n.getAttribute("name")
	
	setn = n.firstChild
	while setn != None:		
		if setn.nodeType != Node.ELEMENT_NODE:
			setn = setn.nextSibling
			continue

		s = Setting()
		s.name = setn.getAttribute("name")
		s.type = setn.getAttribute("type")
		s.default = setn.getAttribute("default")
		setn.normalize()
		s.description = setn.childNodes.item(0).data
		
		assert(s.name)
		assert(s.type)
		g.settings.append(s)
		
		setn = setn.nextSibling
		
	groups.append(g)
	
	n = n.nextSibling

settings.unlink()

# Open files and output headers

unitcpp = open('settings.cpp', 'w')
unith = open('settings.h', 'w')

unith.write("""
// This is an autogenerated file. Please do not modify it manually.
// To modify settings, edit setdesc.xml file.

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "common_libs.h"

namespace nSettings {
""")

unitcpp.write("""
// This is an autogenerated file. Please do not modify it manually.
// To modify settings, edit setdesc.xml file.

#include "settings.h"

namespace nSettings {
""")

# Calculate flags count

flagcount = 0

for group in groups:
	for setting in group.settings:
		if type != 'flag': continue
		flagcount += 1

flagvars = flagcount / 64 +1

# Prepare flags constants and defaults

flagvar = 0
flagbit = 0

defaults = []
defaults.append("") # first flag

for group in groups:
	unitcpp.write("namespace n" + group.grName + " {\n")
	for setting in group.settings:
		if setting.type != 'flag': continue
		
		if setting.default == 'on':
			defaults[flagvar] += "\n\t\t | n" + group.grName + "::flag" + setting.name
		
		unitcpp.write("\tstatic const uint64_t flag" + setting.name + " = " + hex(1 << flagbit) + ";\n");
		
		flagbit += 1
		if flagbit >= 64:
			flagvar += flagbit/64
			flagbit %= 64
			defaults.append("")
	
	unitcpp.write("} // namespace n" + group.grName + "\n")

unitcpp.write("\n\n")

for i in range(0, flagvars):
	unitcpp.write("\tstatic uint64_t flags" + str(i) + " = 0" + defaults[i] + ";\n\n\n")

# Handle normal settings

for group in groups:
	unith.write("\tnamespace n" + group.grName + " {\n")
	unitcpp.write("namespace n" + group.grName + " {\n")
	
	for setting in group.settings:
		# Flags are handled at the end of the section, 'cause they are handled in different ways
		if setting.type == 'flag': continue
		
		unith.write(
			"\t\t" + toCType(setting.type) + " get" + setting.name + "();\n"
			"\t\tvoid set" + setting.name + "(" + toCType(setting.type) + ");\n\n"
			);
		
		unitcpp.write(
			"\tstatic " + toCType(setting.type) + " m_" + setting.name + " = " + formatC(setting) + ";\n")
		unitcpp.write(
			"\t" + toCType(setting.type) + " get" + setting.name + "()\n"
			"\t{ return m_" + setting.name + "; }\n\n"
			)
	
	unith.write("\t} // namespace n" + group.grName + "\n")
	unitcpp.write("} // namespace n" + group.grName + "\n")

# Cleanup...
unitcpp.write("""
} // namespace

""")
unitcpp.close()

unith.write("""
} // namespace

#endif

""")
unith.close()
