#ifndef RUNNER_H
#define RUNNER_H

#include "Common.h"
#include "DataProducer.h"
#include "DataConsumer.h"
#include "Data.h"
#include "DataConsummerFatctory.h"
#include "DataProducerFactory.h"
#include "Options.h"
#include "OptionsInfo.h"
#include "Job.h"
#include "QueryJob.h"
#include "InteractiveQuery.h"
#include "LoadJob.h"
#include "CopyJob.h"
#include "ExtractJob.h"
#include <vector>

class Runner
{
public:
    Runner(int argc, char *argv[]);
    void run();

private:
    void parse_command(int argc, char *argv[]);

    void generate_runner_option_and_job_commands(int argc, char *argv[]);

    void generate_jobs();

    void initialize_support_options();

    //TODO: Options supportOptions; consider combine with help function
    Options options_;
    OptionsInfo supported_options_;

    std::vector<std::pair<std::string, std::string>> execution_commands_;
    std::vector<std::unique_ptr<Job>> jobs_;
};
#endif
