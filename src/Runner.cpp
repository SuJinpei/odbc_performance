#include <string>
#include <sstream>
#include <iostream>

#include "Runner.h"


Runner::Runner(int argc, char * argv[])
{
    initialize_support_options();

    if (argc == 1)
        options_.add_option("-help");
    else
        parse_command(argc, argv);
}

void Runner::run()
{
    options_.print();

    for (auto& job : jobs_) {
        job->run();
    }
}

void Runner::parse_command(int argc, char *argv[])
{
    generate_runner_option_and_job_commands(argc, argv);

    generate_jobs();
}

void Runner::generate_runner_option_and_job_commands(int argc, char *argv[])
{
    // -option [optionvalue]
    bool expects_value = false;
    std::string option_name;
    std::string cmd_str = argv[0];

    for (int i = 1;/* normal break or exception exit */; ++i)
    {
        cmd_str += " ";

        if (i < argc)
            cmd_str += argv[i];

        if (expects_value) {
            if (i == argc || argv[i][0] == '-')
                odb_error("bad command:", cmd_str, "\n", "missing option value after '", argv[i - 1], "'\n");

            if (supported_options_.option(option_name).is_operation)
                execution_commands_.push_back({ option_name, argv[i] });
            else
                options_.add_option(option_name, argv[i]);

            expects_value = false;
        }
        else {
            if (i == argc)
                break;

            if (argv[i][0] != '-')
                odb_error("bad command:", cmd_str, "\n", "expects an option name after '", argv[i - 1], "', but get ", argv[i], "\n");

            option_name = argv[i];

            if (!supported_options_.contains_option(option_name))
                odb_error("bad command:", cmd_str, "\n", "unsupported option '", argv[i], "'\n");

            if (supported_options_.option(option_name).is_value_option)
                expects_value = true;
            else {
                if (supported_options_.option(option_name).is_operation)
                    execution_commands_.push_back({ option_name, "" });
                else
                    options_.add_option(option_name);
            }
        }
    }
}

void Runner::generate_jobs()
{
    for (const auto& cmd : execution_commands_) {
        std::unique_ptr<Job> pjob;
        if (cmd.first == "-x") {
            pjob = std::make_unique<QueryJob>();
        }
        else if (cmd.first == "-I") {
            pjob = std::make_unique<InteractiveQueryJob>();
        }
        else if (cmd.first == "-l") {
            pjob = std::make_unique<LoadJob>();
        }
        else if (cmd.first == "-cp") {
            pjob = std::make_unique<CopyJob>();
        }
        else if (cmd.first == "-e") {
            pjob = std::make_unique<ExtractJob>();
        }
        else {
            odb_warning("TODO: unimplemented option '", cmd.first, "'\n");
        }

        // Expects(pjob.get() != nullptr);

        pjob->init(options_, cmd.second);
        jobs_.push_back(std::move(pjob));
    }
}

void Runner::initialize_support_options()
{
    supported_options_.add_option_info("-h", "print help", false, true);
    supported_options_.add_option_info("-version", "print version and exit", false, true);
    supported_options_.add_option_info("-lsdrv", "list available drivers @ Driver Manager level", false, true);
    supported_options_.add_option_info("-lsdsn", "list available Data Sources", false, true);

    // Connection related options.
    supported_options_.add_option_info("-u", "User: (default $ODB_USER variable)", true);
    supported_options_.add_option_info("-p", "Password: (default $ODB_PWD variable)", true);
    supported_options_.add_option_info("-d", "Data_Source_Name: (default $ODB_DSN variable)", true);
    supported_options_.add_option_info("-ca", "Connection_Attributes (normally used instead of -d DSN)", true);
    supported_options_.add_option_info("-U", "sets SQL_TXN_READ_UNCOMMITTED isolation level", true);
    supported_options_.add_option_info("-ndsn", "-ndsn [+]<number>: adds 1 to <number> to DSN", true);
    supported_options_.add_option_info("-nps", "-nps <nbytes>[:<nbytes>]: specify source[:target] network packet size", true);

    // TODO: add SQL interpreter options
    supported_options_.add_option_info("-I", "interactive mode shell", false, true);
    supported_options_.add_option_info("-v", "be verbose");
    supported_options_.add_option_info("-vv", "be more verbose");

    // TODO: add more execution options [connection required]
    supported_options_.add_option_info("-x", "-x [#inst:]'command': runs #inst (default 1) command instances", true, true);
    supported_options_.add_option_info("-f", "-f [#inst:]'script': runs #inst (default 1) script instances", true, true);

    supported_options_.add_option_info("-l", "Data loading options [connection required]: --help -l for more help.", true, true);
    supported_options_.add_option_info("-e", "Data extraction options [connection required]: --help -e for more help.", true, true);
    supported_options_.add_option_info("-cp", "Data copy options [connection required]: --help -cp for more help.", true, true);
    supported_options_.add_option_info("-pipe", "Data pipe options [connection required]: --help -pipe for more help.", true, true);
    supported_options_.add_option_info("-diff", "Data diff options [connection required]: --help -diff for more help.", true, true);
}
