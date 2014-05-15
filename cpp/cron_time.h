/**
 * @file:   cron_time.h
 * @author: jameyli <lgy AT live DOT com>
 * @brief:  类似crontab的时间配置
 * 实现的比较土... 尤其是初始化
 * 不过初始化只有一次
 */

#ifndef CRON_TIME_H
#define CRON_TIME_H

#include <bitset>
#include <vector>
#include <iostream>
#include <sstream>

class CronTime
{
    // minute 0-59
    static const unsigned int FIRST_MINUTE = 0;
    static const unsigned int LAST_MINUTE = 59;
    static const size_t MINUTE_COUNT = 60;
    // hour 0-23
    static const unsigned int FIRST_HOUR = 0;
    static const unsigned int LAST_HOUR = 23;
    static const size_t HOUR_COUNT = 24;
    // day of month 1-31
    static const unsigned int FIRST_DOM = 1;
    static const unsigned int LAST_DOM = 31;
    static const size_t DOM_COUNT = 31;
    // month 1-12
    static const unsigned int FIRST_MONTH = 1;
    static const unsigned int LAST_MONTH = 12;
    static const size_t MONTH_COUNT = 12;
    // day of week 0-7
    static const unsigned int FIRST_DOW = 0;
    static const unsigned int LAST_DOW = 7;
    static const size_t DOW_COUNT = 8;

    static const unsigned int IS_ACTIVE = 0x01;
    static const unsigned int DOM_STAR = 0x02;
    static const unsigned int DOW_STAR = 0x04;

    typedef std::vector<unsigned int> NumList;
public:
    /**
     * @brief: 初始化
     *
     * @param  str 类似crontab配置的字符串
     *
     *
     * @return: 0 成功 其他失败
     *
     */
    inline int Init(const std::string& str);

    /**
     * @brief: 测试时间是否满足
     * 因为最小单位是分钟，所以调用者要保证不要在一个循环里重复调用
     *
     * @param  t Unix Time
     *
     * @return: true 已设置，false 未设置
     */
    inline bool Test(time_t t) const;

    // 是否激活
    inline bool IsActive() const
    {
        return flag_ & IS_ACTIVE;
    }

    /**
     * @brief:  打印
     *
     * @return:
     */
    inline std::string debug_str() const;

private:
    inline int GetList(const std::string& str, unsigned int low, unsigned int high, NumList& num_list);
    inline int GetRange(const std::string& str, unsigned int low, unsigned int high, NumList& num_list);

private:
    std::bitset<MINUTE_COUNT> minute_;
    std::bitset<HOUR_COUNT> hour_;
    std::bitset<DOM_COUNT> dom_;
    std::bitset<MONTH_COUNT> month_;
    std::bitset<DOW_COUNT> dow_;

    unsigned int flag_;
};

