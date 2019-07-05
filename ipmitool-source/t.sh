#!/bin/bash
rm /gproduct/src/main/lib2/* -rf
cp lib/.libs/libipmitool.a  /gproduct/src/main/lib2/
cp src/plugins/.libs/libintf.a  /gproduct/src/main/lib2/
cp src/plugins/lanplus/.libs/libintf_lanplus.a  /gproduct/src/main/lib2/
cp src/plugins/lan/.libs/libintf_lan.a  /gproduct/src/main/lib2/
cp src/plugins/open/.libs/libintf_open.a  /gproduct/src/main/lib2/
cp src/plugins/serial/.libs/libintf_serial.a  /gproduct/src/main/lib2/
cp src/plugins/imb/.libs/libintf_imb.a  /gproduct/src/main/lib2/
cp include/ipmitool/ipmi_main.h  /gproduct/src/main/include/ipmitool
cp include/ipmitool/ipmi_sdr.h  /gproduct/src/main/include/ipmitool

