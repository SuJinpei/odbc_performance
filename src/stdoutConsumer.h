#ifndef STDOUT_CONSUMER_H
#define STDOUT_CONSUMER_H

#include "FileConsumer.h"
#include "ConsoleLog.h"

class stdoutConsumer : public FileConsumer
{
public:
    stdoutConsumer(Options& options);
    DataBuffer consume_data(DataBuffer&& data) override;

private:
    char field_sep;
    char rec_sep;
};
#endif
