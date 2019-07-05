package main

import (
	"bytes"
	"errors"
	"fmt"
	"io/ioutil"
	"net/http"
	"net/url"
	"path"

	// "strconv"
	"time"
)

type SendData struct {
	Tags        map[string]string
	Fields      map[string]int
	Measurement string
	Time        time.Time
}

func GetPrecisionMultiplier(precision string) int64 {
	d := time.Nanosecond
	switch precision {
	case "u":
		d = time.Microsecond
	case "ms":
		d = time.Millisecond
	case "s":
		d = time.Second
	case "m":
		d = time.Minute
	case "h":
		d = time.Hour
	}
	return int64(d)
}

func (sd *SendData) String(precision string) string {
	var sdata string
	sdata = fmt.Sprintf("%s", sd.Measurement)
	for key, value := range sd.Tags {
		sdata = sdata + fmt.Sprintf(",%s=%s", key, value)
	}
	sdata = sdata + " "
	flag := true
	for key, value := range sd.Fields {
		if flag {
			sdata = sdata + fmt.Sprintf("%s=%d", key, value)
			flag = !flag
		} else {
			sdata = sdata + fmt.Sprintf(",%s=%d", key, value)
		}

	}
	sdata = sdata + fmt.Sprintf(" %d\n", sd.Time.UnixNano())
	return sdata
}

type DbData struct {
	URL       string
	DbName    string
	Username  string
	Password  string
	Rp        string
	Precision string
}

func (d *DbData) WriteAll(sds ...SendData) error {
	u, err := url.Parse(d.URL)
	if err != nil {
		return err
	}
	u.Path = path.Join(u.Path, "write")
	client := http.Client{}
	var buffer bytes.Buffer
	for _, sdone := range sds {
		buffer.WriteString(sdone.String(d.Precision))
	}
	req, err := http.NewRequest("POST", u.String(), &buffer)
	if err != nil {
		return err
	}
	req.SetBasicAuth(d.Username, d.Password)
	q := req.URL.Query()
	q.Set("db", d.DbName)
	q.Set("precision", d.Precision)
	if d.Rp != "" {
		q.Set("rp", d.Rp)
	}

	req.URL.RawQuery = q.Encode()
	resp, err := client.Do(req)
	if err != nil {
		return err
	}
	if resp.StatusCode == http.StatusNoContent || resp.StatusCode == http.StatusOK {
		return nil
	} else {
		body, err := ioutil.ReadAll(resp.Body)
		if err != nil {
			return err
		} else {
			return errors.New(string(body))
		}
	}

}

func (d *DbData) WriteAllString(sds ...string) error {
	u, err := url.Parse(d.URL)
	if err != nil {
		return err
	}
	u.Path = path.Join(u.Path, "write")
	client := http.Client{}
	var buffer bytes.Buffer
	for _, sdone := range sds {
		buffer.WriteString(sdone)
	}
	req, err := http.NewRequest("POST", u.String(), &buffer)
	if err != nil {
		return err
	}
	req.SetBasicAuth(d.Username, d.Password)
	q := req.URL.Query()
	q.Set("db", d.DbName)
	q.Set("precision", d.Precision)
	if d.Rp != "" {
		q.Set("rp", d.Rp)
	}

	req.URL.RawQuery = q.Encode()
	resp, err := client.Do(req)
	if err != nil {
		return err
	}
	if resp.StatusCode == http.StatusNoContent || resp.StatusCode == http.StatusOK {
		return nil
	} else {
		body, err := ioutil.ReadAll(resp.Body)
		if err != nil {
			return err
		} else {
			return errors.New(string(body))
		}
	}

}

// func main() {
// 	influxdb_instance := DbData{
// 		URL:       "http://10.240.212.100:8086",
// 		DbName:    "lico",
// 		Username:  "lico",
// 		Password:  "123456",
// 		Precision: "us",
// 	}
// 	data := "nodes_data,host=compute power=2,temp=2,status=1  1562322518354730863"
// 	data = fmt.Sprintf("%s\n", data)
// 	influxdb_instance.WriteAllString(data)
// }
