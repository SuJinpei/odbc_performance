#ifndef EXTRACT_JOB_H
#define EXTRACT_JOB_H

#include "ETLJob.h"

class ExtractJob : public ETLJob
{
public:

    std::string name() const override;

protected:
    // Inherited via ETLJob
    virtual DataProducer * create_producer(size_t id) override;

    virtual DataConsumer * create_consumer(size_t id) override;

    void initialize_producer_buffer() override;
};
#endif
