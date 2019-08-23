#include "DBConsumer.h"

#include <sstream>
#include <regex>

DBConsumer::DBConsumer(Options& options, DBConnection& conn, const DBConsumerMeta& meta)
    :DataConsumer(options), conn_(conn), meta_(meta)
{
    hstmt_ = conn.hstmt();

    // prepare
    if (!SQL_SUCCEEDED(SQLPrepare(hstmt_, (SQLCHAR*)meta.load_query.c_str(), SQL_NTS))) {
        conn.diag_hstmt();
        odb_error("SQLPrepare \"", meta.load_query, "\"");
    }

    // bind parameters
    if (!SQL_SUCCEEDED(SQLSetStmtAttr(hstmt_, SQL_ATTR_PARAM_BIND_TYPE, (SQLPOINTER)meta.buffer_width, 0))) {
        conn.diag_hstmt();
        odb_error("set SQL_ATTR_PARAM_BIND_TYPE = ", meta.buffer_rows);
    }

    last_row_count = meta.buffer_rows;
    row_status_.resize(last_row_count);

    if (!SQL_SUCCEEDED(SQLSetStmtAttr(hstmt_, SQL_ATTR_PARAMSET_SIZE, (SQLPOINTER)last_row_count, 0))) {
        conn.diag_hstmt();
        odb_error("set SQL_ATTR_PARAMSET_SIZE = ", last_row_count);
    }

    if (!SQL_SUCCEEDED(SQLSetStmtAttr(hstmt_, SQL_ATTR_PARAMS_PROCESSED_PTR, (SQLPOINTER)&num_processed_rows_, 0))) {
        conn.diag_hstmt();
        odb_error("set SQL_ATTR_PARAMS_PROCESSED_PTR");
    }

    if (!SQL_SUCCEEDED(SQLSetStmtAttr(hstmt_, SQL_ATTR_PARAM_STATUS_PTR, (SQLPOINTER)&row_status_[0], 0))) {
        conn.diag_hstmt();
        odb_error("set SQL_ATTR_PARAMS_PROCESSED_PTR");
    }
}

DataBuffer DBConsumer::consume_data(DataBuffer && data)
{
    if (last_row_count != data.row_count()) {
        last_row_count = data.row_count();
        row_status_.resize(last_row_count);
        if (!SQL_SUCCEEDED(SQLSetStmtAttr(hstmt_, SQL_ATTR_PARAMSET_SIZE, (SQLPOINTER)last_row_count, 0))) {
            conn_.diag_hstmt();
            odb_error("set SQL_ATTR_PARAMSET_SIZE = ", data.row_count());
        }
    }

    for (SQLUSMALLINT c = 0, cmax = (SQLUSMALLINT)meta_.col_meta.size(); c < cmax; ++c) {
        if (!SQL_SUCCEEDED(SQLBindParameter(hstmt_, c + 1, SQL_PARAM_INPUT, data.cell_data_type(c), meta_.col_meta.at(c).data_type,
            meta_.col_meta.at(c).column_size, meta_.col_meta.at(c).decimal_digits, (SQLPOINTER)data.cell_body_address(0, c),
            data.cell_body_capacity(c), &data.cell_body_length_ref(0, c)))) {

            conn_.diag_hstmt();
            odb_error("SQLBindParameter col:", meta_.col_meta.at(c).column_name);
        }
    }

    if (data.dump_cell(0, 0) == "0,") {
      std::cerr << "how this happen?\n";
    }

    if (SQLExecute(hstmt_) != SQL_SUCCESS) {
        SQLSMALLINT rec_num = 1, len;
        SQLCHAR state[10];
        SQLINTEGER error;
        SQLCHAR message[1024];
        SQLSMALLINT row_num, col_num;

        SQLRETURN ret = 0;
        while (SQL_SUCCEEDED(ret = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt_, rec_num, state, &error, message, sizeof(message), &len))) {
            // figure out error data
            ret = SQLGetDiagField(SQL_HANDLE_STMT, hstmt_, rec_num, SQL_DIAG_ROW_NUMBER, (SQLPOINTER)&row_num, 0, 0);
            ret = SQLGetDiagField(SQL_HANDLE_STMT, hstmt_, rec_num, SQL_DIAG_COLUMN_NUMBER, (SQLPOINTER)&col_num, 0, 0);

            console().log_line<ConsoleLog::LERROR>("STATE:", state, ",CODE:", error, ",row:", row_num, ",col:", col_num, ",message:", message);

	    std::regex reg{"Row: \\d+ Column: \\d+"};
	    std::smatch matches;
	    std::string target{(char*)message};
	    if (std::regex_search(target, matches, reg)) {
	      std::istringstream iss{matches[0].str()};
	      std::string temp;
	      iss >> temp >> temp;
	      row_num = std::stoi(temp);
	      iss >> temp >> temp;
	      col_num = std::stoi(temp);
	    }

	    std::cerr << "FAILED ROW:" << row_num << "\n    " << data.dump(row_num - 1);
	    std::cerr << "BAD DATA:"  << data.dump_cell(row_num - 1, col_num - 1) << std::endl;
            ++rec_num;
        }

        odb_error("SQLExecute load");
    }

    //SQLFetch(hstmt_);
    //SQLFreeStmt(hstmt_, SQL_CLOSE);

    return std::move(data);
}
