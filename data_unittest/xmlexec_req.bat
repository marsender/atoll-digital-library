@echo off
setlocal

set XmlReq=%1
if "%XmlReq%"=="" set XmlReq=xmlexec_req.xml

cd ..
win\Debug\atoll_console -v -l ./data_unittest/_atoll.log -x ./data_unittest/%XmlReq%
start ./data_unittest/xmlexec_res.xml
