#include "DBConnection.h"

#include <iomanip>
#include <cctype>

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32


DBConnection::DBConnection()
{
}

DBConnection::DBConnection(std::string connect_string)
{
    connect(connect_string);
}

DBConnection::DBConnection(std::string dsn, std::string uid, std::string pwd)
{
    std::ostringstream conn_oss;
    conn_oss << "DSN=" << dsn << ";UID=" << uid << ";PWD=" << pwd;
    connect(conn_oss.str());
}

DBConnection::DBConnection(DBConnection && other)
{
    *this = std::move(other);
}

DBConnection & DBConnection::operator=(DBConnection && other)
{
    std::swap(this->henv_, other.henv_);
    std::swap(this->hdbc_, other.hdbc_);
    std::swap(this->hstmt_, other.hstmt_);
    return *this;
}

void DBConnection::connect(std::string connect_string) {
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv_))) {
        odb_error("Alloc ODBC env failed");
    }
    
    if (!SQL_SUCCEEDED(SQLSetEnvAttr(henv_, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))) {
        diag_henv();
    }

    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_DBC, henv_, &hdbc_))) {
        diag_henv();
    }

    char outstr[1024];
    SQLSMALLINT len;
    if (!SQL_SUCCEEDED(SQLDriverConnect(hdbc_, NULL, (SQLCHAR*)connect_string.c_str(), SQL_NTS, (SQLCHAR*)outstr, sizeof(outstr), &len, SQL_DRIVER_NOPROMPT))) {
        diag_henv();
        diag_hdbc();
    }

    // TODO: oracle not support this feature.
    //if (!SQL_SUCCEEDED(SQLSetConnectAttr(hdbc_, SQL_ATTR_CONNECTION_TIMEOUT, (SQLPOINTER)600, 0))) {
    //    diag_hdbc();
    //    diag_henv();
    //}

    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, hdbc_, &hstmt_))) {
        diag_henv();
        diag_hdbc();
    }

}

void DBConnection::execute_query(std::string query)
{
    if (!SQL_SUCCEEDED(SQLExecDirect(hstmt_, (SQLCHAR*)query.c_str(), SQL_NTS))) {
        std::cout << "SQL QUERY ERROR\n";
        diag_all();
    }
    else {
        std::vector<ColumnMeta> results_meta = get_results_meta();

        if (results_meta.size() > 0) {
            print_result(std::move(results_meta), is_select_query(query));
        }

        std::cout << "\n--- SQL operation success!\n";
    }
}

void DBConnection::execute_direct(std::string query)
{
    if (!SQL_SUCCEEDED(SQLExecDirect(hstmt_, (SQLCHAR*)query.c_str(), SQL_NTS))) {
        diag_all();
        odb_error("execute direct \"", query, "\" failed\n");
    }
}

void DBConnection::print_result(std::vector<ColumnMeta>&& results_meta, bool print_header)
{
    size_t line_width = 0;
    size_t max_column_width = 0;
    std::vector<size_t> column_widths(results_meta.size());

    for (size_t i = 0, max = results_meta.size(); i < max; ++i) {
        if (print_header) {
            column_widths[i] = results_meta[i].column_name.length();
        }

        if (max_column_width < results_meta[i].column_size)
            max_column_width = results_meta[i].column_size;
    }

    char *tempbuf = new char[max_column_width];
    SQLLEN ind = 0;

    using Row = std::vector<std::string>;

    std::vector<Row> records;

    // fetch max 100 rows data
    for (size_t r = 0; r < 100 && SQL_SUCCEEDED(SQLFetch(hstmt_)); ++r) {
        Row row;
        for (size_t c = 0, max = results_meta.size(); c < max; ++c) {
            if (!SQL_SUCCEEDED(SQLGetData(hstmt_, static_cast<SQLSMALLINT>(c + 1), SQL_C_CHAR, tempbuf, max_column_width, &ind))) {
                diag_all();
            }

            if (ind == SQL_NULL_DATA) {
                row.push_back("NULL");
            }
            else {
                for (size_t i = ind - 1; i && tempbuf[i] == ' '; --i) tempbuf[i] = '\0';
                row.push_back(tempbuf);
            }

            if (column_widths[c] < row[c].length())
                column_widths[c] = row[c].length();
        }
        records.push_back(row);
    }

    if (print_header) {
        std::string sepline;
        for (size_t c = 0, maxc = results_meta.size(); c < maxc; ++c) {
            std::cout << std::left << std::setw(column_widths[c]) << results_meta[c].column_name << " ";
            sepline.resize(sepline.length() + column_widths[c], '-');
            sepline += " ";
        }
        std::cout << std::endl << sepline << std::endl;
    }

    for (size_t r = 0, maxr = records.size(); r < maxr; ++r) {
        for (size_t c = 0, max = results_meta.size(); c < max; ++c) {
            switch (results_meta[c].data_type)
            {
            case SQL_INTEGER:
            case SQL_NUMERIC:
            case SQL_DOUBLE:
            case SQL_FLOAT:
            case SQL_BIGINT:
                std::cout << std::right;
                break;
            default:
                std::cout << std::left;
                break;
            }

            std::cout << std::setw(column_widths[c]) << records[r][c] << " ";
        }
        std::cout << std::endl;
    }

    if (!SQL_SUCCEEDED(SQLCloseCursor(hstmt_))) {
        diag_all();
    }
}

