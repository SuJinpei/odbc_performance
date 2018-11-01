#include "FileConsumer.h"

FileConsumer::FileConsumer(Options & options)
    :DataConsumer(options), foss{}, oss{ [&]()->std::ostream& {
    if (options.option_value("src") == "stdout") { return std::cout; }
    else { foss.open(options.option_value("src")); return foss; } }() }
{
    field_sep = options.option_value("fs")[0];
    rec_sep = options.option_value("rs")[0];
}

DataBuffer FileConsumer::consume_data(DataBuffer && data)
{
    for (int r = 0; r < data.row_count(); ++r) {
        for (int c = 0; c < data.column_count() - 1; ++c) {
            oss << data.dump_cell(r, c, false) << field_sep;
        }
        oss << data.dump_cell(r, data.column_count() - 1, false) << rec_sep;
    }
    return std::move(data);
}
