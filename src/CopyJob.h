#ifndef COPY_JOB_H
#define COPY_JOB_H

#include "ETLJob.h"
#include "DBProducer.h"
#include "DBConsumer.h"

class CopyJob : public ETLJob
{
public:
    CopyJob();

    std::string name() const override;

    void init(const Options& runer_option, std::string command) override;

    // Inherited via ETLJob
    virtual DataProducer * create_producer(size_t id) override;

    virtual DataConsumer * create_consumer(size_t id) override;

    void initialize_producer_buffer() override;

private:
    void parse_connection_info();
    void create_producer_connections();
    void create_consumer_connections();

    std::string src_dsn;
    std::string src_uid;
    std::string src_pwd;
    std::string tgt_dsn;
    std::string tgt_uid;
    std::string tgt_pwd;

    std::vector<DBConnection> producer_connections_;
    std::vector<DBConnection> consumer_connections_;
    DBConsumerMeta consumer_meta_;
};
#endif
