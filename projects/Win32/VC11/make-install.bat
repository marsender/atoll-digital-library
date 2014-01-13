call "%ProgramFiles(x86)%\NSIS\Makensis" /V2 install-atoll.nsi
copy install.exe ..\..\php\opale-public\install-atoll.exe
call install.exe
