#ifndef MAP_DATA_PRODUCER_H
#define MAP_DATA_PRODUCER_H

#include "DataProducer.h"
#include "MapRules.h"

#include <sqlext.h>
#include <vector>


struct MapMeta {
    DataType type;
    size_t size;
};

class MapDataProducer : public DataProducer
{
public:
    MapDataProducer(Options& options, MapWorker&& map_worker);

    DataBuffer produce_data(DataBuffer&& d) override;

    MapWorker map_worker_;
};
#endif
