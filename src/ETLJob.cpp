#include "ETLJob.h"

#include <atomic>

ETLJob::ETLJob()
{
    supports_options_.set_option_info("parallel", "Number of loading threads. odb uses:\n"
                                        "- one thread to read from the input file and\n"
                                        "-as many threads as the parallel argument to write via ODBC.\n"
                                        "This option is database independent.", true, false, true, "1");

    supports_options_.set_option_info("rows", "This defines the size of the I/O buffer for each loading thread.\n"
                                        "You can define the size of this buffer in two different ways :\n"
                                        "1. number of rows(for example: rows = 100 means 100 rows as IO buffer)\n"
                                        "2.* buffer size in kB or MB(for example: rows = k512(512 kB buffer) or rows = m20(20MB buffer))", true, false, true, "100");

    supports_options_.set_option_info("max", "The max number of records to load. Default is to load all records in the input file", true, false, true, "-1");
    supports_options_.set_option_info("Istats", "Seconds of monitor stats interval.", true, false, true, "1");
    supports_options_.set_option_info("pseudo", "To test producer speed, consumer not really consum.");
}

ETLJob::~ETLJob()
{

}

std::string ETLJob::name() const
{
    return std::string("ETL Job");
}

void ETLJob::init(const Options & runer_option, std::string command)
{
    Job::init(runer_option, command);

    // parallel
    std::istringstream iss{ options_.option_value("parallel") };
    std::string temp;
    std::getline(iss, temp, '/');
    parallel_producer_num_ = std::stoul(temp);
    if (std::getline(iss, temp)) {
        parallel_consumer_num_ = std::stoul(temp);
    }
    else {
        parallel_consumer_num_ = parallel_producer_num_;
    }

    consum_totals_.resize(parallel_consumer_num_);
    setup_monitor();

    // rows
    rows_ = std::stoul(options_.option_value("rows"));
    max_ = std::stoull(options_.option_value("max"));

    pseudo_ = options_.option_value("pseudo") == "true";
    show_ = options_.option_value("show") == "true";
}

/*************************************************************
* Function Name: run
* Description: create parallel_provider_num producer and parallel_consumer_num
* consumer. Then run producer and consumer to task done.
**************************************************************/
void ETLJob::run()
{
    std::cout << "*** run " << name() << " ***" << std::endl;

    size_t produced_total = 0;

    std::cout << "produce and consume data till " << max_ << " row processed or exception happen\n";
    std::vector<std::thread> producer_threads;
    std::vector<std::thread> consumer_threads;

    size_t average = max_ / parallel_producer_num_;
    for (size_t i = 0; i < parallel_producer_num_; ++i) {
        produce_maxs_.push_back(average);
    }

    size_t left = max_ % parallel_producer_num_;
    for (size_t i = 0; i < left; ++i) {
        produce_maxs_[i] += 1;
    }

    initialize_producer_buffer();

    for (std::atomic_size_t i{0}; i < parallel_producer_num_; ++i) {
        producer_threads.push_back(std::thread{ [this](size_t id, size_t thread_max) {
            this->run_producer(id, thread_max);
                                                }, i.load(), produce_maxs_[i.load()] });
    }

    for (std::atomic_size_t i{0}; i < parallel_consumer_num_; ++i) {
        consumer_threads.push_back(std::thread{ [this](size_t id) {
            this->run_consumer(id);
                                                }, i.load() });
    }

    monitor_ptr_->start();
    for (auto& t : producer_threads) t.join();
    for (auto& t : consumer_threads) t.join();
    monitor_ptr_->stop();
}

/*
 *    
 */
void ETLJob::run_producer(size_t id, size_t thread_max_rows)
{
    {
        ProducerConter counter;
        std::unique_ptr<DataProducer> producer{ create_producer(id) };

        size_t produced_rows = 0;

        while (produced_rows < thread_max_rows) {
            DataBuffer data;
            {
                std::unique_lock<std::mutex> lck{ mutex_producer_buffer_ };

                cond_producer_buffer_not_empty_.wait(lck, [&] { return !producers_data_buffer_.empty() || is_job_abort; });

                if (is_job_abort) break;

                data = std::move(producers_data_buffer_.front());
                producers_data_buffer_.pop();
            }

            size_t left_rows = thread_max_rows - produced_rows;
            size_t plan_produce_rows = left_rows > rows_ ? rows_ : left_rows;

            data.set_row_count(plan_produce_rows);

            data = producer->produce_data(std::move(data));

            if (!data.row_count()) break;

            produced_rows += data.row_count();

            {
                std::unique_lock<std::mutex> lck{ mutex_consumer_buffer_ };
                consumers_data_buffer_.push(std::move(data));
                cond_consumer_buffer_not_empty.notify_one();
            }
        }

        console().log<ConsoleLog::LDEBUG>("producer ", id, " produced rows:", produced_rows, "\n");
    }

    if (ProducerConter::number == 0)
        cond_consumer_buffer_not_empty.notify_all();
}

/*
 *    
 */
void ETLJob::run_consumer(size_t id)
{
    std::unique_ptr<DataConsumer> consumer{create_consumer(id)};

    while (true) {
        DataBuffer data;
        {
            std::unique_lock<std::mutex> lck{ mutex_consumer_buffer_ };
            cond_consumer_buffer_not_empty.wait(lck, 
                [&] { return !consumers_data_buffer_.empty() || ProducerConter::number == 0 || is_job_abort; });

            if (consumers_data_buffer_.empty()) break;

            data = std::move(consumers_data_buffer_.front());
            consumers_data_buffer_.pop();
        }

        if (show_)
            console().log<ConsoleLog::LINFO>(data.dump());

        if (!pseudo_)
            data = consumer->consume_data(std::move(data));

        consum_totals_[id] += data.row_count();

        {
            std::unique_lock<std::mutex> lck{ mutex_producer_buffer_ };
            producers_data_buffer_.push(std::move(data));
            cond_producer_buffer_not_empty_.notify_one();
        }
    }

    console().log<ConsoleLog::LDEBUG>("consumer ", id, " exit\n");
}

void ETLJob::setup_monitor()
{
    size_t interval = stoul(options_.option_value("Istats"));

    monitor_ptr_ = std::make_unique<Monitor>([=] {
            size_t last_loaded_total = this->total_loaded_rows_;
            this->total_loaded_rows_ = 0;
            for (size_t i = 0, mx = this->consum_totals_.size(); i < mx; ++i) {
                this->total_loaded_rows_ += consum_totals_[i];
            }
            console().log_line<ConsoleLog::LINFO>("speed in ", interval, " seconds: ",
                (this->total_loaded_rows_ - last_loaded_total) / interval, " rows/s");
        }, 
        std::chrono::seconds(interval));
}

/*
 *    
 */
ETLJob::ProducerConter::ProducerConter()
{
    ++number;
}

/*
 *    
 */
ETLJob::ProducerConter::~ProducerConter()
{
    --number;
}

std::atomic_size_t ETLJob::ProducerConter::number;
