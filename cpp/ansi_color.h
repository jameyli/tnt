/**
 * @file:   ansi_color.h
 * @author: jameyli <lgy AT live DOT com>
 * @date:   2013-10-02
 * @brief:
 */

#ifndef TNT_ANSI_COLOR_H
#define TNT_ANSI_COLOR_H

#define COLOR_RESET         "\033[0m"

// 前景 文字
#define COLOR_FG_BLACK      "\033[30m"
#define COLOR_FG_RED        "\033[31m"
#define COLOR_FG_GREEN      "\033[32m"
#define COLOR_FG_YELLOW     "\033[33m"
#define COLOR_FG_BLUE       "\033[34m"
#define COLOR_FG_PURPLE     "\033[35m"  // 紫
#define COLOR_FG_CYAN       "\033[36m"  // 青绿、深绿
#define COLOR_FG_WHITE      "\033[37m"

// 背景
#define COLOR_BG_BLACK      "\033[40m"
#define COLOR_BG_RED        "\033[41m"
#define COLOR_BG_GREEN      "\033[42m"
#define COLOR_BG_YELLOW     "\033[43m"
#define COLOR_BG_BLUE       "\033[44m"
#define COLOR_BG_PURPLE     "\033[45m"  // 紫
#define COLOR_BG_CYAN       "\033[46m"  // 青绿、深绿
#define COLOR_BG_WHITE      "\033[47m"

// underline
#define COLOR_UL_BLACK      "\33[4;30m"
#define COLOR_UL_RED        "\33[4;31m"
#define COLOR_UL_GREEN      "\33[4;32m"
#define COLOR_UL_YELLOW     "\33[4;33m"
#define COLOR_UL_BLUE       "\33[4;34m"
#define COLOR_UL_PURPLE     "\33[4;35m"
#define COLOR_UL_CYAN       "\33[4;36m"
#define COLOR_UL_WHITE      "\33[4;37m"

#endif // TNT_ANSI_COLOR_H

