package main

import (
	"encoding/json"
	"errors"
	"fmt"
	"os"

	"github.com/dgraph-io/badger"
)

type Badger struct {
	Dir      string
	ValueDir string
	Db       *badger.DB
}

func (bd *Badger) Init() error {
	var err error
	checkPath(bd.ValueDir)
	opts := badger.DefaultOptions
	opts.Dir = bd.Dir
	opts.ValueDir = bd.ValueDir
	bd.Db, err = badger.Open(opts)
	return err
}

func (bd *Badger) Close() {
	bd.Db.Close()
}

func (bd *Badger) Add(host, addr, username, passwd, ty string) error {
	data := map[string]string{
		"host":     host,
		"addr":     addr,
		"username": username,
		"passwd":   passwd,
		"type":     ty,
	}
	insert_data, err := json.Marshal(data)
	if err != nil {
		return err
	}
	addhandle := bd.Db.NewTransaction(true)
	defer addhandle.Discard()
	if err := addhandle.Set([]byte(host), insert_data); err == nil {
		err = addhandle.Commit()
		return err
	} else {
		return err
	}

}

func (bd *Badger) AddItem(host, addr, username, passwd, ty string) error {
	data := map[string]string{
		"host":     host,
		"addr":     addr,
		"username": username,
		"passwd":   passwd,
		"type":     ty,
	}
	insert_data, err := json.Marshal(data)
	if err != nil {
		return err
	}
	addhandle := bd.Db.NewTransaction(true)
	defer addhandle.Discard()
	if err := addhandle.Set([]byte(host), insert_data); err == nil {
		err = addhandle.Commit()
		return err
	} else {
		return err
	}

}

func (bd *Badger) Delete(host string) error {
	get := bd.Db.NewTransaction(false)
	defer get.Discard()
	if _, err := get.Get([]byte(host)); err == nil {
		delTxn := bd.Db.NewTransaction(true)
		defer delTxn.Discard()
		err := delTxn.Delete([]byte(host))
		if err == nil {
			err = delTxn.Commit()

		}
		return err
	}
	return errors.New("cannot get")

}

func (bd *Badger) GetAll() []Node {
	var nodes []Node
	txn := bd.Db.NewTransaction(false)
	defer txn.Discard()
	iter := badger.DefaultIteratorOptions
	it := txn.NewIterator(iter)
	defer it.Close()
	for it.Rewind(); it.Valid(); it.Next() {
		item := it.Item()
		data, _ := item.ValueCopy(nil)
		var node Node
		err := json.Unmarshal(data, &node)
		if err == nil {
			nodes = append(nodes, node)
		}

	}
	return nodes

}

func (bd *Badger) PrintAll(pas bool) {
	nodes := bd.GetAll()
	fmt.Println("############################################")
	for _, value := range nodes {
		if pas {
			fmt.Printf("Host:%s \t Addr:%s \t Username:%s \t Password:%s \t Type:%s\n",
				value.Host,
				value.Addr,
				value.Username,
				value.Passwd,
				value.Type)
		} else {
			fmt.Printf("Host:%s \t Addr:%s \t Username:%s \t Password:%s \t Type:%s\n",
				value.Host,
				value.Addr,
				value.Username,
				"******",
				value.Type)
		}
		fmt.Println("############################################")
	}
	fmt.Printf("counts: %d\n", len(nodes))
	fmt.Println("############################################")

}

func checkPath(path string) {
	_, err := os.Stat(path)
	if err != nil && os.IsNotExist(err) {
		os.MkdirAll(path, os.ModePerm)
	}

}
func init() {

}
