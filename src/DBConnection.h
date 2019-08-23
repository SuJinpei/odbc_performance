#ifndef D_B_CONNECTION_H
#define D_B_CONNECTION_H

#ifdef _WIN32
#include <Windows.h>
#endif // WINDOWS_

#include "error.h"
#include "Common.h"
#include "ColumnMeta.h"

#include <sqlext.h>
#include <string>
#include <vector>
#include <iostream>

class DBConnection
{
public:
    DBConnection();
    DBConnection(std::string connect_string);
    DBConnection(std::string dsn, std::string uid, std::string pwd);
    DBConnection(const DBConnection&) = delete;
    DBConnection& operator=(const DBConnection&) = delete;
    DBConnection(DBConnection&& other);
    DBConnection& operator=(DBConnection&& other);

    void connect(std::string connect_string);
    void execute_query(std::string query);
    void execute_direct(std::string query);
    void print_result(std::vector<ColumnMeta>&& results_meta, bool print_header = true);

    SQLHSTMT hstmt();
    SQLHDBC hdbc();
    SQLHENV henv();

    std::vector<ColumnMeta> get_results_meta();

    void diag_henv();
    void diag_hdbc();
    void diag_hstmt();
    void diag_all();

private:
    void diag(SQLHANDLE handle, SQLSMALLINT handle_type);
    bool is_select_query(std::string query);

    SQLHENV henv_ = NULL;
    SQLHDBC hdbc_ = NULL;
    SQLHSTMT hstmt_ = NULL;

    static std::mutex conn_mutex_;
};
#endif
