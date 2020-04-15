#ifndef DATA_CONSUMER_H
#define DATA_CONSUMER_H

#include "Options.h"
#include "OptionsInfo.h"
#include "ConsumerMeta.h"
#include "Data.h"

class DataConsumer
{
private:
    std::vector<ColumnMeta> consumerMeta;

public:
    DataConsumer(Options& options);

    virtual DataBuffer consume_data(DataBuffer&& data);

protected:
    Options& options_;
};
#endif
