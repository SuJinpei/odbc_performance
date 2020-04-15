#ifndef JOB_H
#define JOB_H

#include "OptionsInfo.h"
#include "Options.h"
#include "error.h"

#include <string>
#include <vector>
#include <iostream>

class Job
{
public:
    virtual ~Job();

    virtual void init(const Options& runer_option, std::string command);

    OptionsInfo supports_options() const;

    virtual std::map<std::string, std::string> parse_command(std::string&& command);

    virtual std::string name() const = 0;

    virtual void run() = 0;

protected:
    Options options_;
    OptionsInfo supports_options_;
};
#endif
