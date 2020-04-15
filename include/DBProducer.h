#ifndef D_B_PRODUCER_H
#define D_B_PRODUCER_H

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <assert.h>

#include "DataProducer.h"
#include "DBConnection.h"
#include "DBConsumer.h"

class DBProducer : public DataProducer
{
public:
    DBProducer(const DBProducer&) = delete;
    DBProducer(Options& options, DBConnection& conn, const DBConsumerMeta& meta);

    // Inherited via DataProducer
    virtual DataBuffer produce_data(DataBuffer && data) override;

private:
    size_t last_row_count;
    SQLULEN num_fetched_rows_;
    std::vector<SQLSMALLINT> row_status_;

    DBConnection& db_conn;
};
#endif
