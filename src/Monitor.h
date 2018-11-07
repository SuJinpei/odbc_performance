#ifndef MONITOR_H
#define MONITOR_H

#include <chrono>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>

class Monitor
{
public:
    Monitor(std::function<void(void)> stats_fun, std::chrono::seconds intevals);
    void start();
    void stop();
    std::chrono::seconds duration();

private:
    bool is_stoped_ = false;
    std::chrono::seconds interval_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::function<void(void)> stats_fun_;
    std::thread stats_thread_;

#ifdef  _WIN32
    std::chrono::time_point<std::chrono::steady_clock> ts_;
    std::chrono::time_point<std::chrono::steady_clock> te_;
#else
    std::chrono::time_point<std::chrono::system_clock> ts_;
    std::chrono::time_point<std::chrono::system_clock> te_;
#endif //  _WIN32
};

#endif
