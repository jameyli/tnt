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

	// TODO:: buff 的大小要合理
	buff := make([]byte, 10240)

	n, err := f.Read(buff)

	str := string(buff[:n])

	err = proto.UnmarshalText(str, msg)

	return err
}
