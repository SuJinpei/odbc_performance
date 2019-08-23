#include "LoadJob.h"

#include <future>

LoadJob::LoadJob()
{
    supports_options_.set_option_info("-d", "Data Source Name for data loading", true, false, false);
    supports_options_.set_option_info("-u", "User Name to connect to the Data Source", true, false, false);
    supports_options_.set_option_info("-p", "Password of the user to connect to the data source", true, false, false);

    supports_options_.set_option_info("src", "Input file. You can use the following keywords for this field:\n"
        "- %t expand to the(lower case) table name\n"
        "- %T expand to the(upper case) table name\n"
        "- %s / %S expand to the schema name\n"
        "- %c / %C expand to the catalog name\n"
        "- stdin load reading from the standard input\n"
        "--<file> to load all files listed in <file>\n"
        "-[hdfs][@host,port[, user]].<hdfspath> to load files from Hadoop File System(via libhdfs.so)\n"
        "-[mapr][@host,port[, user]].<maprpath> to load files from MapR File System(via libMapRClient.so)",
        true, false, false);

    supports_options_.set_option_info("tgt", "This is the target table", true, false, false);
    supports_options_.set_option_info("map", "Uses mapfile to map source file to target table columns, see manual.", true, false);
    supports_options_.set_option_info("fs", "This is the field separator. You can define the field separator:\n"
        "- as normal character(for example fs=, )\n"
        "- as ASCII decimal(for example fs = 44 - 44 means comma)\n"
        "- as ASCII octal value(for example fs = 054 ¨C 054 means comma)\n"
        "- as ASCII hex value(for example fs = x2C ¨C x2C means comma)", true, false);

    supports_options_.set_option_info("rs", "This is the record separator. You can define the record separator the same way as the field separator.", true, false);
    supports_options_.set_option_info("soe", "Stop On Error ¡ª stop as soon as odb encounters an error.", false, false);
    supports_options_.set_option_info("skip", "Skips a given number of lines when loading. This can be useful to skip headers in the source file.", true, false);
    supports_options_.set_option_info("ns", "odb inserts NULL when it finds nullstring in the input file. By default the nullstring is the empty string", true, false);
    supports_options_.set_option_info("ec", "The character used as escape character. You can define the escape character the same way as the field separator.", true, false);
    supports_options_.set_option_info("sq", "The string qualifier character used to enclose strings. You can define the escape character the same way as the field separator.", true, false);
    supports_options_.set_option_info("pc", "Pad character used when loading fixed format files. You can use the same notation as the field separator.", true, false);
    supports_options_.set_option_info("em", "the map file defines source data transformation rules", true, false);
    supports_options_.set_option_info("errmax", "odb orints up to num error messages per rowset. Normally used\n"
        "with soe to limit the number of error messages printed to the\n"
        "standard error", true, false);
    supports_options_.set_option_info("commit", "Defines how odb commits the inserts. You have the following choices:\n"
        "- auto (default) : Commit every single insert(see also rows load operator).\n"
        "- end : Commit when all rows(assigned to a given thread) have been inserted.\n"
        "- #rows : Commit every #rows inserted rows.\n"
        "- x#rs: Commit every #rs rowsets(see rows)", true, false);


    supports_options_.set_option_info("norb", "Loads WITH NO ROLLBACK.", true, false);
    supports_options_.set_option_info("full", "the map file defines source data transformation rules", true, false);
    supports_options_.set_option_info("truncate", "Truncates the target table before loading.", false, false);
    supports_options_.set_option_info("show", "odb prints what would be loaded in each column but no data is\n"
        "actually loaded.This is useful if you want to see how the input file\n"
        "fits into the target tables, Normally used to analyze the first few\n"
        "rows of CSV files(use :max).This option forces : \n\n"
        "- parallel to 1.\n"
        "- rows to 1.\n"
        "- ifempty to false.\n"
        "- truncate to false.", false, false, true, "false");

    supports_options_.set_option_info("bpc", "Bytes allocated in the ODBC buffer for each (non wide) CHAR / VARCHAR column length unit. (Default: 1)", true, false);
    supports_options_.set_option_info("bpwc", "Bytes allocated in the ODBC buffer for each (wide) CHAR / VARCHAR column length unit. (Default: 4)", true, false);
    supports_options_.set_option_info("nomark", "Don¡¯t print the number of records loaded so far during loads", true, false);

    supports_options_.set_option_info("iobuff", "the map file defines source data transformation rules", true, false);
    supports_options_.set_option_info("buffsz", "the map file defines source data transformation rules", true, false);
    supports_options_.set_option_info("fieldtrunc", "Defines how odb manages fields longer than the destination target column :\n"
        "-fieldtrunc = 0 (default) : Truncates input string, print a warning\n"
        "and load the truncated field if the target column is a text field.\n"
        "- fieldtrunc = 1 : Like fieldtrunc = 0 but no warning message is printed.\n"
        "- fieldtrunc = 2 : Prints an error message and does NOT load the row.\n"
        "- fieldtrunc = 3 : Like fieldtrunc = 0 but tries to load the field even if the target column is NOT a text field.\n"
        "- fieldtrunc = 4 : Like fieldtrunc = 3 but no warnings are printed.\n"
        "WARNING : the last two options could bring to unwanted resilts.\n"
        "For example, an input string like 2001 - 10 - 2345 is loaded as a valid\n"
        "2001 - 10 - 23 if the target field is a DATE.", true, false);
    supports_options_.set_option_info("pre", "odb runs a single instance of either sqlfile script or sqlcmd\n"
        "(enclosed between square brackets) on the target system\n"
        "immediately before loading the target table.You can, for example,\n"
        "CREATE the target table before loading it.\n"
        "Target table is not loaded if SQL execution fails and Stop On Error\n"
        "(soe) is set.", true, false);
    supports_options_.set_option_info("post", "odb runs a single instance of either sqlfile script or sqlcmd\n"
        "(enclosed between square brackets) on the target system\n"
        "immediately after the target table has been loaded.You can, for\n"
        "example, update database stats after loading a table.", true, false);
    supports_options_.set_option_info("ifempty", "Loads the target table only if it contains no records.", true, false);
    supports_options_.set_option_info("direct", "Adds /*+ DIRECT */ hint to the insert statement. To be used with\n"
        "Vertica databases in order to store inserted rows directly into the\n"
        "Read - Only Storage(ROS).See Vertica¡¯s documentation.", true, false);
    supports_options_.set_option_info("bad", "Where to write rejected rows. If you omit this parameter, then\n"
        "rejected rows is printed to standard error together with the error\n"
        "supports_options_urned by the ODBC Driver.\n"
        "If you add a + sign in front of the file - name, odb appends to\n"
        "<file>`instead of create the `<file > .", true, false);

    supports_options_.set_option_info("tpar", "odb loads num tables in parallel when src is a list of files to be loaded.", true, false);
    supports_options_.set_option_info("maxlen", "odb limits the amount of memory allocated in the ODBC buffers for CHAR / VARCHAR fields to #bytes.", true, false);
    supports_options_.set_option_info("time", "odb prints a timeline (milliseconds from start) for each insert.", false, false);
    supports_options_.set_option_info("loadcmd", "the map file defines source data transformation rules", true, false, true, "IN");
    supports_options_.set_option_info("xmltag", "Input file is XML. Load all XML nodes under the one specified\n"
        "with this option.If a plus sign is specified, then odb loads nodeattributesvalues.", true, false);
    supports_options_.set_option_info("xmlord", "By default, odb matches target table columns with XML node or\n"
                                    "attributes using their names.If this option is specified, then odb\n"
                                    "loads the first node / attribute to the first column, the second\n"
                                    "node / attribute to the second column and so on without checking\n"
                                    "node / attribute names.", false, false);
    supports_options_.set_option_info("xmldump", "odb does not load the XML file content. Instead, XML\n"
        "attribute / tage names are printed to standard output so you can\n"
        "check what is going to be loaded.", false, false);

    supports_options_.set_option_info("sql", "odb allow user customize load sql.", true);
    supports_options_.set_option_info("columns", "odb allow user specify columns to load", true);
}

