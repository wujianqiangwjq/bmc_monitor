package main

import (
	"fmt"
	"log"
	"os"
	"os/signal"
	"sync"
	"time"
)

func findint(a int, s []int) bool {
	flag := false
	for _, value := range s {
		if value == a {
			flag = true
			break
		}
	}
	return flag

}
func init() {}

type Task struct {
	Nodes       []*NodeClient
	Issues      []int
	InfluDb     DbData
	Time        time.Duration
	Measurement string
	Mux         *sync.Mutex
	Interrpt    chan os.Signal
	Status      bool
}

func (task *Task) Init() {
	log.Println(len(task.Nodes))
	for index, _ := range task.Nodes {
		node := task.Nodes[index]
		node.Connect()
		if !node.Status {
			task.Issues = append(task.Issues, index)
		}
	}
}
func (task *Task) GetInterep() bool {
	select {
	case <-task.Interrpt:
		return true
	default:
		return false
	}

}

func (task *Task) Close() {
	for index, _ := range task.Nodes {
		task.Nodes[index].Close()
	}

}
func (task *Task) Run(wg *sync.WaitGroup) {
	defer wg.Done()
	task.Init()
	log.Println("init end***********")
	log.Println(task.Issues)
	lens := len(task.Nodes)
	var local sync.WaitGroup
	signal.Notify(task.Interrpt, os.Interrupt)
	local.Add(2)
	go func(task *Task) {
		defer local.Done()
		for {
			if task.GetInterep() {
				task.Status = true
				task.Close()
				return
			}
			sds := make([]string, lens)
			for index, node := range task.Nodes {
				data := task.Measurement
				if findint(index, task.Issues) {
					data = fmt.Sprintf("%s,host=%s ", data, node.Node.Host)
					data = fmt.Sprintf("%s%s", data, node.GetDefaultValue(0))
					data = fmt.Sprintf("%s,%s=%d ", data, "status", -1)
					data = fmt.Sprintf("%s %d\n", data, time.Now().UnixNano())
				} else {
					flag_power := node.GetPowerStatus()
					if flag_power == -1 {
						task.Mux.Lock()
						task.Issues = append(task.Issues, index)
						task.Mux.Unlock()
						data = fmt.Sprintf("%s,host=%s ", data, node.Node.Host)
						data = fmt.Sprintf("%s%s", data, node.GetDefaultValue(0))
						data = fmt.Sprintf("%s,%s=%d ", data, "status", -1)
						data = fmt.Sprintf("%s %d\n", data, time.Now().UnixNano())
					} else {
						data = fmt.Sprintf("%s,host=%s ", data, node.Node.Host)
						fields := node.GetData()
						if len(fields) > 0 {
							data = fmt.Sprintf("%s%s", data, fields)
						} else {
							data = fmt.Sprintf("%s%s", data, node.GetDefaultValue(0))
						}
						data = fmt.Sprintf("%s,%s=%d ", data, "status", flag_power)
						data = fmt.Sprintf("%s %d\n", data, time.Now().UnixNano())
					}
				}

				log.Println(data)
				sds = append(sds, data)
			}
			task.InfluDb.WriteAllString(sds...)
			time.Sleep(task.Time * time.Second)
		}
	}(task)
	go func(task *Task) {
		defer local.Done()
		for {
			if task.Status {
				return
			}
			for index, value := range task.Issues {
				node := task.Nodes[value]
				node.Close()
				node.Connect()
				if node.Status {
					task.Mux.Lock()
					nodelens := len(task.Issues)
					if index == (nodelens - 1) {
						task.Issues = task.Issues[:index]
					} else {
						task.Issues = append(task.Issues[:index], task.Issues[index+1:]...)
					}

					task.Mux.Unlock()

				}
			}
			time.Sleep((task.Time + 3) * time.Second)
		}
	}(task)
	local.Wait()
}
