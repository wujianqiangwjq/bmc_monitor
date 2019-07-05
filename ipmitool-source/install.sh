#!/bin/bash
rm ../lib2/* -rf
cp lib/.libs/libipmitool.a  ../lib2/
cp src/plugins/.libs/libintf.a  ../lib2/
cp src/plugins/lanplus/.libs/libintf_lanplus.a  ../lib2/
cp src/plugins/lan/.libs/libintf_lan.a  ../lib2/
cp src/plugins/open/.libs/libintf_open.a  ../lib2/
cp src/plugins/serial/.libs/libintf_serial.a  ../lib2/
cp src/plugins/imb/.libs/libintf_imb.a  ../lib2/
