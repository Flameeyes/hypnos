#!/bin/sh
grep "$1" * -r $2 | egrep -v 'errors.log|deprecated|doxyerror|\~|CVS|apidocs|\.o|Makefile|doc|/\.#'

