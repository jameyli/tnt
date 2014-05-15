/**
 * @file:   shm_mmap.hpp
 * @author: jameyli <jameyli AT tencent DOT com>
 * @author: jameyli <lgy AT live DOT com>
 * @date:   2013-10-12
 * @brief:  基于MMAP实现共享内存
 */

#ifndef TNT_SHM_MMAP_H
#define TNT_SHM_MMAP_H

#include <sys/mman.h>

namespace tnt
{

class ShmMmap
{
public:
    ShmMmap();
    ~ShmMmap();

public:
    // 打开
    int Open(const char* mmap_file,
             std::size_t shm_size,
             bool if_restore = false,
             int mmap_prot=PROT_READ|PROT_WRITE,
             int mmap_flags=MAP_SHARED);

    // 关闭
    int Close();

    // 同步
    int sync();

    // 删除文件
    int remove();

    // 内存
    void* addr() const;

private:
    // mmap文件名
    std::string shm_name_;
    // 内存大小
    std::size_t shm_size_;
    // 内存地址
    void* shm_addr_;
    // mmap文件描述符
    int mmap_fd_;

}; // class ShmMmap

} // namespace tnt

#endif //TNT_SHM_MMAP_H

