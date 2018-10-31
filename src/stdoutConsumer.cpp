#include "stdoutConsumer.h"

stdoutConsumer::stdoutConsumer(Options & options)
    :FileConsumer(options)
{
    sep = options.option_value("fs")[0];
}

DataBuffer stdoutConsumer::consume_data(DataBuffer && data)
{
    for (int r = 0; r < data.row_count(); ++r) {
        for (int c = 0; c < data.column_count(); ++c) {
            console().log<ConsoleLog::LINFO>(data.dump_cell(r, c, false), sep);
        }
        std::cout << std::endl;
    }
    return std::move(data);
}
