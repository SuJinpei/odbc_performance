#include "Data.h"

#include <sstream>

DataBuffer::DataBuffer()
{
}

DataBuffer::~DataBuffer()
{
    if (buffer_) delete[] buffer_;
}

DataBuffer::DataBuffer(DataBuffer && d)
{
    *(this) = std::move(d);
}

DataBuffer & DataBuffer::operator=(DataBuffer && d)
{
    std::swap(this->buffer_, d.buffer_);
    std::swap(this->buffer_size_, d.buffer_size_);
    std::swap(this->initialized, d.initialized);
    std::swap(this->meta_, d.meta_);
    std::swap(this->row_count_, d.row_count_);
    return *this;
}

DataBuffer DataBuffer::copy()
{
    DataBuffer databuffer;

    databuffer.initialized = this->initialized;
    databuffer.buffer_size_ = this->buffer_size_;
    databuffer.row_count_ = this->row_count_;
    databuffer.meta_ = this->meta_;
    if (this->buffer_)
        databuffer.buffer_ = new char[this->buffer_size_];

    return databuffer;
}

size_t DataBuffer::buffer_length()
{
    return buffer_size_;
}

void DataBuffer::realloc(size_t size)
{
    if (size > buffer_size_) {
        if (buffer_) delete[] buffer_;
        buffer_ = (Address)new char[size];
        buffer_size_ = size;
    }
}

void DataBuffer::reset_meta()
{
    meta_.clear();
    row_count_ = 0;
}

void DataBuffer::append(DataType type, size_t length)
{
    meta_.emplace_back();
    size_t last = meta_.size() - 1;
    meta_[last].data_type = type;

    if (last > 0) {
        meta_[last].offset = meta_[last - 1].offset + meta_[last - 1].length;
    }
    else {
        meta_[last].offset = 0;
    }

    meta_[last].length = length + sizeof(SQLLEN);
}

size_t DataBuffer::row_width()
{
    size_t meta_size = meta_.size();
    if (meta_size > 0) {
        return meta_[meta_size - 1].offset + meta_[meta_size - 1].length;
    }
    return 0;
}

size_t DataBuffer::row_count()
{
    return row_count_;
}

size_t DataBuffer::column_count()
{
    return meta_.size();
}

void DataBuffer::set_row_count(size_t row_count)
{
    row_count_ = row_count;
}

SQLLEN& DataBuffer::cell_body_length_ref(size_t row, size_t column)
{
    return *((SQLLEN *)(buffer_ + row_width() * row + meta_[column].offset));
}

Address DataBuffer::cell_body_address(size_t row, size_t column)
{
    return buffer_ + row * row_width() + meta_[column].offset + sizeof(SQLLEN);
}

size_t DataBuffer::cell_body_capacity(size_t column)
{
    return meta_.at(column).length - sizeof(SQLLEN);
}

DataType DataBuffer::cell_data_type(size_t column)
{
    return meta_.at(column).data_type;
}

void DataBuffer::adjust_buffer()
{
    size_t expects_size = row_count_ * row_width();

    if (expects_size > buffer_size_) {
        realloc(expects_size);
    }

    buffer_size_ = expects_size;
}

std::string DataBuffer::dump()
{
    std::ostringstream oss;
 
    size_t maxc = column_count();
    for (size_t r = 0; r < row_count_; ++r) {
        for (size_t c = 0; c < maxc; ++c) {
            oss << "[" << cell_body_length_ref(r, c) << "]" << "\"" << cell_body_address(r, c) << "\",";
        }
        oss << "\n";
    }

    return oss.str();
}
