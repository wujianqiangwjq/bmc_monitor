#!/bin/bash
rm /gproduct/src/ipmi/lib2/* -rf
cp lib/.libs/libipmitool.a  /gproduct/src/ipmi/lib2/
cp src/plugins/.libs/libintf.a  /gproduct/src/ipmi/lib2/
cp src/plugins/lanplus/.libs/libintf_lanplus.a  /gproduct/src/ipmi/lib2/
cp src/plugins/lan/.libs/libintf_lan.a  /gproduct/src/ipmi/lib2/
cp src/plugins/open/.libs/libintf_open.a  /gproduct/src/ipmi/lib2/
cp src/plugins/serial/.libs/libintf_serial.a  /gproduct/src/ipmi/lib2/
cp src/plugins/imb/.libs/libintf_imb.a  /gproduct/src/ipmi/lib2/
cp include/ipmitool/ipmi_main.h  /gproduct/src/ipmi/include/ipmitool