inline int
CronTime::Init(const std::string& str)
{
    minute_.reset();
    hour_.reset();
    dom_.reset();
    month_.reset();
    dow_.reset();
    flag_ = 0;

    NumList num_list;

    size_t begin_pos = 0;
    size_t end_pos = 0;

    // 如果字符串为空
    // 那么就是不用了。。
    if (str.size() <=0)
    {
        return 0;
    }
    else
    {
        begin_pos = str.find_first_not_of(' ');
        if (begin_pos == std::string::npos)
        {
            return 0;
        }
    }


    // minute
    num_list.clear();
    end_pos = str.find(' ', begin_pos);
    std::string sub_str = str.substr(begin_pos, end_pos - begin_pos);
    GetList(sub_str, FIRST_MINUTE, LAST_MINUTE, num_list);
    for (size_t i=0; i<num_list.size(); ++i)
    {
        if (num_list[i] <= LAST_MINUTE)
        {
            minute_[num_list[i] - FIRST_MINUTE] = 1;
        }
    }

    if (end_pos == std::string::npos)
    {
        return -1;
    }


    // hour
    num_list.clear();
    begin_pos = end_pos + 1;
    end_pos = str.find(' ', begin_pos);
//    std::cout << "debug:" << begin_pos << " " << end_pos<<std::endl;
    sub_str = str.substr(begin_pos, end_pos - begin_pos);
    GetList(sub_str, FIRST_HOUR, LAST_HOUR, num_list);
    for (size_t i=0; i<num_list.size(); ++i)
    {
        if (num_list[i] <= LAST_HOUR)
        {
            hour_[num_list[i] - FIRST_HOUR] = 1;
        }
    }

    if (end_pos == std::string::npos)
    {
        return -2;
    }

    // dom
    num_list.clear();
    begin_pos = end_pos + 1;
    if (str[begin_pos] == '*')
    {
        flag_ |= DOM_STAR;
    }

    end_pos = str.find(' ', begin_pos);
//    std::cout << "debug:" << begin_pos << " " << end_pos<<std::endl;
    sub_str = str.substr(begin_pos, end_pos - begin_pos);
    GetList(sub_str, FIRST_DOM, LAST_DOM, num_list);
    for (size_t i=0; i<num_list.size(); ++i)
    {
        if (num_list[i] <= LAST_DOM)
        {
            dom_[num_list[i] - FIRST_DOM] = 1;
        }
    }

    if (end_pos == std::string::npos)
    {
        return -3;
    }

    // month
    num_list.clear();
    begin_pos = end_pos + 1;
    end_pos = str.find(' ', begin_pos);
//    std::cout << "debug:" << begin_pos << " " << end_pos<<std::endl;
    sub_str = str.substr(begin_pos, end_pos - begin_pos);
    GetList(sub_str, FIRST_MONTH, LAST_MONTH, num_list);
    for (size_t i=0; i<num_list.size(); ++i)
    {
        if (num_list[i] <= LAST_MONTH)
        {
            month_[num_list[i] - FIRST_MONTH] = 1;
        }
    }

    if (end_pos == std::string::npos)
    {
        return -4;
    }

    // dow
    num_list.clear();
    begin_pos = end_pos + 1;
    if (str[begin_pos] == '*')
    {
        flag_ |= DOM_STAR;
    }
    end_pos = str.find(' ', begin_pos);
//    std::cout << "debug:" << begin_pos << " " << end_pos<<std::endl;
    sub_str = str.substr(begin_pos, end_pos-begin_pos);
    GetList(sub_str, FIRST_DOW, LAST_DOW, num_list);
    for (size_t i=0; i<num_list.size(); ++i)
    {
        if (num_list[i] <= LAST_DOW)
        {
            dow_[num_list[i] - FIRST_DOW] = 1;
        }
    }

    /*  make sundays equivilent */
    if (dow_.test(FIRST_DOW) || dow_.test(LAST_DOW))
    {
        dow_[FIRST_DOW] = 1;
        dow_[LAST_DOW] = 1;
    }


    // 只有所有的都设置了才能激活
    flag_ |= IS_ACTIVE;

    return 0;
}

inline bool
CronTime::Test(time_t t) const
{
    if (!(flag_ & IS_ACTIVE))
    {
        return false;
    }

    struct tm* tm = localtime(&t);
    int minute = tm->tm_min - FIRST_MINUTE;
    int hour = tm->tm_hour - FIRST_HOUR;
    int dom = tm->tm_mday - FIRST_DOM;
    int month = tm->tm_mon + 1 - FIRST_MONTH;
    int dow = tm->tm_wday - FIRST_DOW;

    /*  the dom/dow situation is odd.  '* * 1,15 * Sun' will run on the
     *  first and fifteenth AND every Sunday;  '* * * * Sun' will run *only*
     *  on Sundays;  '* * 1,15 * *' will run *only* the 1st and 15th.  this
     *  is why we keep 'DOW_STAR' and 'DOM_STAR'.  yes, it's bizarre.
     *  like many bizarre things, it's the standard.
     */
    if (minute_.test(minute)
        && hour_.test(hour)
        && month_.test(month)
        &&( ((flag_ & DOM_STAR) || (flag_ & DOW_STAR))
           ? (dow_.test(dow) && dom_.test(dom))
           : (dow_.test(dow) || dom_.test(dom))
          )
       )
    {
        return true;
    }

    return false;
}

