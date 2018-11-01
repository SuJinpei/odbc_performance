#include "ExtractJob.h"

#include <future>

ExtractJob::ExtractJob()
{
    supports_options_.set_option_info("-d", "Data Source Name for extracting data", true, false, false);
    supports_options_.set_option_info("-u", "User Name to connect to the Data Source", true, false, false);
    supports_options_.set_option_info("-p", "Password of the user to connect to the data source", true, false, false);

    supports_options_.set_option_info("src", "Defines the source table(s). You can use:\n"
        "- a single table name(for example TRAFODION.MFTEST.LINEITEM)\n"
        "- a group of tables(for example TRAFODION.MFTEST.LIN%)\n"
        "- a file containing a list of tables to extract(-should precede the filename)", true);

    supports_options_.set_option_info("sql", "A custom SQL command you can use to extract data. This is alternative to src = .", true);

    supports_options_.set_option_info("tgt", "Output file. You can use the following keywords for this field:\n"
        "- %t / %T expands to the(lower / upper case) table name\n"
        "- %s / %S expands to the(lower / upper case) schema name\n"
        "- %c / %C expands to the(lower / upper case) catalog name\n"
        "- %d expands to the extraction date(YYYYMMDD format)\n"
        "- %D expands to the extraction date(YYYY - MM - DD format)\n"
        "- %m expands to the extraction time(hhmmss format)\n"
        "- %M expands to the extraction time(hh:mm:ss format)\n"
        "- stdout prints the extracted records to the standard output.\n"
        "If you add a + sign in front of the file - name, then odb appends to file instead of creates file.\n"
        "hdfs. / <hdfspath> / <file> to write exported table under the Hadoop File Distributed System(HDFS).", true, false, false);

    supports_options_.set_option_info("fs", "Field separator. You can define the field separator as:\n"
        "- a normal character(for example fs=, )\n"
        "- ASCII decimal(for example fs = 44 - 44 means comma)\n"
        "- ASCII octal value(for example fs = 054 ¨C 054 means comma)\n"
        "- ASCII hex value(for example fs = x2C ¨C x2C means comma)\n"
        "The default field separator is, (comma)", true, false, true, ",");

    supports_options_.set_option_info("rs", "Record separator. You can define the record separator the same way as the field separator.\n"
        "The default record separator is \n(new line)", true, false, true, "\n");

    supports_options_.set_option_info("max", "Max number of records to extract.The default is to extract all records", true, false, true, "-1");

    supports_options_.set_option_info("sq", "The string qualifier character used to enclose strings."
        "You can define the string qualifier the same way as the field separator.", true, false, true, "\"");

    supports_options_.set_option_info("ec", "Character used as escape character. You can define the escape character the same way as the field separator."
        "Default is \\ (back slash).", true, false, true, "\\");

    supports_options_.set_option_info("ns", "How odb represents NULL values in the output file."
        "Default is the empty string(two field separators one after the other)", true);

    supports_options_.set_option_info("es", "How odb represents VARCHAR empty strings (NOT NULL with zero length) values in the output file."
        "Default is the empty string(two field separators one after the other)", true);

    supports_options_.set_option_info("gzpar", "This are extra parameters you can pass to tune the gzip compression algorithm.\n"
        "Examples\n"
        "- gzpar = wb9: max compression(slower)\n"
        "- gzpar = wb1 : basic compression(faster)\n"
        "- gzpar = wb6h : Huffman compression only\n"
        "- gzpar = wb6R : Run - length encoding only", true);

    supports_options_.set_option_info("nomark", "Don¡¯t print the number of records extracted so far by each thread.");
    supports_options_.set_option_info("soe", "Stop On Error. odb stop as soon as it encounters an error.");
    supports_options_.set_option_info("parallel", "odb uses as many threads as the parallel argument to extract data from "
        "partitioned source tables.You have to use splitby.\n"
        "Each thread takes care of a specific range of the source table partitions.\n"
        "For example if you specify parallel = 4 and the source table is made of "
        "32 partitions, then odb starts four threads(four ODBC connections) :\n"
        "-thread 0 extracts partitions 0 - 7\n"
        "- thread 1 extracts partitions 8 - 15\n"
        "- thread 2 extracts partitions 16 - 23\n"
        "- thread 3 extracts partitions 24 - 31", true, false, true, "1");
}

