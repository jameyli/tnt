/**
 * @file:   application_base.cpp
 * @author: jameyli <lgy AT live DOT com>
 * @date:   2013-09-19
 * @brief:  进程框架
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <string>
#include <sys/time.h>
#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include "ansi_color.h"
#include "code_inbox.h"
#include "application_base.h"

using namespace tnt;

// 常量定义
static const unsigned int APP_DEFAULT_IDLE_COUNT = 10;
static const unsigned int APP_DEFAULT_IDLE_SLEEP = 10 /*MS*/;
static const unsigned int APP_DEFAULT_TICK_TIMER = 100 /*MS*/;

static const char* APP_CMD_START = "start";
static const char* APP_CMD_STOP = "stop";
static const char* APP_CMD_KILL = "kill";
static const char* APP_CMD_RELOAD = "reload";

bool ApplicationBase::is_init_ = false;
bool ApplicationBase::is_run_ = false;
bool ApplicationBase::is_exit_ = false;
bool ApplicationBase::is_reload_ = false;

ApplicationBase::ApplicationBase()
{
    total_proc_count_ = 0;
    total_tick_count_ = 0;
    total_ilde_count_ = 0;

    proc_min_cost_ = 100000;
    proc_max_cost_ = 0;
    proc_last_cost_ = 0;
    proc_total_cost_ = 0;

    tick_min_cost_ = 100000;
    tick_max_cost_ = 0;
    tick_last_cost_ = 0;
    tick_total_cost_ = 0;

    last_tick_ms_ = 0;

    runtime_env_ = 0;

    // 控制逻辑初始化
    idle_count_ = APP_DEFAULT_IDLE_COUNT;
    idle_sleep_= APP_DEFAULT_IDLE_SLEEP;
    tick_timer_ = APP_DEFAULT_TICK_TIMER;
}

ApplicationBase::~ApplicationBase()
{
}

// 提供给调用者的接口
void
ApplicationBase::Init(int argc, char** argv)
{
    // 解析命令行
    GetOpt(argc, argv);

    // 信号处理
    InitSigHandler();


    // 这里是防止起两个相同的进程, 如果之前的进程存在，就直接kill掉
    SendSignal(SIGQUIT);

    WritePid();

    std::cout << COLOR_FG_YELLOW << OptStr() << COLOR_RESET << std::endl;

    int ret = OnInit(conf_file_.c_str());
    if (0 != ret)
    {
        std::cout << COLOR_FG_RED << "App Init Error, OnInit() ret = "<< ret << COLOR_RESET << std::endl;
        exit(-1);
    }

    std::cout << COLOR_FG_GREEN << app_name_ << " Init Success. \"Run! Forrest! Run!!!\""<< COLOR_RESET << std::endl;
}

int
ApplicationBase::Run()
{
    is_run_ = true;

    size_t ilde_count = 0;
    int ret = 0;
    while (is_run_)
    {
        if (is_exit_)
        {
            OnStop();
            OnExit();
            is_run_ = false;
            continue;
        }

        if (is_reload_)
        {
            OnReload();
            is_reload_ = false;
            continue;
        }

        ///////////////////////////////////////////////////////////////////////
        struct timeval tv_start;
        struct timeval tv_end;
        struct timeval tv_diff;
        float time_cost;

        gettimeofday(&tv_start, NULL);
        ret = OnProc();
        gettimeofday(&tv_end, NULL);
        ++total_proc_count_;

        tv_diff = TV_DIFF(tv_end, tv_start);
        time_cost = tv_diff.tv_sec*1000 + tv_diff.tv_usec/1000.0;

        proc_last_cost_ = time_cost;

        if (time_cost < proc_min_cost_)
        {
            proc_min_cost_ = time_cost;
        }

        if (time_cost > proc_max_cost_)
        {
            proc_max_cost_ = time_cost;
        }

        proc_total_cost_ += time_cost;
        ///////////////////////////////////////////////////////////////////////

        if (ret < 0)
        {
            ++ilde_count;
            ++total_ilde_count_;
        }
        else
        {
            ilde_count = 0;
        }

        ///////////////////////////////////////////////////////////////////////
        struct timeval tv_now;
        time_t ms_now;
        gettimeofday(&tv_now, NULL);
        ms_now = TV_TO_MS(tv_now);
        if (last_tick_ms_ + tick_timer_ <= ms_now)
        {
            OnTick();
            gettimeofday(&tv_end, NULL);

            ++total_tick_count_;

            last_tick_ms_ = ms_now;


            tv_diff = TV_DIFF(tv_end, tv_now);
            time_cost = tv_diff.tv_sec*1000 + tv_diff.tv_usec/1000.0;

            tick_last_cost_ = time_cost;

            if (time_cost < tick_min_cost_)
            {
                tick_min_cost_ = time_cost;
            }

            if (time_cost > tick_max_cost_)
            {
                tick_max_cost_ = time_cost;
            }

            tick_total_cost_ += time_cost;
        }

        ///////////////////////////////////////////////////////////////////////
        if (ilde_count >= idle_count_)
        {
            OnIdle();

            ilde_count = 0;
            usleep(idle_sleep_ * 1000);
        }
    };

    return ret;
}

