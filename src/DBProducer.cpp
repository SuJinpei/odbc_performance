#include "DBProducer.h"

DBProducer::DBProducer(Options & options, DBConnection & conn)
    :DataProducer(options), db_conn(conn)
{
}

DataBuffer DBProducer::produce_data(DataBuffer && data)
{
    return std::move(data);
}
