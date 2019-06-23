package main

/*
#cgo CFLAGS: -I./include
#cgo LDFLAGS:  ./lib2/libintf.a ./lib2/libipmitool.a ./lib2/libintf_lanplus.a ./lib2/libintf_serial.a ./lib2/libintf_open.a ./lib2/libintf_imb.a ./lib2/libintf_lan.a -lm  -lcrypto
#include <ipmitool/ipmitool.h>
#include <stdio.h>

*/
import "C"
import (
	"fmt"
	"unsafe"
)

func init() {

}
func GetString(intf *C.struct_ipmi_intf, sdrname string) string {
	name := C.CString(sdrname)
	data := C.printv(intf, name)
	defer C.free(unsafe.Pointer(data))
	return C.GoString(data)

}
func main() {

	gonameunits := []string{"ipmitool", "-I", "lanplus", "-H", "10.240.212.99", "-U", "USERID", "-P", "PASSW0RD"}
	nameunits := make([]*C.char, len(gonameunits))
	for i, _ := range gonameunits {
		nameunits[i] = C.CString(gonameunits[i])
		defer C.free(unsafe.Pointer(nameunits[i]))
	}
	var intf *C.struct_ipmi_intf
	intf = C.bmc_connect(C.int(len(gonameunits)), (**C.char)(unsafe.Pointer(&nameunits[0])))

	defer C.ipmi_close(intf)
	for {
		fmt.Println(GetString(intf, "Ambient Temp"))
	}
	// C.run(C.int(len(gonameunits)), (**C.char)(unsafe.Pointer(&nameunits[0])))
	// var sensor string = "Ambient Temp"
	// data := C.get_sdr_data(client, (*C.char)(unsafe.Pointer(&sensor)))

	// fmt.Println(data.value)

	// C.print()
	//C.bmc_connect(C.int(len(gonameunits)), (**C.char)(unsafe.Pointer(&nameunits[0])))
}
