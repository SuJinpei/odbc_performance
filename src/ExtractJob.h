#ifndef EXTRACT_JOB_H
#define EXTRACT_JOB_H

#include "ETLJob.h"
#include "DBProducer.h"
#include "DBConsumer.h"
#include "FileConsumer.h"

class ExtractJob : public ETLJob
{
public:

    ExtractJob();

    std::string name() const override;

    void init(const Options& runer_option, std::string command) override;

    SQLSMALLINT sql_type_to_C(SQLSMALLINT sql_type);

protected:
    // Inherited via ETLJob
    virtual DataProducer * create_producer(size_t id) override;

    virtual DataConsumer * create_consumer(size_t id) override;

    void initialize_producer_buffer() override;

    std::string get_select_query() const;

    std::vector<DBConnection> db_connections_;

    DBConsumerMeta producer_meta_;
};
#endif
