#!/bin/sh
grep "$1" * -r $2 | egrep -v 'errors.log|deprecated|doxyerror|\~|.svn|apidocs|\.o|Makefile|doc|.libs|.deps'
