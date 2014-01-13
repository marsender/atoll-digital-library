@echo off

set action=%1
if "%action%"=="" set action=Build
if "%action%"=="Build" goto FinAction
if "%action%"=="UnitTest" goto FinAction
if "%action%"=="Install" goto FinAction
goto Usage
:FinAction

set config=%2
if "%config%"=="" set config=Release
if "%config%"=="Release" goto FinConfig
if "%config%"=="Debug" goto FinConfig
goto Usage
:FinConfig

cd /D %ATOLLROOT%

if not "%action%"=="Build" goto FinBuild
echo *** %action% %config% ***
vcbuild /nologo win\atoll.sln "%config%|win32"
if %ERRORLEVEL% EQU 0 goto FinBuildLog
start file:///d:/work/dev/atoll/win/atoll_engine/%config%/BuildLog.htm
goto Error
:FinBuildLog
rem set action=UnitTest
:FinBuild

if not "%action%"=="UnitTest" goto FinUnitTest
rem echo *** %action% %config% ***
win\%config%\atoll_console -j test -l ./data_unittest/_atoll.log
:FinUnitTest

if not "%action%"=="Install" goto FinInstall
echo *** %action% %config% ***
if not "%config%"=="Release" goto FinInstall
vcbuild /nologo win\atoll.sln "%config%|win32"
if %ERRORLEVEL% EQU 0 goto FinInstallBuildLog
start file:///d:/work/dev/atoll/win/atoll_engine/%config%/BuildLog.htm
goto Error
:FinInstallBuildLog
echo *** Install engine Php / Apache ***
rem set RepPhp=%RepSoft%\php
%RepApache%\bin\httpd.exe -w -n "Apache2.2" -k stop
rem copy win\%config%\atoll_*.dll %RepPhp%
copy win\%config%\atoll_*.dll %RepApache%\bin
%RepApache%\bin\httpd.exe -w -n "Apache2.2" -k start
echo The Apache2 service is starting
:FinInstall
goto Fin

:Usage
echo make [Build UnitTest or Install] [Release or Debug]
pause
goto Fin

:Error
echo *** Erreur ***
pause
goto Fin

:Fin