void ApplicationBase::Usage() const
{
	printf("Common Usage:\n");
	printf("  %s%s%s [options] %s<%s|%s|%s|%s>%s\n",
           COLOR_FG_GREEN,  app_name_.c_str(), COLOR_FG_YELLOW,
           COLOR_UL_YELLOW,
           APP_CMD_START, APP_CMD_STOP, APP_CMD_RELOAD, APP_CMD_KILL,
           COLOR_RESET);

	printf("\noptions:\n");
	printf("  %s--id=[name]:%s\n", COLOR_FG_GREEN, COLOR_RESET);
	printf("      the identifier of this process.\n");
	printf("  %s--tick_timer=[millisec]:%s\n", COLOR_FG_GREEN, COLOR_RESET);
	printf("      the tick timer period, default %d ms.\n", APP_DEFAULT_TICK_TIMER);
	printf("  %s--wait=[millisec]:%s\n", COLOR_FG_GREEN, COLOR_RESET);
	printf("      the milliseconds to wait the previous process to exit, default %d s.\n", 0);
	printf("  %s--epoll_wait=[millisec]:%s\n", COLOR_FG_GREEN, COLOR_RESET);
	printf("      the milliseconds for epall_wait to timeout, default %d ms.\n", 0);
	printf("  %s--idle_sleep=[millisec]:%s\n", COLOR_FG_GREEN, COLOR_RESET);
	printf("      the milliseconds for for sleep when the process enter idle status, default %d ms.\n", APP_DEFAULT_IDLE_SLEEP);
	printf("  %s--idle_count=[num]:%s\n", COLOR_FG_GREEN, COLOR_RESET);
	printf("      the count for idle cycle to enter idle status, default %d.\n", APP_DEFAULT_IDLE_COUNT);
	printf("  %s--conf_file=[path]:%s\n", COLOR_FG_GREEN, COLOR_RESET);
	printf("      the path of config file for this process.\n");
	printf("  %s--noloadconf:%s\n", COLOR_FG_GREEN, COLOR_RESET);
	printf("      do not load configure file.\n");
	printf("  %s--pid_file=[path]:%s\n", COLOR_FG_GREEN, COLOR_RESET);
	printf("      the path of the file for storing the process\'s pid.\n");
	printf("  %s--log_file=[path]:%s\n", COLOR_FG_GREEN, COLOR_RESET);
	printf("      the path of the file for logging.\n");
	printf("  %s--log_level=[num]:%s\n", COLOR_FG_GREEN, COLOR_RESET);
	printf("      the logging level for this process.\n");

	printf("  %s--runtime_env=[level]:%s\n", COLOR_FG_GREEN, COLOR_RESET);
	printf("      the env of app running. 0 formal; 1 dev; 2 test; 3 informal\n");

	printf("  %s--daemon, -D:%s\n", COLOR_FG_GREEN, COLOR_RESET);
	printf("      whether start this process as a daemon.\n");
	printf("  %s--version, -v:%s\n", COLOR_FG_GREEN, COLOR_RESET);
	printf("      print version information. \n");
	printf("  %s--help, -h:%s\n", COLOR_FG_GREEN, COLOR_RESET);
	printf("      print help information. \n");

    printf("\n");

	return;
}

