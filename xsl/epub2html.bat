@echo off
setlocal

set path=%path%;%XALANCROOT%\bin;%XERCESCROOT%\bin

set FicSrc=.\fb.opf
set FicOut=.\epub.html
set FicXsl=D:\_cvs\dev\atoll\xsl\epub2html.xsl

cd /D "%1"

xalan -i 2 -m -o %FicOut% %FicSrc% %FicXsl%

endlocal
