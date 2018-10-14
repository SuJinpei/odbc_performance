#include "Monitor.h"

Monitor::Monitor(std::function<void(void)> stats_fun, std::chrono::seconds interval)
    :stats_fun_(stats_fun), interval_(interval)
{
}

void Monitor::start()
{
    stats_thread_ = std::thread{
        [this] {
            while (true)
            {
                std::unique_lock<std::mutex> lck{ mutex_ };
                cond_.wait_for(lck, interval_, [this] { return this->is_stoped_; });

                if (is_stoped_) break;

                stats_fun_();
            }
        }
    };
}

void Monitor::stop()
{
    is_stoped_ = true;
    cond_.notify_all();
    stats_thread_.join();
}
