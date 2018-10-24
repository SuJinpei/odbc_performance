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
            map_worker_.fill_buffer(c, databuffer.cell_address(r, c), databuffer.cell_body_capacity(c));
        }
    }

    return std::move(databuffer);
}
