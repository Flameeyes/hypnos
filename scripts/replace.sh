#!/bin/sh
sed -e "s�$1�$2�g" -i `grep "$1" * -rl | egrep -v '\~|.svn|Makefile'`

