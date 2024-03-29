package main

import (
	"flag"
	"log"
	"net/http"
	"strconv"

	"os"
	"runtime"
	"sync"
	"time"

	"github.com/gin-gonic/gin"

	"github.com/astaxie/beego/config"
)

var config_p string
var nodeclients []*NodeClient

func GetNodeClient(nodeclients []*NodeClient, name string) *NodeClient {
	for _, value := range nodeclients {
		if value.Node.Host == name {
			return value
		}
	}
	return nil

}

type PowerData struct {
	Node  string `json:"node"`
	State string `json:"state"`
}

func middle(c *gin.Context) {
	username, passowrd, ok := c.Request.BasicAuth()
	if ok && username == "lico" && passowrd == "Passw0rd" {
		c.Next()
	} else {
		c.Abort()
		c.JSON(http.StatusUnauthorized, gin.H{"errid": 2013, "msg": "Invalid backend auth header format"})
	}

}
func PowerHandle(c *gin.Context) {
	var pdata PowerData

	err := c.BindJSON(&pdata)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"msg": err})
	} else {
		node := pdata.Node
		state := pdata.State
		nc := GetNodeClient(nodeclients, node)
		if nc == nil {
			c.JSON(http.StatusOK, gin.H{"status": "failed", "msg": "cannot  get node:" + node})
		} else {
			var rc int
			if state == "on" {
				rc = nc.RunPowerCommand(true)

			} else {
				rc = nc.RunPowerCommand(false)
			}
			if rc == -1 {
				c.JSON(http.StatusOK, gin.H{"status": "failed", "msg": "handle failed"})
			} else {
				c.JSON(http.StatusOK, gin.H{"status": "success"})
			}
		}
	}

}
func StartWeb(addr string) {
	r := gin.Default()
	group := r.Group("/api", middle)
	group.POST("/node/power/state", PowerHandle)
	r.Run(addr)

}
func main() {
	flag.StringVar(&config_p, "c", "/etc/ipmi.ini", "config path")
	flag.Parse()
	cf, err := config.NewConfig("ini", config_p)
	if err != nil {
		log.Fatal(err)
	}

	core, err := cf.GetSection("core")
	if err != nil {
		log.Fatal(err)
	}

	path, err := cf.GetSection("path")
	if err != nil {
		log.Fatal(err)
	}

	influxdb, err := cf.GetSection("influxdb")
	if err != nil {
		log.Fatal(err)
	}

	timeout_str, ok := core["timeout"]
	if !ok {
		timeout_str = "10"
	}
	timeout, err := strconv.Atoi(timeout_str)
	if err != nil {
		log.Fatal(err)
	}

	core_str, ok := core["core"]
	if !ok {
		core_str = "1"
	}
	core_num, err := strconv.Atoi(core_str)
	if err != nil {
		log.Fatal(err)
	}

	gb_str, ok := core["groupbatch"]
	if !ok {
		gb_str = "100"
	}
	gb_int, err := strconv.Atoi(gb_str)
	if err != nil {
		log.Fatal(err)
	}

	plugn_path, ok := path["plugin_path"]
	if !ok {
		log.Fatal(ok)
	}

	store_path, ok := path["store_path"]
	if !ok {
		log.Fatal(ok)
	}

	host, ok := influxdb["host"]
	if !ok {
		log.Fatal(ok)
	}

	db_name, ok := influxdb["db_name"]
	if !ok {
		log.Fatal(ok)
	}
	username, ok := influxdb["username"]
	if !ok {
		log.Fatal(ok)
	}
	password, ok := influxdb["password"]
	if !ok {
		log.Fatal(ok)
	}
	if core_num > 0 && core_num < runtime.NumCPU() {
		runtime.GOMAXPROCS(core_num)
	}

	// get all nodes
	badger := Badger{
		Dir:      store_path,
		ValueDir: store_path,
	}
	err = badger.Init()
	if err != nil {
		log.Fatal(err)
	}
	nodes := badger.GetAll()
	badger.Close()
	//get config plugin
	configs, err := LoadConfig(plugn_path)
	if err != nil {
		log.Fatal(err)
	}

	for index, _ := range nodes {
		node := nodes[index]
		nodeclient := node.GetNodeClient()
		key := node.Type
		config, ok := configs[key]
		if ok {
			nodeclient.Config = config
			nodeclient.SdrsName = config.GetSdrs()

		} else {
			default_config, _ := configs["default"]
			nodeclient.Config = default_config
			nodeclient.SdrsName = default_config.GetSdrs()
		}

		nodeclients = append(nodeclients, nodeclient)
	}

	//group batch

	lens := len(nodeclients)
	num := 1
	if lens%gb_int == 0 {
		num = lens / gb_int

	} else {
		num = lens/gb_int + 1
	}
	var nodegroup [][]*NodeClient

	for i := 1; i <= num; i++ {
		if i*gb_int > lens {
			nodegroup = append(nodegroup, nodeclients[(i-1)*gb_int:])
		} else {
			nodegroup = append(nodegroup, nodeclients[(i-1)*gb_int:i*gb_int])
		}

	}
	//influxdb
	influxdb_instance := DbData{
		URL:       host,
		DbName:    db_name,
		Username:  username,
		Password:  password,
		Precision: "us",
	}
	var tasks []Task
	for index, _ := range nodegroup {
		task := Task{
			Nodes:       nodegroup[index],
			InfluDb:     influxdb_instance,
			Measurement: "nodes_data",
			Time:        time.Duration(timeout),
			Mux:         new(sync.Mutex),
			Interrpt:    make(chan os.Signal, 1),
			Status:      false,
		}
		tasks = append(tasks, task)
	}
	var wg sync.WaitGroup
	wg.Add(len(tasks))
	//init tasks,start
	for index, _ := range tasks {
		go tasks[index].Run(&wg)

	}
	go StartWeb(":4005")
	wg.Wait()
	// for index, _ := range tasks {
	// 	tasks[index].Close()

	// }

}
