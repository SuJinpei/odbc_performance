#ifndef QUERY_JOB_H
#define QUERY_JOB_H

#include "Job.h"
#include "DBConnection.h"

class QueryJob : public Job
{
public:
    QueryJob();

    std::string name() const override;
    void run() override;

    std::map<std::string, std::string> parse_command(std::string&& command) override;
};
#endif

