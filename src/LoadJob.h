#ifndef LOAD_JOB_H
#define LOAD_JOB_H

#include "ETLJob.h"
#include "DBConnection.h"
#include "MapRules.h"
#include "MapDataProducer.h"
#include "DBConsumer.h"

#include <string>
#include <memory>

class LoadJob : public ETLJob
{
public:

    LoadJob();

    std::string name() const override;

    void init(const Options& runer_option, std::string command) override;

protected:
    // Inherited via ETLJob
    DataProducer * create_producer(size_t id) override;

    DataConsumer * create_consumer(size_t id) override;

    void initialize_producer_buffer() override;

    void initialize_consumer_meta();

    void initialize_producer_meta();

private:
    std::unique_ptr<MapRules> map_rules_ptr_;
    std::vector<DBConnection> db_connections_;
    std::vector<MapWorker> map_workers_;
    DBConsumerMeta consumer_meta_;
    DBConsumerMeta producer_meta_;
};
#endif
