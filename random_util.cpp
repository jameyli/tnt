/**
 * @file:   tnt_random_util.cpp
 * @author: jameyli <jameyli AT tencent DOT com>
 * @brief:  随机工具类
 */

#include "tnt_random_util.h"
#include <iostream>

unsigned int RandomUtil::seed_ = 0;

unsigned int RandomUtil::WeightedRandomSelect(std::vector<int>& weight_list)
{
    unsigned int total_weight = 0;
    for(unsigned int i=0; i<weight_list.size(); ++i)
    {
        total_weight += weight_list[i];
    }

    unsigned int r = Random(total_weight);

    unsigned int tmp_weight = total_weight;
    for(unsigned int i=0; i<weight_list.size(); ++i)
    {
        tmp_weight -= weight_list[i];
        if(r >= tmp_weight)
        {
            return i;
        }
    }

    return weight_list.size() - 1;
}

// 单次遍历
unsigned int RandomUtil::WeightedRandomSelectOnce(std::vector<int>& weight_list)
{
    unsigned int ret = 0;

    float total_weight = 0.0;


    for(unsigned int i=0; i<weight_list.size(); ++i)
    {
        int weight = weight_list[i];
        if (weight <= 0)
        {
            continue;
        }


        total_weight += weight;

        float fr = Random();

        if (fr * total_weight < weight)
        {
            ret = i;
        }
    }

    return ret;
}

// 带权随机出m(>=1)个
// 比较笨的办法
// 一个一个的抽
int RandomUtil::WeightedRandomSelect(std::vector<int>& weight_list, unsigned int m,
                                            std::vector<unsigned int>& idx_list)
{
    if (m <= 0)
    {
        return -1;
    }
    else if (m >= weight_list.size())
    {
        // 全选
        for (unsigned int i=0; i<weight_list.size(); ++i)
        {
            idx_list.push_back(i);
        }
    }

    std::vector<int> weight_list_temp(weight_list);
    for (unsigned int i=0; i<m; ++i)
    {
        unsigned int idx = WeightedRandomSelectOnce(weight_list_temp);
        idx_list.push_back(idx);
        weight_list_temp[idx]=0;
    }

//    std::cout << "idx_list begin#################" << std::endl;
//    for (unsigned int i=0; i<idx_list.size();++i)
//    {
//        std::cout << idx_list[i] << std::endl;
//    }
//    std::cout << "idx_list end#################" << std::endl;

    return 0;
}


void RandomUtil::Test(unsigned int loop_times, std::vector<int>& weight_list)
{
    unsigned int total_weight = 0;
    for(unsigned int i=0; i<weight_list.size(); ++i)
    {
        total_weight+=weight_list[i];
    }
//
//    std::vector<unsigned int> times_list1(weight_list.size(), 0);
//    std::vector<unsigned int> times_list2(weight_list.size(), 0);
//
//    struct timeval tv;
//    gettimeofday(&tv,NULL);
//
//    std::cout<<"开始时间"<<":\t" <<tv.tv_sec<<"."<< tv.tv_usec<< std::endl;
//
//    for(unsigned int i=0; i<loop_times; ++i)
//    {
//        unsigned int idx = WeightedRandomSelect(weight_list);
//        times_list1[idx]++;
//    }
//
//    gettimeofday(&tv,NULL);
//    std::cout<<"算法1结束"<<":\t" <<tv.tv_sec<<"."<< tv.tv_usec<< std::endl;
//
//    for(unsigned int i=0; i<loop_times; ++i)
//    {
//        unsigned int idx = WeightedRandomSelectOnce(weight_list);
//        times_list2[idx]++;
//    }
//
//    gettimeofday(&tv,NULL);
//    std::cout<<"算法2结束"<<":\t" <<tv.tv_sec <<"."<< tv.tv_usec<< std::endl;
//
//    std::cout<<"序号:\t权重\t权重占比\t算法1命中\t命中占比\t算法2命中\t命中占比"<<std::endl;
//
//    for(unsigned int i=0; i<weight_list.size(); ++i)
//    {
//        std::cout<<i<<":\t" << weight_list[i] << "\t" << float(weight_list[i])/(float)(total_weight);
//        std::cout<<"\t" <<times_list1[i] << "\t\t"<<float(times_list1[i])/(float)(loop_times);
//        std::cout<<"\t" <<times_list2[i] << "\t\t"<<float(times_list2[i])/(float)(loop_times);
//        std::cout<<std::endl;
//    }


    std::vector<unsigned int> times_list3(weight_list.size(), 0);
    for(unsigned int i=0; i<loop_times; ++i)
    {
        std::vector<unsigned int> idx_list;
        WeightedRandomSelect(weight_list, 3, idx_list);
        for (unsigned int j = 0; j<idx_list.size(); ++j)
        {
            times_list3[idx_list[j]]++;
        }
    }

    std::cout<<"序号:\t权重\t权重占比\t算法命中\t命中占比"<<std::endl;
    for(unsigned int i=0; i<weight_list.size(); ++i)
    {
        std::cout<<i<<":\t" << weight_list[i] << "\t" << float(weight_list[i])/(float)(total_weight);
        std::cout<<"\t" <<times_list3[i] << "\t\t"<<float(times_list3[i])/(float)(loop_times);
        std::cout<<std::endl;
    }

    return;
}

//int main(int argc, char* argv[])
//{
//   if (argc < 3)
//   {
//       printf("Usage:%s loop_times, weight1, weight2, ... weightn\n", argv[0]);
//       return -1;
//   }
//
//   unsigned int loop_times = atoi(argv[1]);
//
//   std::vector<int> weight_list;
//
//   for (int i=2; i<argc; ++i)
//   {
//       weight_list.push_back(atoi(argv[i]));
//   }
//
//   RandomUtil::Test(loop_times, weight_list);
//}
//
