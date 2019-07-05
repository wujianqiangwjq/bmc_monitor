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
	"strconv"
	"strings"
	"unsafe"
)

type Node struct {
	Host     string `json:"host"`
	Addr     string `json:"addr"`
	Username string `json:"username"`
	Passwd   string `json:"passwd"`
	Type     string `json:"type"`
}

type NodeClient struct {
	Node     *Node
	Intf     *C.struct_bmc_client
	Status   bool
	Config   Config
	SdrsName []string
}

func (nc *NodeClient) Connect() {
	username := C.CString(nc.Node.Username)
	ps := C.CString(nc.Node.Passwd)
	addr := C.CString(nc.Node.Addr)
	defer C.free(unsafe.Pointer(username))
	defer C.free(unsafe.Pointer(ps))
	defer C.free(unsafe.Pointer(addr))
	intf := C.bmc_connect(username, ps, addr)
	if intf == nil {
		nc.Status = false
	} else {
		nc.Status = true
		nc.Intf = intf
	}

}

func (nc *NodeClient) Close() {
	if nc.Status {
		C.bmc_close(nc.Intf)
		nc.Status = false
	}
}

func (nc *NodeClient) GetPowerStatus() int {
	//return -1 ,0(off),1(on)
	if nc.Status {
		res := C.bmc_chassis_power_status(nc.Intf)
		return int(res)
	} else {
		return -1
	}

}
func (nc *NodeClient) GetSdrValues() string {
	res := ""
	if nc.Status {
		nameunits := make([]*C.char, len(nc.SdrsName))
		for i, _ := range nc.SdrsName {
			nameunits[i] = C.CString(nc.SdrsName[i])
			defer C.free(unsafe.Pointer(nameunits[i]))
		}
		num := len(nc.SdrsName)
		C.get_sdr(nc.Intf, C.int(num), ((**C.char)(unsafe.Pointer(&nameunits[0]))))

		res = C.GoString(nc.Intf.res)

	}
	return res

}
func (nc *NodeClient) RunPowerCommand(power_on bool) int {
	if nc.Status {
		var res C.int
		if power_on {
			res = C.run_power_on(nc.Intf)

		} else {
			res = C.run_power_off(nc.Intf)
		}
		return int(res)
	} else {
		return -1
	}

}
func (node *Node) GetNodeClient() *NodeClient {

	return &NodeClient{
		Node:   node,
		Status: false,
		Intf:   nil,
	}
}
func (nc *NodeClient) GetData() string {
	data := nc.GetSdrValues()
	if len(data) == 0 {
		return ""
	}
	keyvalues := strings.Split(data, ",")
	resdata := make(map[string]string)
	for _, item := range keyvalues {
		if len(item) > 0 {
			mapdata := strings.Split(item, ":")
			resdata[mapdata[0]] = mapdata[1]
		}
	}
	keys := nc.Config.GetKeys()
	res := ""
	for _, key := range keys {
		if len(res) > 0 {
			res = fmt.Sprintf("%s,%s=", res, key)
		} else {
			res = fmt.Sprintf("%s=", key)
		}
		total_value := nc.Config.GetValue(key)
		value_len := len(total_value)
		if value_len > 1 {
			var r int = 0
			for _, item := range total_value {
				if tem_item, ok := resdata[item]; ok {
					if r_item, err := strconv.Atoi(tem_item); err == nil {
						r += r_item
					}
				}

			}

			res = fmt.Sprintf("%s%d", res, r)
		} else if value_len == 1 {
			v, _ := resdata[total_value[0]]

			res = fmt.Sprintf("%s%s", res, v)
		}

	}
	return res
}
func (nc *NodeClient) GetDefaultValue(default_value int) string {
	data := ""
	keys := nc.Config.GetKeys()
	for _, key := range keys {
		if len(data) > 0 {
			data = fmt.Sprintf("%s,%s=%d", data, key, default_value)
		} else {
			data = fmt.Sprintf("%s=%d", key, default_value)
		}
	}
	return data

}
func init() {

}
