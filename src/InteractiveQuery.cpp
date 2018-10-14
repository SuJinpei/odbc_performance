#include "InteractiveQuery.h"


InteractiveQueryJob::InteractiveQueryJob()
{
    supports_options_.set_option_info("-d", "Data_Source_Name", true, false, false);
    supports_options_.set_option_info("-u", "User Name", true, false, false);
    supports_options_.set_option_info("-p", "Password", true, false, false);
}

void InteractiveQueryJob::run()
{
    std::cout << "***run interactive mode***\n";
    options_.print();

    // connection to data base
    std::ostringstream oss;
    oss << "DSN=" << options_.option_value("-d") << ";UID=" << options_.option_value("-u") << ";PWD=" << options_.option_value("-p");
    DBConnection conn(oss.str());

    prompt(std::cout);

    for (std::string line; std::getline(std::cin, line);) {
        if (line != "q") {
            conn.execute_query(line);
        }
        else {
            std::cout << "exit.\n";
            return;
        }
        prompt(std::cout);
    }
}

std::string InteractiveQueryJob::name() const
{
    return std::string("Interactive Shell Mode");
}

std::ostream & InteractiveQueryJob::prompt(std::ostream & os)
{
    os << prompt_;
    return os;
}
