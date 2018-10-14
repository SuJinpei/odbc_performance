#include "MapDataProducer.h"

#include <fstream>
#include <cstring>

MapDataProducer::MapDataProducer(Options & options, MapWorker&& map_worker)
    :DataProducer(options), map_worker_(std::move(map_worker))
{
}

DataBuffer MapDataProducer::produce_data(DataBuffer && databuffer)
{
    size_t row_count = databuffer.row_count();
    size_t column_count = databuffer.column_count();

    for (size_t r = 0; r < row_count; ++r) {
        for (size_t c = 0; c < column_count; ++c) {
            std::string temp = map_worker_.map(c);

            if (temp.length() > databuffer.cell_body_capacity(c)) {
                odb_error("contents too long");
            }

            strcpy(databuffer.cell_body_address(r, c), temp.c_str());
            //databuffer.cell_body_length_ref(r, c) = temp.length();
            databuffer.cell_body_length_ref(r, c) = SQL_NTS;
        }
    }

    return std::move(databuffer);
}
