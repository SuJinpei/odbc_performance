#include "QueryJob.h"

QueryJob::QueryJob()
{
    supports_options_.set_option_info("query", "any sql statement", true, false, false);
    supports_options_.set_option_info("-d", "data source name", true, false, false);
    supports_options_.set_option_info("-u", "connection username", true, false, false);
    supports_options_.set_option_info("-p", "connection password", true, false, false);
}

std::string QueryJob::name() const
{
    return "query job";
}

void QueryJob::run()
{
    std::cout << "*** run query job ***" << std::endl;

    std::ostringstream oss;
    oss << "DSN=" << options_.option_value("-d") << ";UID=" << options_.option_value("-u") << ";PWD=" << options_.option_value("-p");

    DBConnection conn(oss.str());
    conn.execute_query(options_.option_value("query"));
}

std::map<std::string, std::string> QueryJob::parse_command(std::string && command)
{
    std::map<std::string, std::string> ret;
    ret.insert({ "query", command });
    return ret;
}