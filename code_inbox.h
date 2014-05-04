/**
 * @file:   code_inbox.h
 * @author: jameyli <jameyli AT tencent DOT com>
 * @date:   2013-10-12
 * @brief:  代码收件箱
 *
 * 怎么会有这个东西？
 *
 * 偶尔会遇到一段代码可能会比较通用，但又没决定放在哪里的时候就先丢到这个地
 * 方来, 是一个临时的代码收集箱.
 *
 * XXX:
 * 这里的代码可能随时会被迁移到其他更合适的地方, 如果引用到这个头文件的，一定要
 * 注意.
 */

#ifndef TNT_CODE_INBOX_H
#define TNT_CODE_INBOX_H

namespace tnt
{

// 本来感觉 bit_set bit_clr 更合适一些，但是bitset是个数据类型，担心弄混。
// TODO::模板的默认参数
template<typename T>
inline void set_bit(T& value, const std::size_t& bits)
{
    value |= bits;
}

template<typename T>
inline void clr_bit(T& value, const std::size_t& bits)
{
    value &= (~bits);
}


inline bool bit_test_true(std::size_t value, std::size_t bits)
{
    return ((value & bits) != 0);
}

inline bool bit_test_false(std::size_t value, std::size_t bits)
{
    return ((value & bits) == 0);
}

inline struct timeval TV_DIFF(const struct timeval& t1, const struct timeval& t2)
{
    struct timeval t;
    if (t1.tv_usec >= t2.tv_usec)
    {
        t.tv_sec = t1.tv_sec - t2.tv_sec;
        t.tv_usec = t1.tv_usec - t2.tv_usec;
    }
    else
    {
        t.tv_sec = t1.tv_sec - t2.tv_sec - 1;
        t.tv_usec = 1000000 + t1.tv_usec - t2.tv_usec;
    }

    return t;
}

inline time_t TV_TO_MS(const struct timeval& tv)
{
    return tv.tv_sec*1000 + tv.tv_usec/1000;
}

#ifndef TNT_ASSERT

#define TNT_ASSERT(expr)\
    do{\
        assert(expr);\
    }while(false);

#endif

}; // namespace tnt

#endif // TNT_CODE_INBOX_H

