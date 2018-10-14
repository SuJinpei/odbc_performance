#ifndef DATA_H
#define DATA_H

#include "DataMeta.h"

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

#include <sqlext.h>

using Address = char *;
using DataType = SQLSMALLINT;

struct Meta {
    DataType data_type;
    size_t offset;
    size_t length;
};

class DataBuffer
{
public:
    DataBuffer();
    ~DataBuffer();

    DataBuffer(DataBuffer&& d);
    DataBuffer& operator=(DataBuffer&& d);
    DataBuffer(const DataBuffer&) = delete;
    DataBuffer& operator=(const DataBuffer&) = delete;
    DataBuffer copy();

    void realloc(size_t size);
    void reset_meta();
    void append(DataType type, size_t length);

    size_t row_width();
    size_t row_count();
    size_t column_count();
    void set_row_count(size_t row_count);
    size_t buffer_length();

    SQLLEN& cell_body_length_ref(size_t row, size_t column);
    Address cell_body_address(size_t row, size_t column);
    size_t cell_body_capacity(size_t column);
    DataType cell_data_type(size_t column);

    void adjust_buffer();

    std::string dump();

private:

    bool initialized = false;
    char* buffer_ = nullptr;
    size_t buffer_size_ = 0;
    size_t row_count_ = 0;
    std::vector<Meta> meta_;
};
#endif