int ApplicationBase::GetOpt(int argc, char** argv)
{
	app_name_.assign(basename(argv[0]));
    pid_file_ = app_name_ + ".pid";

    if (argc <= 1)
    {
        Usage();
        exit(0);
    }


	bool is_start = false;
	bool is_daemon = false;

	int opt_idx = 0;

	static int opt_char = 0;
    static struct option long_option[] = {
		{"id", 1, &opt_char, 'i'},
		{"conf_file", 1, &opt_char, 'C'},
		{"noloadconf", 0, &opt_char, 'n'},
		{"log_file", 1, &opt_char, 'L'},
		{"log_level", 1, &opt_char, 'l'},

		{"tick_timer", 1, &opt_char, 't'},
		{"wait", 1, &opt_char, 'w'},
		{"epoll_wait", 1, &opt_char, 'e'},
		{"idle_sleep", 1, &opt_char, 's'},
		{"idle_count", 1, &opt_char, 'c'},

		{"runtime_env", 1, &opt_char, 'r'},

		{"daemon", 0, 0, 'D'},
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},

		{0, 0, 0, 0}
	};

	int opt;
    while(-1 != (opt=getopt_long(argc, argv, "Dvh", long_option, &opt_idx)) )
    {
        switch(opt)
        {
           case 0:
               switch(*long_option[opt_idx].flag)
               {
                    case 'i':
                        id_ = optarg;
                        break;

                    case 't':
                        tick_timer_ = strtol(optarg, NULL, 0);
                        break;

                    case 'w':
                        break;

                    case 'e':
                        break;

                    case 'c':
                        idle_count_ = strtol(optarg, NULL, 0);
                        break;

                    case 's':
                        idle_sleep_ = strtol(optarg, NULL, 0);
                        break;

                    case 'C':
                        conf_file_ = optarg;
                        break;

                    case 'r':
                        runtime_env_ = strtol(optarg, NULL, 0);
                        break;
               }
               break;

            case 'D':
                is_daemon =	true;
                break;

            case 'v':
                printf("%s\n", VersionStr().c_str());
                exit(0);

                break;

            case 'h':
            case '?':
            default:
                Usage();

                exit(0);

                break;
        }
    }


	//  process <start/stop/reload/kill>
	for( opt=optind; opt<argc; opt++ )
	{
		if( 0==strcmp(argv[opt], APP_CMD_STOP) )
		{
            SendSignal(SIGQUIT);
			exit(0);
		}
		else if( 0==strcmp(argv[opt], APP_CMD_RELOAD) )
		{
            SendSignal(SIGUSR1);
			exit(0);
		}
		else if( 0==strcmp(argv[opt], APP_CMD_KILL) )
		{
            SendSignal(SIGQUIT);
			exit(0);
		}
		else if( 0==strcmp(argv[opt], APP_CMD_START) )
		{
			is_start = 1;
			break;
		}
	}

	if( !is_start )
	{
        printf(COLOR_FG_RED);
		printf("use \'%s start\' to start the app.\n", app_name_.c_str());
        printf(COLOR_RESET);

		exit(-2);
	}

    if( is_daemon )
    {
        MakeDaemon();
    }

	//  restore the getopt environment.

	optarg	=	NULL;
	optind	=	1;
	optopt	=	'?';

	return 0;
}

void ApplicationBase::InitSigHandler()
{
    struct sigaction sa;
    sigset_t sset;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = ExitProcessCtrl;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    sa.sa_handler = ExitProcessCtrl;
    sigaction(SIGHUP, &sa, NULL);

    sa.sa_handler = OnSigUsr1;
    sigaction(SIGUSR1, &sa, NULL);

    sa.sa_handler = OnSigUsr2;
    sigaction(SIGUSR2, &sa, NULL);


    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    sigemptyset(&sset);
    sigaddset(&sset, SIGSEGV);
    sigaddset(&sset, SIGBUS);
    sigaddset(&sset, SIGABRT);
    sigaddset(&sset, SIGILL);
    sigaddset(&sset, SIGCHLD);
    sigaddset(&sset, SIGFPE);
    sigprocmask(SIG_UNBLOCK, &sset, &sset);
}

