#!/bin/bash
#
# xmlexec_req.sh
#

# Un argument
if test $# -eq 1; then
	XmlReq=$1
else
	XmlReq=xmlexec_req.xml
	if [ ! -f ./xmlexec_req.xml ]; then echo 'Copy request file'; cp ./xmlexec_req_01.xml ./xmlexec_req.xml; fi
fi

cd ..
atoll_console -l ./data_unittest/_atoll.log -x ./data_unittest/$XmlReq
geany ./data_unittest/xmlexec_res.xml
