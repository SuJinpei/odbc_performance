#include "DBProducer.h"

DBProducer::DBProducer(Options & options, DBConnection & conn, const DBConsumerMeta& meta)
    :DataProducer(options), db_conn(conn)
{
    row_status_.resize(meta.buffer_rows);

    // bind columns
    if (!SQL_SUCCEEDED(SQLSetStmtAttr(db_conn.hstmt(), SQL_ATTR_ROW_BIND_TYPE, (SQLPOINTER)meta.buffer_width, 0))) {
        conn.diag_hstmt();
        odb_error("set SQL_ATTR_ROW_BIND_TYPE = ", meta.buffer_rows);
    }

    if (!SQL_SUCCEEDED(SQLSetStmtAttr(db_conn.hstmt(), SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)meta.buffer_rows, 0))) {
        conn.diag_hstmt();
        odb_error("set SQL_ATTR_ROW_ARRAY_SIZE = ", meta.buffer_rows);
    }

    if (!SQL_SUCCEEDED(SQLSetStmtAttr(db_conn.hstmt(), SQL_ATTR_ROWS_FETCHED_PTR, (SQLPOINTER)&num_fetched_rows_, 0))) {
        conn.diag_hstmt();
        odb_error("set SQL_ATTR_ROWS_FETCHED_PTR");
    }

    if (!SQL_SUCCEEDED(SQLSetStmtAttr(db_conn.hstmt(), SQL_ATTR_ROW_STATUS_PTR, (SQLPOINTER)&row_status_[0], 0))) {
        conn.diag_hstmt();
        odb_error("set SQL_ATTR_PARAMS_PROCESSED_PTR");
    }

    if (!SQL_SUCCEEDED(SQLExecDirect(db_conn.hstmt(), (SQLCHAR*)meta.load_query.c_str(), SQL_NTS))) {
        db_conn.diag_all();
    }
}

DataBuffer DBProducer::produce_data(DataBuffer && data)
{
    for (SQLSMALLINT c = 0, max = (SQLSMALLINT)data.column_count(); c < max; ++c) {
        if (!SQL_SUCCEEDED(SQLBindCol(db_conn.hstmt(), c + 1, data.cell_data_type(c), data.cell_body_address(0, c),
            data.cell_body_capacity(c), &data.cell_body_length_ref(0, c)))) {
            db_conn.diag_all();
        }
    }

    if (!SQL_SUCCEEDED(SQLFetch(db_conn.hstmt()))) {
        db_conn.diag_hstmt();
    }

    if (num_fetched_rows_ < data.row_count()) {
        data.set_row_count(num_fetched_rows_);
    }

    return std::move(data);
}

