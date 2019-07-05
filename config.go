package main

import (
	"errors"
	"io/ioutil"

	yaml "gopkg.in/yaml.v2"
)

type Config struct {
	Name string
	Data map[interface{}]interface{}
}

func (c *Config) GetKeys() (res []string) {
	for key := range c.Data {
		res = append(res, key.(string))
	}
	return
}

func (c *Config) GetSdrs() []string {
	var res []string
	keys := c.GetKeys()
	for _, key := range keys {
		sdr := c.GetValue(key)
		res = append(res, sdr[:]...)
	}
	return res

}
func (c *Config) GetValue(key string) []string {
	var res []string
	data := c.Data[key]
	switch data.(type) {
	case string:
		res = append(res, data.(string))
		return res
	case []interface{}:
		temp_data := data.([]interface{})
		for index, _ := range temp_data {
			res = append(res, temp_data[index].(string))
		}
		return res
	}
	return res
}

func LoadConfig(path string) (map[string]Config, error) {
	configs := make(map[string]Config)
	data, err := ioutil.ReadFile(path)
	if err != nil {
		return configs, err
	}
	mapdata := make(map[string]interface{})
	err = yaml.Unmarshal(data, mapdata)
	if err != nil {
		return configs, err
	}
	if configinter, ok := mapdata["config"]; ok {
		config := configinter.(interface{}).(map[interface{}]interface{})
		for key := range config {
			configs[key.(string)] = Config{Name: key.(string), Data: config[key].(map[interface{}]interface{})}
		}
		return configs, nil

	}
	return configs, errors.New("yaml format error")

}
