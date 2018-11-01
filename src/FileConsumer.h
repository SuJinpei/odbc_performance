#ifndef FILE_CONSUMER_H
#define FILE_CONSUMER_H

#include "DataConsumer.h"

#include <fstream>

class FileConsumer : public DataConsumer
{
public:
    FileConsumer(Options& options);
    DataBuffer consume_data(DataBuffer&& data) override;

protected:
    char field_sep;
    char rec_sep;

    std::ofstream foss;
    std::ostream& oss;
};
#endif
