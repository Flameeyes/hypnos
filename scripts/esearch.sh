#!/bin/sh
egrep "$1" src includes libhypnos -r $2 | egrep -v '\~|.svn|Makefile'
