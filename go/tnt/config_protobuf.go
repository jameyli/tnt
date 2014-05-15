package tnt

import proto "code.google.com/p/goprotobuf/proto"
import "os"

func GetConfig(config_path string, pb proto.Message) error {

	f, err := os.Open(config_path)
	if err != nil {
		// fmt.Printf("failed: %s\n", err)
		return err
	}

	defer f.Close()

	// TODO:: buff 的大小要合理
	buff := make([]byte, 10240)

	n, err := f.Read(buff)

	str := string(buff[:n])

	err = proto.UnmarshalText(str, pb)
	if err != nil {
		return err
	}

	return err
}
