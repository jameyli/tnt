/**
 * @file:   logging.cpp
 * @brief:  统一log接口
 * @author: jameyli <lgy AT live DOT com>
 *
 * @date:   2014-04-16
 */

#include "logging.h"

namespace tnt
{

namespace internal {

void DefaultVaLogHandler(const LogRecord& lr, const char* fmt, va_list vl)
{
    static const char* level_names[] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

    std::fprintf(stderr, "%s|%lu|%lu|%s|%s:%lu(%s)|", level_names[lr.log_level_], lr.log_id_, lr.user_id_, lr.user_name_.c_str(),
                 lr.file_, lr.line_, lr.func_);

    std::vfprintf(stderr, fmt, vl);
    std::fprintf(stderr, "\n");

    std::fflush(stderr);

}

void NullVaLogHandler(const LogRecord& lr, const char* fmt, va_list vl)
{
  // Nothing.
}

static VaLogHandler* va_log_handler_ = &DefaultVaLogHandler;

} // end namespace internal

VaLogHandler* SetVaLogHandler(VaLogHandler* new_func)
{
    VaLogHandler* old = internal::va_log_handler_;
    if (old == &internal::NullVaLogHandler) {
        old = NULL;
    }
    if (new_func == NULL) {
        internal::va_log_handler_ = &internal::NullVaLogHandler;
    } else {
        internal::va_log_handler_ = new_func;
    }
    return old;
}

void Logging(const LogRecord& lr, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    internal::va_log_handler_(lr, fmt, args);
    va_end(args);
}

} // end namespace tnt

