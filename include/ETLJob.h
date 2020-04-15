#ifndef E_T_L_JOB_H
#define E_T_L_JOB_H


#include "Common.h"
#include "Job.h"
#include "Data.h"
#include "DataProducer.h"
#include "DataConsumer.h"
#include "ConsoleLog.h"
#include "Monitor.h"

#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

#ifdef max
#undef max
#endif

class ETLJob : public Job
{
public:
    ETLJob();

    ~ETLJob();

    virtual std::string name() const override;

    void init(const Options& runer_option, std::string command) override;

    void run();

    void run_producer(size_t id, size_t thread_max);
    void run_consumer(size_t id);

protected:
    virtual DataProducer* create_producer(size_t id) = 0;
    virtual DataConsumer* create_consumer(size_t id) = 0;
    virtual void initialize_producer_buffer() = 0;

    void setup_monitor();

    size_t max_;
    size_t rows_;
    size_t parallel_producer_num_;
    size_t parallel_consumer_num_;

    size_t total_loaded_rows_ = 0;

    class ProducerConter {
    public:
        ProducerConter();
        ~ProducerConter();
        static std::atomic_size_t number;
    };

    bool producer_buffer_initialized_ = false;
    bool is_job_abort = false;
    bool pseudo_ = false;
    bool show_ = false;

    std::mutex mutex_producer_buffer_;
    std::condition_variable cond_producer_buffer_not_full;
    std::condition_variable cond_producer_buffer_not_empty_;

    std::mutex mutex_consumer_buffer_;
    std::condition_variable cond_consumer_buffer_not_full;
    std::condition_variable cond_consumer_buffer_not_empty;

    std::queue<DataBuffer> producers_data_buffer_;
    std::queue<DataBuffer> consumers_data_buffer_;
    std::vector<size_t> produce_maxs_;
    std::vector<size_t> consum_totals_;

    std::unique_ptr<Monitor> monitor_ptr_;
};
#endif