std::string LoadJob::name() const
{
    return std::string("Load Job");
}

void LoadJob::init(const Options & runer_option, std::string command)
{
    ETLJob::init(runer_option, command);

    // initialize map rules
    map_rules_ptr_ = std::make_unique<MapRules>(options_.option_value("map"));

    // for load job, we need to connect to database first to determine dataloading buffer.
    std::ostringstream oss;
    oss << "DSN=" << options_.option_value("-d") << ";UID=" << options_.option_value("-u") << ";PWD=" << options_.option_value("-p");

    std::vector<std::future<DBConnection>> fc;
    for (size_t n = 0; n < parallel_consumer_num_; ++n) {
        fc.push_back(std::async([&] {
            return DBConnection(oss.str());
        }));
    }

    for (size_t n = 0; n < parallel_producer_num_; ++n) {
        map_workers_.push_back(map_rules_ptr_->create_map_worker());
    }

    initialize_producer_meta();

    for (auto& f : fc) {
        db_connections_.push_back(f.get());
    }
}

DataProducer * LoadJob::create_producer(size_t id)
{
    return new MapDataProducer{options_, std::move(map_workers_[id])};
}

DataConsumer * LoadJob::create_consumer(size_t id)
{
    return new DBConsumer{options_, db_connections_.at(id), consumer_meta_};
}

