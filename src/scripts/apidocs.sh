#!/bin/sh

CURR=`pwd`

rm -rf apidocs/html

doxygen

FILES=`find apidocs/html -name \*html`
sed -e 's/html"/html.gz"/g' -i $FILES
gzip $FILES 

