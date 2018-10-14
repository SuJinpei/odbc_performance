#ifndef MONITOR_H
#define MONITOR_H

#include <chrono>
#include <mutex>
#include <condition_variable>
#include <functional>

class Monitor
{
public:
    Monitor(std::function<void(void)> stats_fun, std::chrono::seconds intevals);
    void start();
    void stop();

private:
    bool is_stoped_ = false;
    std::chrono::seconds interval_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::function<void(void)> stats_fun_;
    std::thread stats_thread_;
};

#endif
