@echo off
setlocal

set path=%path%;%XALANCROOT%\bin;%XERCESCROOT%\bin

set FicSrc=.\tei_xsl_test.xml
set FicOut=.\tei_xsl_test.html
set FicXsl=..\xsl\tei_html.xsl

rem set XsltParam=-p html.stylesheet 'corpstyle.css' -p para.propagates.style 1 -p phrase.propagates.style 1
set XsltParam=-p atoll.highwords "' 13 14 '"

rem xalan -i 2 -m %XsltParam% -o %FicOut% %FicSrc% %FicXsl%
set xmlutil=%RepDev%\tools\xmlutil\vc_msw\xmlutil_console.exe
%xmlutil% --work=transform --valid=always --type=file --input=%FicSrc% --output=%FicOut% --stylesheet=%FicXsl% --catalog="C:\Program Files\XML Copy Editor\catalog\catalog"
pause
