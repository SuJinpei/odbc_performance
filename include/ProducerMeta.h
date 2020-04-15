#ifndef PRODUCER_META_H
#define PRODUCER_META_H

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <assert.h>

class ProducerMeta
{
public:
    virtual size_t column_count() = 0;

    virtual size_t row_count() = 0;

    virtual short column_data_type(size_t column) = 0;

};
#endif
