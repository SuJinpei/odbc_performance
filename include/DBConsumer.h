#ifndef D_B_CONSUMER_H
#define D_B_CONSUMER_H

#include "ConsoleLog.h"
#include "DataConsumer.h"
#include "DBConnection.h"
#include "ConsumerMeta.h"

struct DBConsumerMeta {
    size_t buffer_width;
    size_t buffer_rows;
    std::string load_query;
    std::vector<ColumnMeta> col_meta;
};

class DBConsumer : public DataConsumer
{
public:
    DBConsumer(Options& options, DBConnection& conn, const DBConsumerMeta& meta);
    DataBuffer consume_data(DataBuffer&& data) override;

private:
    size_t last_row_count;
    SQLULEN num_processed_rows_;
    std::vector<SQLSMALLINT> row_status_;
    DBConnection& conn_;
    const DBConsumerMeta& meta_;
    SQLHANDLE hstmt_;
};
#endif
