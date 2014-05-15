/**
 * @file:   tnt_random_util.h
 * @author: jameyli <lgy AT live DOT com>
 * @brief:  随机工具类
 */

#ifndef RANDOM_UTIL_H
#define RANDOM_UTIL_H

#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <sys/time.h>

class RandomUtil
{
public:
    // 两次遍历选一个
    static unsigned int WeightedRandomSelect(std::vector<int>& weight_list);

    // 单次遍历选一个
    static unsigned int WeightedRandomSelectOnce(std::vector<int>& weight_list);

    // 带权随机出m(>=1)个
    // 比较笨的办法
    // 一个一个的抽
    static int WeightedRandomSelect(std::vector<int>& weight_list, unsigned int m,
                                    std::vector<unsigned int>& idx_list);



    inline static unsigned int Random(unsigned int high)
    {
        return static_cast<unsigned int>(high * Random());
    }

    inline static unsigned int Random(unsigned int low, unsigned int high)
    {
        // 要不要做判断?
        unsigned int real_low = low;
        unsigned int internal = high - low;
        if (low > high)
        {
            real_low = high;
            internal = low - high;
        }

        return real_low + static_cast<unsigned int>(Random(internal));
    }

    static void Test(unsigned int loop_times, std::vector<int>& weight_list);

private:

    /**
     * @brief:  检查种子
     */
    inline static void CheckSeed()
    {
        if (0 == seed_)
        {
            struct timeval tv;
            gettimeofday(&tv,NULL);
            seed_ =(unsigned)(tv.tv_usec+getpid());
        }
    }


    /**
     * @brief:  随机一个[0, 1) 之间的数
     *
     * @return:
     */
    inline static float Random()
    {
        CheckSeed();

        return rand_r(&seed_)/(RAND_MAX + 1.0);
    }


private:
    static unsigned int seed_;
};

#endif //TNT_RANDOM_H
