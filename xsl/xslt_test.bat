@echo off

set FicSrc=.\xslt_test.xml
set FicOut=.\xslt_test.html
set FicXsl=.\xslt_test.xsl

xalan -i 2 -m -o %FicOut% %FicSrc% %FicXsl%
pause
