/**
 * @file:   logging.h
 * @brief:  统一log接口
 * @author: jameyli <lgy AT live DOT com>
 * @date:   2014-04-16
 *
 * 本身并不提供日志功能
 * 只是统一日志的格式
 *
 * 可以通过
 * SetVaLogHandler
 * 来改变日志输出
 *
 */
// use like this:
// void MyVaLogHandler(const tnt::LogRecord& lr, const char* fmt, va_list vl)
// {
//     // do something
// }

// int main(int argc, char *argv[])
// {
//     tnt::SetVaLogHandler(MyVaLogHandler);

//     // ...
// }

#ifndef TNT_LOGGING_H
#define TNT_LOGGING_H

#include <cstddef>
#include <cstdio>
#include <cstdarg>
#include <string>

namespace tnt
{

enum LogLevel
{
  LOG_LEVEL_TRACE,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_FATAL,
};

struct LogRecord
{
    LogRecord(LogLevel log_level, std::size_t log_id, std::size_t user_id, const std::string& user_name,
              const char* file, std::size_t line, const char* func) :
        log_level_(log_level),
        log_id_(log_id),
        user_id_(user_id),
        user_name_(user_name),
        file_(file),
        line_(line),
        func_(func)
    {
    }

	LogLevel log_level_;
    std::size_t log_id_;
    std::size_t user_id_;
    std::string user_name_;

    const char* file_;
    std::size_t line_;
    const char* func_;
};

typedef void VaLogHandler(const LogRecord& lr, const char* fmt, va_list vl);

VaLogHandler* SetVaLogHandler(VaLogHandler* new_func);

void Logging(const LogRecord& lr, const char* fmt, ...);


#define LOG(log_level, log_id, user_id, user_name, fmt, args...) \
    do\
    {\
        tnt::LogRecord lr(log_level, log_id, user_id, user_name, __FILE__, __LINE__, __FUNCTION__);\
        tnt::Logging(lr, fmt, ##args);\
    } while(0)

#define LOG_ERROR(log_id, user_id, user_name, fmt, args...)   LOG(tnt::LOG_LEVEL_ERROR, log_id, user_id, user_name, fmt, ##args)
#define LOG_WARN(log_id, user_id, user_name, fmt, args...)    LOG(tnt::LOG_LEVEL_WARN, log_id, user_id, user_name, fmt, ##args)
#define LOG_INFO(log_id, user_id, user_name, fmt, args...)    LOG(tnt::LOG_LEVEL_INFO, log_id, user_id, user_name, fmt, ##args)
#define LOG_DEBUG(log_id, user_id, user_name, fmt, args...)   LOG(tnt::LOG_LEVEL_DEBUG, log_id, user_id, user_name, fmt, ##args)
#define LOG_TRACE(log_id, user_id, user_name, fmt, args...)   LOG(tnt::LOG_LEVEL_TRACE, log_id, user_id, user_name, fmt, ##args)

struct FuncTraceStruct
{
    FuncTraceStruct(const char* file_name, std::size_t file_line, const char* func_name,
                    unsigned int user_id, const std::string& user_name) :
        file_name_(file_name),
        file_line_(file_line),
        func_name_(func_name),
        user_id_(user_id),
        user_name_(user_name)
    {
        LogRecord lr(tnt::LOG_LEVEL_TRACE, 0, user_id_, user_name_, file_name_, file_line_, func_name_);
        tnt::Logging(lr, "Enter Function");
    }

    ~FuncTraceStruct()
    {
        LogRecord lr(tnt::LOG_LEVEL_TRACE, 0, user_id_, user_name_, file_name_, file_line_, func_name_);
        tnt::Logging(lr, "Leave Function");
    }

private:
    const char* file_name_;
    std::size_t file_line_;
    const char* func_name_;
    unsigned int user_id_;
    std::string user_name_;
};

// 在进如函数的时候定义一下就可以打印进出函数的trace日志
// 没有uin 时填0
#ifndef FUNC_TRACE
#define FUNC_TRACE(user_id) \
    tnt::FuncTraceStruct temp_func_trace_struct(__FILE__, __LINE__, __PRETTY_FUNCTION__, user_id, "")
#endif

};

#endif // end of TNT_LOGGING_H

