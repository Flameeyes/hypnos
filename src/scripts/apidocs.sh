#!/bin/sh

CURR=`pwd`

rm -rf apidocs/html

doxygen
cd apidocs/html

FILES=`find -name \*html`
sed -e 's/html/html.gz/g' -i $FILES
gzip $FILES