void ApplicationBase::MakeDaemon()
{
    pid_t pid;

    pid = fork() ;

    if (pid != 0) exit(0);

    setsid();

    signal(SIGINT,  SIG_IGN);
    signal(SIGHUP,  SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGHUP,  SIG_IGN);

    pid = fork() ;

    if (pid != 0) exit(0);

    umask(0);
}

int ApplicationBase::WritePid()
{
    FILE* fp = fopen(pid_file_.c_str(), "wt");
    if (!fp) return -1;

    fprintf(fp, "%d\n", (int)getpid());
    fflush(fp);
    fclose(fp);

    return 0;
}

pid_t ApplicationBase::ReadPid()
{
    int pid = -1;

    FILE* fp = fopen(pid_file_.c_str(), "rt");
    if (!fp) return -1;

    fscanf(fp, "%d", &pid);

    fclose(fp);

    return (pid_t)pid;
}

void ApplicationBase::SendSignal(unsigned int sig)
{
    pid_t pid = ReadPid();
    if (-1 != pid)
    {
        kill(pid, sig);
    }
}

void ApplicationBase::ExitProcessCtrl(int )
{
    is_exit_ = true;
}

void ApplicationBase::OnSigUsr1(int)
{
    is_reload_ = true;
}

void ApplicationBase::OnSigUsr2(int)
{
}

void ApplicationBase::SetVersion(const char* major, const char* minor, const char* rev, const char* build)
{
    if(major) version_major_ = major;
    if(minor) version_minor_ = minor;
    if(rev) version_rev_ = rev;
    if(build) version_build_ = build;
}

const std::string ApplicationBase::VersionStr() const
{
    std::ostringstream stream;
    stream << "Version: " << version_major_ << "." <<  version_minor_
        << "." <<  version_rev_ << "." << version_build_ << std::endl;

    stream << "Compiled at " << __DATE__ << " " <<  __TIME__ << std::endl;

    stream << "Gcc Version: " << __VERSION__ << std::endl;

    return stream.str();
}

// 统计数据
const std::string ApplicationBase::StatStr() const
{
    std::ostringstream stream;
    stream << "total_proc_count_:" << total_proc_count_ << ";";
    stream << "total_tick_count_:" << total_tick_count_ << ";";
    stream << "total_ilde_count_:" << total_ilde_count_ << ";";
    stream << std::endl;

    stream << "proc_min_cost_:" << proc_min_cost_ << ";";
    stream << "proc_max_cost_:" << proc_max_cost_ << ";";
    stream << "proc_last_cost_:" << proc_last_cost_ << ";";
    stream << "proc_total_cost_:" << proc_total_cost_ << ";";
    if (total_proc_count_ > 0)
    {
        stream << "proc_average_cost_:" << proc_total_cost_/total_proc_count_ << ";";
    }
    stream << std::endl;

    stream << "tick_min_cost_:" << tick_min_cost_ << ";";
    stream << "tick_max_cost_:" << tick_max_cost_ << ";";
    stream << "tick_last_cost_:" << tick_last_cost_ << ";";
    stream << "tick_total_cost_:" << tick_total_cost_ << ";";
    if (total_tick_count_ > 0)
    {
        stream << "tick_average_cost_:" << tick_total_cost_/total_tick_count_ << ";";
    }
    stream << std::endl;

    return stream.str();
}

const std::string ApplicationBase::OptStr() const
{
    std::ostringstream stream;

    stream << std::endl;
    stream << "conf_file = " <<  conf_file_ << std::endl;
    stream << "id = " << id_  << std::endl;

    stream << "idle_count = " << idle_count_ << std::endl;
    stream << "idle_sleep(ms) = " << idle_sleep_  << std::endl;

    stream << "tick_timer(ms) = " << tick_timer_ << std::endl;

    stream << "pid_file = " << pid_file_ << std::endl;

    return stream.str();
}

