# bmc_monitor
今天终于验证，使用cgo调用ipmitool库，达到管理服务器的目的
安装步骤:
 1. download
 2. cd  ipmitool-source && ./bootstrap && ./configure
 3. make 可以能会报错，需要换libtool一般我是cp系统下面的libtool,在make
 4. insall.sh 将生成的静态库，放在../lib2中
 编译main:
    go build
 编译client(添加节点):
    cd client && go build badgerc.go
config里面的配在文件放在/etc下
