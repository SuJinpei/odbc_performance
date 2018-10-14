#include "CopyJob.h"

#include <future>

CopyJob::CopyJob()
{
    supports_options_.set_option_info("-d", "Data source name for source DB and target DB, example \"-d=srcdsn:tgt=dsn\"", true, false, false);
    supports_options_.set_option_info("-u", "User name for source DB and target DB, example \"-d=srcuser:tgt=tgtuser\"", true, false, false);
    supports_options_.set_option_info("-p", "Password for source DB and target DB, example \"-d=srcpass:tgt=tgtpass\"", true, false, false);

    supports_options_.set_option_info("src", "Defines the source table(s). You can use:\n"
        "- a single table(for example: TRAFODION.MFTEST.LINEITEM)\n"
        "- a group of tables(for example: TRAFODION.MFTEST.LIN%)\n"
        "- a file containing a list of tables to copy(' - ' should precede the filename))", true, false, false);

    supports_options_.set_option_info("tgt", "Target table(s). You can use the following keywords for this field:\n"
        "- %t / %T: Expands to the(lower / upper case) source table name.\n"
        "- %s / %S : Expands to the(lower / upper case) source schema name.\n"
        "- %c / %C : Expands to the(lower / upper case) source catalog name.", true, false, false);

    supports_options_.set_option_info("sql", "odb uses a generic SQL ¡ª instead of a real table ¨C- as source.", true, false);
    supports_options_.set_option_info("max", "This is the max number of records to copy."
        " Default is to copy all records in the source table.", true, false, true, "-1");

    supports_options_.set_option_info("rows", "Defines the size of the I/O buffer for each copy thread."
        "You can define the size of this buffer in two different ways :\n"
        "-number of rows(for example: rows = 100 means 100 rows as IO buffer)\n"
        "- buffer size in kB or MB(for example: rows = k512(512 kB buffer) or rows = m20(20MB buffer))", true, false, true, "100");

    supports_options_.set_option_info("truncate", "Truncates the target table before loading.");
    supports_options_.set_option_info("ifempty", "Loads the target table only if empty.");
    supports_options_.set_option_info("nomark", "Don¡¯t print the number of records loaded so far during loads.");
    supports_options_.set_option_info("soe", "Stop On Error. odb stops as soon as it encounters an error.");

    supports_options_.set_option_info("parallel", "odb uses as many threads as the parallel argument to extract data from"
        " partitioned source tables PLUS an equivalent number of threads to write to the target table.\n"
        "You have to specify splitby.", true, false, true, "1");

    supports_options_.set_option_info("pwhere", "Used in conjunction with parallel to copy only records satisfying the where condition.\n"
        "Note: The where condition is limited to columns in the source table.\n"
        "Example:\n"
        "You want to copy records with TRANS_TS > 1999 - 12 - 12 09:00 : 00 from\n"
        "the source table TRAFODION.MAURO.MFORDERS using eight parallel\n"
        "streams to a target table having the same name as the source table :\n"
        "src=trafodion.mauro.mforders:tgt=trafodion.dest_schema.%t:parallel=8:pwhere=[TRANS_TS > TIMESTAMP ¡®1999 - 12 - 12 09:00 : 00¡¯]¡­\n"
        "You can enclose the where condition between square brackets to avoid\n"
        "a misinterpretation of the characters in the where condition.", true);

    supports_options_.set_option_info("commit", "Defines how odb will commit the inserts. You have the following choices:\n"
        "- auto (Default)¡ª Commits every single insert(see also rows load operator).\n"
        "- end commits when all rows(assigned to a given thread) have been inserted.\n"
        "- #rows ¡ª Commits every #rows copied rows.\n"
        "- x#rs ¡ª Commits every #rs rowsets copied. (See :rows)", true, false, true, "auto");

    supports_options_.set_option_info("uncommitted", "Adds FOR READ UNCOMMITTED ACCESS to the select(s) command(s).");

    supports_options_.set_option_info("splitby", "Lets you to use parallel copy from any database. <column> has to be a"
        " SINGLE, numeric column.odb calculates min() / max() value for"
        " <column> and assigns to each <parallel> thread the extraction of the"
        " rows in its bucket.", true);
}

std::string CopyJob::name() const
{
    return std::string("Copy Job");
}

void CopyJob::init(const Options & runer_option, std::string command)
{
    ETLJob::init(runer_option, command);

    parse_connection_info();
    create_producer_connections();
    create_consumer_connections();
}

DataProducer * CopyJob::create_producer(size_t id)
{
    return new DBProducer{options_, producer_connections_[id]};
}

DataConsumer * CopyJob::create_consumer(size_t id)
{
    return new DBConsumer{ options_, consumer_connections_[id], consumer_meta_ };
}

void CopyJob::initialize_producer_buffer()
{
    console().log<ConsoleLog::LERROR>("unimplemented copy job initialization buffer\n");
}

void CopyJob::parse_connection_info()
{
    std::istringstream dsn_iss{ options_.option_value("-d") };
    std::getline(dsn_iss, src_dsn, ':');
    if (!std::getline(dsn_iss, tgt_dsn)) {
        odb_error("Bad Command: -d expected target dsn, please refer -cp command for help");
    }

    std::istringstream uid_iss{ options_.option_value("-u") };
    std::getline(uid_iss, src_uid, ':');
    if (!std::getline(uid_iss, tgt_uid)) {
        odb_error("Bad Command: -u expected target uid, please refer -cp command for help");
    }

    std::istringstream pwd_iss{ options_.option_value("-p") };
    std::getline(pwd_iss, src_uid, ':');
    if (!std::getline(pwd_iss, tgt_uid)) {
        odb_error("Bad Command: -p expected target pwd, please refer -cp command for help");
    }
}

void CopyJob::create_producer_connections()
{
    std::vector<std::future<DBConnection>> future_conns;

    for (size_t i = 0; i < parallel_producer_num_; ++i) {
        future_conns.push_back(std::async([&] {
            return DBConnection(src_dsn, src_uid, src_pwd);
        }));
    }

    for (auto& fc : future_conns) {
        producer_connections_.push_back(fc.get());
    }
}

void CopyJob::create_consumer_connections()
{
    std::vector<std::future<DBConnection>> future_conns;

    for (size_t i = 0; i < parallel_consumer_num_; ++i) {
        future_conns.push_back(std::async([&] {
            return DBConnection(tgt_dsn, tgt_uid, tgt_pwd);
        }));
    }

    for (auto& fc : future_conns) {
        consumer_connections_.push_back(fc.get());
    }
}