std::string ExtractJob::name() const
{
    return std::string("Extract Job");
}

void ExtractJob::init(const Options & runer_option, std::string command)
{
    ETLJob::init(runer_option, command);

    // for extract job, we need to connect to database first to determine extracting buffer.
    std::ostringstream oss;
    oss << "DSN=" << options_.option_value("-d") << ";UID=" << options_.option_value("-u") << ";PWD=" << options_.option_value("-p");

    std::vector<std::future<DBConnection>> fc;
    for (size_t n = 0; n < parallel_consumer_num_; ++n) {
        fc.push_back(std::async([&] {
            return DBConnection(oss.str());
        }));
    }

    for (auto& f : fc) {
        db_connections_.push_back(f.get());
    }
}

SQLSMALLINT ExtractJob::sql_type_to_C(SQLSMALLINT sql_type)
{
    switch (sql_type)
    {
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
    case SQL_DECIMAL:
    case SQL_NUMERIC:
        return SQL_C_CHAR;
    case SQL_WCHAR:
    case SQL_WVARCHAR:
    case SQL_WLONGVARCHAR:
        return SQL_C_WCHAR;
    case SQL_BIT:
        return SQL_C_BIT;
    case SQL_TINYINT:
        return SQL_C_TINYINT;
    case SQL_SMALLINT:
        return SQL_C_SSHORT;
    case SQL_INTEGER:
        return SQL_C_SLONG;
    case SQL_BIGINT:
        return SQL_C_SBIGINT;
    case SQL_REAL:
        return SQL_C_FLOAT;
    case SQL_FLOAT:
    case SQL_DOUBLE:
        return SQL_C_DOUBLE;
    case SQL_BINARY:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY:
        return SQL_C_BINARY;
    case SQL_TYPE_DATE:
        return SQL_C_TYPE_DATE;
    case SQL_TYPE_TIME:
        return SQL_C_TYPE_TIME;
    case SQL_TYPE_TIMESTAMP:
        return SQL_C_TYPE_TIMESTAMP;
    default:
        return SQL_C_DEFAULT;
    };
}

DataProducer * ExtractJob::create_producer(size_t id)
{
    return new DBProducer(options_, db_connections_[id], producer_meta_);
}

DataConsumer * ExtractJob::create_consumer(size_t id)
{
    return new FileConsumer(options_);
}

void ExtractJob::initialize_producer_buffer()
{
    producer_meta_.load_query = get_select_query();
    db_connections_[0].execute_direct(producer_meta_.load_query);

    producer_meta_.col_meta = db_connections_[0].get_results_meta();

    for (size_t i = 0; i < producer_meta_.col_meta.size(); ++i) {
        producer_meta_.col_meta[i].data_type = sql_type_to_C(producer_meta_.col_meta[i].data_type);
    }

    if (!SQL_SUCCEEDED(SQLFreeStmt(db_connections_[0].hstmt(), SQL_CLOSE))) {
        db_connections_[0].diag_hstmt();
        odb_error("close statement");
    }

    DataBuffer databuffer;
    databuffer.set_row_count(rows_ < produce_maxs_[0] ? rows_ : produce_maxs_[0]);

    for (size_t i = 0, max = producer_meta_.col_meta.size(); i < max; ++i) {
        databuffer.append(producer_meta_.col_meta[i].data_type, producer_meta_.col_meta[i].display_size + 1);
    }

    databuffer.adjust_buffer();

    for (auto i = 1; i < parallel_producer_num_ + parallel_consumer_num_; ++i) {
        producers_data_buffer_.push(databuffer.copy());
    }

    producer_meta_.buffer_width = databuffer.row_width();
    producer_meta_.buffer_rows = databuffer.row_count();

    producers_data_buffer_.push(std::move(databuffer));
}

std::string ExtractJob::get_select_query() const
{
    std::ostringstream oss;
    if (!options_.option_value("sql").empty()) {
        oss << options_.option_value("sql");
    }
    else if (!options_.option_value("src").empty()) {
        oss << "SELECT * FROM " << options_.option_value("src");
    }
    else {
        odb_error("Neither 'sql' nor 'src' option provided for Extract Job.");
    }

    return oss.str();
}
