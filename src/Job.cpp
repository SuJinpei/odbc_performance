#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <assert.h>

#include "Job.h"


Job::~Job()
{
}

void Job::init(const Options & runner_options, std::string command)
{
    // fill options_
    for (auto& cmd_opt : parse_command(std::move(command))) {
        if (supports_options_.contains_option(cmd_opt.first)) {
            if (supports_options_.option(cmd_opt.first).is_value_option)
                options_.set_option(cmd_opt.first, cmd_opt.second);
            else
                options_.set_option(cmd_opt.first, "true");
        }
        else {
            odb_error("Bad command for ", name(), ":", command, ", ", cmd_opt.first, " is not a valid option");
        }
    }

    // initialize uninitialized option
    for (const auto opt : supports_options_.options_info()) {
        if (!options_.has_option(opt.first)) {
            if (runner_options.has_option(opt.first)) {
                options_.add_option(opt.first, runner_options.option_value(opt.first));
            }
            else if (opt.second.is_optional) {
                options_.add_option(opt.first, opt.second.default_value);
            }
            else {
                odb_error("Bad command for ", name(), ", require option '", opt.first, "'");
            }
        }
    }
}

OptionsInfo Job::supports_options() const
{
    return supports_options_;
}

std::map<std::string, std::string> Job::parse_command(std::string && command)
{
    std::map<std::string, std::string> options;

    std::istringstream iss{ command };

    while (iss)
    {
        std::string temp;
        if (std::getline(iss, temp, ':')) {
            std::istringstream issopt{ temp };

            std::string option_name;
            std::string option_value;

            std::getline(issopt, option_name, '=');
            std::getline(issopt, option_value);

            if (!(options.insert({ option_name, option_value })).second) {
                odb_error("duplicate option '", option_name, "'");
            }
        }
    }

    return options;
}
