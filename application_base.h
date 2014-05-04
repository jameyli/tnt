/**
 * @file:   application_base.h
 * @author: jameyli <jameyli AT tencent DOT com>
 * @date:   2013-09-19
 * @brief:  进程框架
 *
 * $Revision: 49478 $
 * $LastChangedBy: jameyli $
 * $LastChangedDate: 2013-10-08 11:22:05 +0800 (Tue, 08 Oct 2013) $
 *
 *
 * 试图提供一种快速开发的解决方案
 * 1 快速开发，只需要实现业务逻辑就可以
 * 2 提供统一的运行数据统计与监控
 * 3 尽量独立，不依赖第三方库，如tbus、proto，但要考虑如何方便的融合
 * 4 统一的命令行参数
 * 5 统一的二进制版本维护
 *
 * 2013-09-20
 * 先提供一个可用的版本，接口应该不会再做变更
 *
 *
 * 如何使用？
 * step1:先继承 ApplicationBase 实现相应的 OnXXX 接口
 * 其中纯虚接口OnInit 和 OnLoop 是必须实现的，其他更具需求是否实现
 *
 *   class MyApplication : public ApplicationBase
 *
 * step2: main.cpp 调用
 *
 * 当然可以使用单例
 typedef boost::serialization::singleton<MyApplication> MyApplicationSigleton;

    int main(int argc, char** argv)
    {
        MyApplicationSigleton.Init(argc, argv);
        return MyApplicationSigleton.Run();
    }
 *
 *
 */

#ifndef APPLICATION_BASE_H
#define APPLICATION_BASE_H

#include <string>


namespace tnt
{

class ApplicationBase
{
public:
    ApplicationBase();
    virtual ~ApplicationBase();

protected:
    // 提供给子类实现的接口
    // 初始化, 这里子类只关心配置文件的路径就行了
    virtual int OnInit(const char* conf_file) = 0;
    // 定时进入, 定时时间可配, 默认100MS
    virtual int OnTick(){return 0;}
    // 重载
    virtual int OnReload(){return 0;}

    // 主处理逻辑 例如收消息
    // Proc是主循环每次都会进入
    // 这里不建议一次Proc处理多个消息，甚至包含一个循环
    // 如果一次Proc用时太长，则会导致Tick时间漂移
    // XXX: 这里的返回值<0 表示本次没做任何事情，比如没有收到消息
    // 如果服务比较空闲，则会进入IDLE
    virtual int OnProc() = 0;

    // 进入Idle之前
    virtual int OnIdle(){return 0;}
    // 进程退出之前, 直接kill掉
    virtual int OnExit(){return 0;}
    // 进程停止之前, 使用stop参数
    virtual int OnStop(){return 0;}

public:
    // 提供给调用者的接口
    void Init(int argc, char** argv);
    int Run();

    /**
     * @brief:  设置版本信息
     *
     * @param  major 主版本号
     * @param  minor 次版本号
     * @param  rev Revision号
     * @param  build Build号
     */
    void SetVersion(const char* major, const char* minor, const char* rev, const char* build);

    const std::string VersionStr() const;

    /**
     * @brief:  获取统计数据
     */
    const std::string StatStr() const;


    /**
     * @brief: 总tick数, 结合tick_timer可以精确控制定时
     */
    inline size_t total_tick_count() const {return total_tick_count_;}

    /**
     * @brief:  获取运行的环境
     */
    inline int runtime_env() const {return runtime_env_;}

private:
    void InitSigHandler();
    void MakeDaemon();
    int WritePid();
    pid_t ReadPid();
    void SendSignal(unsigned int sig);

    void Usage() const;
    int GetOpt(int argc, char** argv);

    static void ExitProcessCtrl(int);
    static void OnSigUsr1(int);
    static void OnSigUsr2(int);

    const std::string OptStr() const;

private:
    // 进程控制
    static bool is_init_;
    static bool is_run_;
    static bool is_exit_;
    static bool is_reload_;


    // 统计数据
    size_t total_proc_count_;
    size_t total_tick_count_;
    size_t total_ilde_count_;

    float proc_min_cost_;
    float proc_max_cost_;
    float proc_last_cost_;
    float proc_total_cost_;

    float tick_min_cost_;
    float tick_max_cost_;
    float tick_last_cost_;
    float tick_total_cost_;

    time_t last_tick_ms_;

    // version
    std::string version_major_;
    std::string version_minor_;
    std::string version_rev_;
    std::string version_build_;

    int runtime_env_;

private:
    // 配置

    std::string app_name_;
    std::string conf_file_;
    std::string id_;

    size_t idle_count_;
    time_t idle_sleep_;
    time_t tick_timer_;

    std::string pid_file_;

}; // class ApplicationBase

} // namespace tntlib

#endif //APPLICATION_BASE_H

