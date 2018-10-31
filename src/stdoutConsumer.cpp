#include "stdoutConsumer.h"

stdoutConsumer::stdoutConsumer(Options & options)
    :FileConsumer(options)
{
    field_sep = options.option_value("fs")[0];
    rec_sep = options.option_value("rs")[0];
}

DataBuffer stdoutConsumer::consume_data(DataBuffer && data)
{
    for (int r = 0; r < data.row_count(); ++r) {
        for (int c = 0; c < data.column_count() - 1; ++c) {
            console().log<ConsoleLog::LINFO>(data.dump_cell(r, c, false), field_sep);
        }
        console().log<ConsoleLog::LINFO>(data.dump_cell(r, data.column_count() - 1, false), rec_sep);
    }
    return std::move(data);
}
