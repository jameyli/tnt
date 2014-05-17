/**
* @file:   config_protobuf.go
* @brief:  读取protobuf的文本配置
* @author: jameyli <lgy AT live DOT com>
*
* @date:   2014-05-15
 */
package tnt

import proto "code.google.com/p/goprotobuf/proto"
import "os"

func GetConfig(config_path string, msg proto.Message) error {
	f, err := os.Open(config_path)
	if err != nil {
		return err
	}

	defer f.Close()

	finfo, err := f.Stat()
	if err != nil {
		return err
	}

	buff := make([]byte, finfo.Size()+16)
	n, err := f.Read(buff)
	if err != nil {
		return err
	}

	str := string(buff[:n])

	return proto.UnmarshalText(str, msg)
}
