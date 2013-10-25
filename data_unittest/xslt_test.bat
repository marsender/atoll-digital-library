@echo off
setlocal

set path=%path%;%XALANCROOT%\bin;%XERCESCROOT%\bin

set FicSrc=.\xslt_test.xml
set FicOut=.\xslt_test.html
set FicXsl=.\xslt_test.xsl

xalan -i 2 -m -o %FicOut% %FicSrc% %FicXsl%
rem set xmlutil=%RepDev%\tools\xmlutil\vc_msw\xmlutil_console.exe
rem %xmlutil% --work=transform --valid=always --type=file --input=%FicSrc% --output=%FicOut% --stylesheet=%FicXsl% --catalog="C:\Program Files\XML Copy Editor\catalog\catalog"
pause
