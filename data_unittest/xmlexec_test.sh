#!/bin/bash
#
# xmlexec_test.sh
#

cd ..
atoll_console -l ./data_unittest/_atoll.log -x ./data_unittest/xmlexec_test.xml
geany ./data_unittest/xmlexec_res.xml
