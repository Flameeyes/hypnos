#!/bin/sh
sed -e "s/$2/$3/g" -i `egrep "$1" libhypnos src includes -rl | egrep -v '\~|.svn|Makefile'`

