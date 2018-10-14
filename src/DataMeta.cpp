#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <assert.h>

#include "DataMeta.h"


size_t DataMeta::column_count()
{
    return 0;
}

void DataMeta::set_row_count(size_t row_count)
{
    row_count_ = row_count;
}

size_t DataMeta::row_count()
{
    return row_count_;
}

short DataMeta::column_data_type(size_t column)
{
    return 0;
}

size_t DataMeta::column_length(size_t coumn)
{
    return 0;
}

size_t DataMeta::row_length()
{
    return 0;
}

size_t DataMeta::element_length(size_t row, size_t column)
{
    return 0;
}

void* DataMeta::element_address(size_t row, size_t column)
{
    return nullptr;
}

void* DataMeta::element_length_address(size_t row, size_t column)
{
    return nullptr;
}

size_t DataMeta::capacity() const
{
    return capacity_;
}
