#include "DataConsumer.h"
#include "ConsoleLog.h"


DataConsumer::DataConsumer(Options & options)
    :options_(options)
{
}

DataBuffer DataConsumer::consume_data(DataBuffer&& data)
{
    console().log<ConsoleLog::LDEBUG>("consume data:", data.row_count(), "\n");
    return std::move(data);
}
