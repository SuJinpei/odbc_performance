#ifndef DATA_META_H
#define DATA_META_H

#include "DataElementMata.h"

class DataMeta
{
private:
    size_t row_count_;
    size_t capacity_;

    DataElementMata dataElementMata;

public:
    virtual size_t column_count();

    virtual void set_row_count(size_t row_count);
    virtual size_t row_count();

    virtual short column_data_type(size_t column);

    size_t column_length(size_t coumn);

    size_t row_length();

    size_t element_length(size_t row, size_t column);

    void* element_address(size_t row, size_t column);

    void* element_length_address(size_t row, size_t column);

    size_t capacity() const;
};
#endif
