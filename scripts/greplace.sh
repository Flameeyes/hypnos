#!/bin/sh
gsed -e "s/$1/$2/g" -i `grep "$1" * -rl | egrep -v 'errors.log|deprecated|doxyerror|\~|.svn|apidocs|\.o|Makefile|doc|.libs|.deps'`

