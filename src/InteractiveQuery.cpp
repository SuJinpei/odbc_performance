#include "InteractiveQuery.h"
#include "ConsoleLog.h"

#include <iomanip>


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
    conn_.connect(oss.str());

    prompt(std::cout);

    for (std::string line; std::getline(std::cin, line);) {
        std::istringstream iss{ line };
        std::string token;
        iss >> token;

        if (token == "showinfo") {
            while (iss >> token)
                show_table_info(token);
        }
        else if (token == "q" || token == "quit") {
            std::cout << "exit.\n";
            return;
        }
        else {
            conn_.execute_query(line);
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

void InteractiveQueryJob::show_table_info(std::string table_name)
{
    std::ostringstream oss;
    oss << "select * from " << table_name << " where 1=0";

    if (!SQL_SUCCEEDED(SQLExecDirect(conn_.hstmt(), (SQLCHAR*)oss.str().c_str(), SQL_NTS))) {
        conn_.diag_hstmt();
        return;
    }

    auto meta = conn_.get_results_meta();

    size_t row_width = 0;

    console().log_line<ConsoleLog::LINFO>("COLUMN_NAME\t", "TYPE\t", "NULL\t", "SIZE1\t", "SIZE2");
    console().log_line<ConsoleLog::LINFO>("-----------\t", "----\t", "----\t", "-----\t", "-----");
    for (auto& m : meta) {
        row_width += m.column_size;
        console().log_line<ConsoleLog::LINFO>(std::setw(11), m.column_name, "\t", m.data_type, "\t", m.nullable, 
            "\t", m.column_size, "\t", m.display_size);
    }
    console().log_line<ConsoleLog::LINFO>("\ntable row width: ", row_width);

    if (!SQL_SUCCEEDED(SQLFreeStmt(conn_.hstmt(), SQL_CLOSE))) {
        conn_.diag_hstmt();
    }
}
