#include "ExtractJob.h"

std::string ExtractJob::name() const
{
    return std::string("Extract Job");
}

DataProducer * ExtractJob::create_producer(size_t id)
{
    return nullptr;
}

DataConsumer * ExtractJob::create_consumer(size_t id)
{
    return nullptr;
}

void ExtractJob::initialize_producer_buffer()
{
    console().log<ConsoleLog::LERROR>("unimplemented extract job initialize producer buffer\n");
}
