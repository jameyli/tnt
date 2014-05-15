#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdio>
#include <string>
#include <assert.h>

#include "code_inbox.h"
#include "shm_mmap.h"

namespace tnt
{

ShmMmap::ShmMmap()
{
    shm_size_ = 0;
    shm_addr_ = NULL;
    mmap_fd_ = -1;
}

ShmMmap::~ShmMmap()
{
    if(NULL != shm_addr_)
    {
        Close();
    }
}

// 打开
int ShmMmap::Open(const char* mmap_file,
         std::size_t shm_size,
         bool if_restore,
         int mmap_prot,
         int mmap_flags)
{
    TNT_ASSERT(NULL != mmap_file);
    TNT_ASSERT(NULL == shm_addr_);
    TNT_ASSERT(-1 == mmap_fd_);

    shm_name_.assign(mmap_file);
    shm_size_ = shm_size;

    // 根据mmap的参数设置file open的参数
    int file_open_flags = O_CREAT|O_RDWR;
    int file_open_mode = 0;

    mmap_fd_ = open(shm_name_.c_str(), file_open_flags, file_open_mode);
    if (-1 == mmap_fd_)
    {
        return -1;
    }


    shm_addr_ = mmap(NULL, shm_size, mmap_prot, mmap_flags, mmap_fd_, 0);
    if (NULL == shm_addr_)
    {
        close(mmap_fd_);
        return -2;
    }

    return 0;
}

// 关闭
int ShmMmap::Close()
{
    munmap(shm_addr_, shm_size_);
    shm_addr_ = NULL;
    shm_size_ = 0;

    close(mmap_fd_);
    mmap_fd_ = -1;
    return 0;
}

// 同步
int ShmMmap::sync()
{
    // 不能阻塞
    return msync(shm_addr_, shm_size_, MS_ASYNC);
}

// 删除文件
int ShmMmap::remove()
{
    return unlink(shm_name_.c_str());
}

// 内存
void* ShmMmap::addr() const
{
    return shm_addr_;
}
}


