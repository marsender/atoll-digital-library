@echo off
setlocal

cd ..
win\Debug\atoll_console -v -l ./data_unittest/_atoll.log -x ./data_unittest/xmlexec_test.xml
start ./data_unittest/xmlexec_res.xml