std::string
CronTime::debug_str() const
{
    std::string debug_str;
    debug_str += "minute:" + minute_.to_string() + ";";
    debug_str += "hour:" + hour_.to_string() + ";";
    debug_str += "dom:" + dom_.to_string() + ";";
    debug_str += "month:" + month_.to_string() + ";";
    debug_str += "dow:" + dow_.to_string() + ";";

    std::ostringstream stream;
    stream << "flag:" << flag_ << ";";

    debug_str += stream.str();

    return debug_str;
}

int
CronTime::GetList(const std::string& str, unsigned int low, unsigned int high, NumList& num_list)
{
//    std::cout << "CronTime::GetList string "<<str << std::endl;
    NumList range_result;

    size_t begin_pos = 0;
    size_t end_pos = 0;
    do
    {
        range_result.clear();

        end_pos = str.find(',', begin_pos);
        std::string sub_str = str.substr(begin_pos, end_pos - begin_pos);

        GetRange(sub_str, low, high, range_result);
        for (size_t i=0; i<range_result.size(); ++i)
        {
            num_list.push_back(range_result[i]);
        }

        begin_pos = end_pos + 1;

    }while (end_pos != std::string::npos && begin_pos != std::string::npos);

    return 0;
}

int
CronTime::GetRange(const std::string& str, unsigned int low, unsigned int high, NumList& num_list)
{
//    std::cout << "CronTime::GetRange string "<<str << std::endl;
    if (str.size() <= 0)
    {
        return -1;
    }

    unsigned int begin_num = 0;
    unsigned int end_num = 0;
    unsigned int step_num = 0;
    std::string num_str;
    size_t idx = 0;
    size_t num_str_len = 0;

    if (str[idx] == '*')
    {
        begin_num = low;
        end_num = high;

        ++idx;
    }
    else
    {
        // 是个数字
        size_t num_begin_idx = idx;
        num_str_len = 0;
        for (; idx<str.size(); ++idx)
        {
            if (!isdigit(str[idx]))
            {
                break;
            }
            ++num_str_len;
        }


        num_str = str.substr(num_begin_idx, num_str_len);
        begin_num = atoi(num_str.c_str());
        begin_num = begin_num < low ? low : begin_num;

        if (idx >= str.size() || str[idx] != '-')
        {
            end_num = begin_num;
        }
        else
        {
            // 跳过 "-"
            ++idx;

            // 是个数字
            size_t num_begin_idx = idx;
            num_str_len = 0;
            for (; idx<str.size(); ++idx)
            {
                if (!isdigit(str[idx]))
                {
                    break;
                }
                ++num_str_len;
            }


            num_str = str.substr(num_begin_idx, num_str_len);
            end_num = atoi(num_str.c_str());
            end_num = end_num > high ? high : end_num;
        }
    }

    if (idx < str.size() && str[idx] == '/')
    {
        // 跳过 "/"
        ++idx;

        // 是个数字
        size_t num_begin_idx = idx;
        num_str_len = 0;
        for (; idx<str.size(); ++idx)
        {
            if (!isdigit(str[idx]))
            {
                break;
            }
            ++num_str_len;
        }

        num_str = str.substr(num_begin_idx, num_str_len);
        step_num = atoi(num_str.c_str());
    }
    else
    {
        step_num = 1;
    }

    step_num = step_num <= 0 ? 1 : step_num;

    if (begin_num > end_num)
    {
        std::swap(begin_num, end_num);
    }


    //    std::cout << "CronTime::GetRange " << begin_num << " "<<end_num << " " << step_num<< std::endl;
    for (unsigned int i=begin_num; i<=end_num; i+=step_num)
    {
        num_list.push_back(i);
    }

    return 0;
}


#endif //COMM_DATE_TIME_H