void LoadJob::initialize_producer_buffer()
{
    db_connections_[0].execute_direct(get_meta_query());

    consumer_meta_.col_meta = db_connections_[0].get_results_meta();

    if (!SQL_SUCCEEDED(SQLFreeStmt(db_connections_[0].hstmt(), SQL_CLOSE))) {
        db_connections_[0].diag_hstmt();
        odb_error("close statement");
    }

    DataBuffer databuffer;
    databuffer.set_row_count(rows_ < produce_maxs_[0] ? rows_ : produce_maxs_[0]);

    std::cout << "consumer cols:" << consumer_meta_.col_meta.size()
              << "\n";
    for (auto &cm : consumer_meta_.col_meta) {
      std::cout << cm.column_name << "\n";
    }

    std::cout << "producer cols:" << producer_meta_.col_meta.size()
              << "\n";
    for (auto &cm : producer_meta_.col_meta) {
      std::cout << cm.column_name << "\n";
    }

    if (producer_meta_.col_meta.size() != consumer_meta_.col_meta.size())
        odb_error("source column not match the target column.");

    for (size_t i = 0, max = consumer_meta_.col_meta.size(); i < max; ++i) {
        databuffer.append(producer_meta_.col_meta[i].data_type, consumer_meta_.col_meta[i].display_size + 1);
    }

    databuffer.adjust_buffer();

    for (auto i = 1; i < parallel_producer_num_ + parallel_consumer_num_; ++i) {
        producers_data_buffer_.push(databuffer.copy());
    }

    producers_data_buffer_.push(std::move(databuffer));

    initialize_consumer_meta();
}

void LoadJob::initialize_consumer_meta()
{
    consumer_meta_.load_query = std::move(get_load_query());
    console().log_line<ConsoleLog::LDEBUG>("load query: ", consumer_meta_.load_query);

    consumer_meta_.buffer_rows = producers_data_buffer_.front().row_count();
    consumer_meta_.buffer_width = producers_data_buffer_.front().row_width();
}

void LoadJob::initialize_producer_meta()
{
    producer_meta_.col_meta.resize(map_workers_[0].count());
    for (size_t i = 0, mx = map_workers_[0].count(); i <mx; ++i) {
        producer_meta_.col_meta[i].data_type = map_workers_[0].type(i);
        producer_meta_.col_meta[i].column_name = map_workers_[0].column_name(i);
    }
}

std::string LoadJob::get_meta_query()
{
    std::ostringstream oss;
    if (options_.option_value("columns").empty()) {
        oss << "SELECT * FROM " << options_.option_value("tgt") << " WHERE 1=0";
    }
    else {
        oss << "SELECT " << options_.option_value("columns") << " FROM " << options_.option_value("tgt") << " WHERE 1=0";
    }

    return oss.str();
}

std::string LoadJob::get_load_query()
{
    std::ostringstream oss;

    if (options_.option_value("sql").empty()) {
        std::string loadcmd = options_.option_value("loadcmd");

        if (loadcmd == "IN") {
            oss << "INSERT ";
        }
        else if (loadcmd == "UP") {
            oss << "UPSERT ";
        }
        else if (loadcmd == "UL") {
            oss << "UPSERT USING LOAD ";
        }
        else {
            oss << loadcmd << " ";
        }

        oss << "INTO " << options_.option_value("tgt") << " VALUES(";

        for (const auto& cm : consumer_meta_.col_meta) {
            oss << "?,";
        }

        oss.seekp(-1, oss.cur);
        oss << ")";

        return oss.str();
    }
    else {
        return options_.option_value("sql");
    }
}
