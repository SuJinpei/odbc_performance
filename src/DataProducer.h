#ifndef DATA_PRODUCER_H
#define DATA_PRODUCER_H

#include "Options.h"
#include "OptionsInfo.h"
#include "ProducerMeta.h"
#include "Data.h"
#include "ConsoleLog.h"

class DataProducer
{
public:
    DataProducer(Options& options);
    virtual DataBuffer produce_data(DataBuffer&& data) = 0;

protected:
    Options& options_;
    // TODO: We can add this in future.
    // OptionsInfo support_options_;
};
#endif
