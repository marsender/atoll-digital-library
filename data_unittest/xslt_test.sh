#!/bin/bash
#
# xslt_test.sh
#

FicOut=./xslt_test.html
FicSrc=./xslt_test.xml
FicXsl=./xslt_test.xsl

xalan -indent 2 -out $FicOut -in $FicSrc -xsl $FicXsl
