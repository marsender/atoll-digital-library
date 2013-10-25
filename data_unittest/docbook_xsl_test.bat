@echo off
setlocal

set path=%path%;%XALANCROOT%\bin;%XERCESCROOT%\bin

set FicSrc=.\docbook_xsl_test.xml
set FicOut=.\docbook_xsl_test.html
set FicXsl=..\xsl\docbook_html.xsl

rem set XsltParam=-p html.stylesheet 'corpstyle.css' -p para.propagates.style 1 -p phrase.propagates.style 1
set XsltParam=-p atoll.highwords "' 13 14 '" -p generate.toc 0 -p suppress.navigation 1 -p chapter.autolabel 0 -p man.indent.refsect 0

xalan -i 2 -m %XsltParam% -o %FicOut% %FicSrc% %FicXsl%
rem set xmlutil=%RepDev%\tools\xmlutil\vc_msw\xmlutil_console.exe
rem %xmlutil% --work=transform --valid=always --type=file --input=%FicSrc% --output=%FicOut% --stylesheet=%FicXsl% --catalog="C:\Program Files\XML Copy Editor\catalog\catalog"
pause