SQLHSTMT DBConnection::hstmt()
{
    return hstmt_;
}

SQLHDBC DBConnection::hdbc()
{
    return hdbc_;
}

SQLHENV DBConnection::henv()
{
    return henv_;
}

std::vector<ColumnMeta> DBConnection::get_results_meta()
{
    std::vector<ColumnMeta> ret;

    // get columns number
    SQLSMALLINT num_col = 0;
    if (SQL_SUCCEEDED(SQLNumResultCols(hstmt_, &num_col))) {
        // get each columns meta
        for (int c = 1; c <= num_col; ++c) {
            ColumnMeta column_meta;
            SQLSMALLINT column_name_len = 0;

            if (!SQL_SUCCEEDED(SQLDescribeCol(hstmt_, c, NULL, 0, &column_name_len, &column_meta.data_type,
                &column_meta.column_size, &column_meta.decimal_digits, &column_meta.nullable))) {
                diag_all();
            }

            if (!SQL_SUCCEEDED(SQLColAttribute(hstmt_, c, SQL_DESC_BASE_COLUMN_NAME, NULL, 0, &column_name_len, NULL))) {
                diag_all();
            }

            column_meta.column_name.resize(column_name_len + 1);

            if (!SQL_SUCCEEDED(SQLColAttribute(hstmt_, c, SQL_DESC_BASE_COLUMN_NAME, (SQLPOINTER)column_meta.column_name.c_str(),
                (SQLSMALLINT)column_meta.column_name.size(), &column_name_len, NULL))) {

                diag_all();
            }

            if (!SQL_SUCCEEDED(SQLColAttribute(hstmt_, c, SQL_DESC_DISPLAY_SIZE, NULL, 0, NULL, &column_meta.display_size))) {
                diag_all();
            }

            ret.push_back(column_meta);
        }
    }
    else {
        diag_all();
    }

    return ret;
}

void DBConnection::diag_henv()
{
    diag(henv_, SQL_HANDLE_ENV);
    //odb_error("diag error");
}

void DBConnection::diag_hdbc()
{
    diag(hdbc_, SQL_HANDLE_DBC);
    //odb_error("diag error");
}

void DBConnection::diag_hstmt()
{
    diag(hstmt_, SQL_HANDLE_STMT);
    //odb_error("diag error");
}

void DBConnection::diag_all()
{
    diag_hstmt();
    diag_hdbc();
    diag_henv();
}

void DBConnection::diag(SQLHANDLE handle, SQLSMALLINT handle_type)
{
    SQLINTEGER native_error;
    char sql_state[10];
    char error_message[1024];
    SQLSMALLINT indicator = 0;

    int i = 1;
    SQLRETURN ret = SQLGetDiagRec(handle_type, handle, i, (SQLCHAR*)sql_state, &native_error, (SQLCHAR*)error_message, sizeof(error_message), &indicator);

    while (SQL_SUCCEEDED(ret)) {
        std::cerr << "[SQL_STATE]:" << sql_state << std::endl;
        std::cerr << "[NATIVE_ERROR]:" << native_error << std::endl;
        std::cerr << "[ERROR_MESSAGE]:" << error_message << std::endl;

        ++i;
        ret = SQLGetDiagRec(handle_type, handle, i, (SQLCHAR*)sql_state, &native_error, (SQLCHAR*)error_message, sizeof(error_message), &indicator);
    }
}

bool DBConnection::is_select_query(std::string query)
{
    std::string select_query = "select";
    return std::equal(select_query.begin(), select_query.end(), query.begin(),
        [](const char c1, const char c2) { return std::toupper(c1) == std::toupper(c2); });
}
