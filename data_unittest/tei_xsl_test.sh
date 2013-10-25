#!/bin/bash
#
# tei_xsl_test.sh
#

FicOut=./tei_xsl_test.html
FicSrc=./tei_xsl_test.xml
FicXsl=../xsl/tei_html.xsl

XsltParam="-param generate.toc 0 -param suppress.navigation 1 -param chapter.autolabel 0 -param man.indent.refsect 0"

xalan -indent 2 -param atoll.highwords "' 13 14 '" $XsltParam -out $FicOut -in $FicSrc -xsl $FicXsl
