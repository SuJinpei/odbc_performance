#include "MapRules.h"
#include "ConsoleLog.h"

#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <cstring>
#include <cmath>


MapRules::MapRules(std::string map_file_name)
{
    std::ifstream mapfile{ map_file_name };

    for (std::string line; std::getline(mapfile, line);) {
        std::istringstream iss{ line };
        std::string field_name;
        std::string rule;

        std::getline(iss, field_name, ':');
        std::getline(iss, rule, ':');

        if (rule == "CONST") {
            if (std::string value; std::getline(iss, value)) {
                rules_.push_back(
                    std::bind(
                        [](std::string value) { return std::make_unique<ConstFeeder>(value); },
                        value));
            }
            else {
                odb_error("map rule:", rule, " expects a value.");
            }
        }
        else if (rule == "SEQ") {
            if (std::string value; std::getline(iss, value)) {
                seqs_.push_back(std::make_unique<std::atomic_long>(std::stol(value)));
                rules_.push_back(
                    std::bind(
                        [](std::atomic_long& start) { return std::make_unique<SeqFeeder>(start); },
                        std::ref(*seqs_.back())));
            }
            else {
                odb_error("map rule:", rule, " expects a value.");
            }
        }
        else if (rule == "IRAND") {
            if (std::string min, max; std::getline(iss, min, ':') && std::getline(iss, max)) {
                rules_.push_back(
                    std::bind(
                        [](long min, long max) { return std::make_unique<IntegerRandFeeder>(min, max); },
                        std::stol(min), std::stol(max)));
            }
            else {
                odb_error("map rule:", rule, " expects a value.");
            }
        }
        else if (rule == "DRAND") {
            if (std::string min, max; std::getline(iss, min, ':') && std::getline(iss, max)) {
                rules_.push_back(
                    std::bind(
                        [](long min, long max) { return std::make_unique<DateRandFeeder>(min, max); },
                        std::stol(min), std::stol(max)));
            }
            else {
                odb_error("map rule:", rule, " expects a value.");
            }
        }
        else if (rule == "TMRAND") {
            rules_.push_back([] {return std::make_unique<TimeRandFeeder>(); });
        }
        else if (rule == "TSRAND") {
            rules_.push_back([] { return std::make_unique<TimeStampRandFeeder>(); });
        }
        else if (rule == "CRAND") {
            if (std::string len; std::getline(iss, len)) {
                rules_.push_back(
                    std::bind(
                        [](long len) {return std::make_unique<CharRandFeeder>(len); },
                        std::stol(len)));
            }
            else {
                odb_error("map rule:", rule, " expects a value.");
            }
        }
        else if (rule == "NRAND") {
            if (std::string prec, scale; std::getline(iss, prec, ':') && std::getline(iss, scale)) {
                rules_.push_back(
                    std::bind(
                        [](size_t prec, size_t scale) { return std::make_unique<NumericRandFeeder>(prec, scale); },
                        std::stoul(prec), std::stoul(scale)));
            }
            else {
                odb_error("map rule:", rule, " expects a value.");
            }
        }
        else if (rule == "UTF8RAND") {
            if (std::string len; std::getline(iss, len)) {
                rules_.push_back(
                    std::bind(
                        [](long len) {return std::make_unique<UTF8RandFeeder>(len); },
                        std::stol(len)));
            }
            else {
                odb_error("map rule:", rule, " expects a value.");
            }
        }
        else if (rule == "GBKRAND") {
            if (std::string len; std::getline(iss, len)) {
                rules_.push_back(
                    std::bind(
                        [](long len) {return std::make_unique<GBKRandFeeder>(len); },
                        std::stol(len)));
            }
            else {
                odb_error("map rule:", rule, " expects a value.");
            }
        }
        else {
            odb_error("unknown map option:", rule);
        }
    }
}

MapRules::~MapRules()
{
}

MapWorker MapRules::create_map_worker()
{
    std::vector<std::unique_ptr<Feeder>> workers;

    for (auto& create_map : rules_) {
        workers.push_back(create_map());
    }

    return MapWorker(std::move(workers));
}

MapWorker::MapWorker(std::vector<std::unique_ptr<Feeder>>&& map_workers)
    :map_workers_(std::move(map_workers))
{
}

std::string MapWorker::map(size_t column_num) const
{
    return map_workers_[column_num]->get();
}

std::string MapWorker::column_name(size_t column_num) const
{
    return map_workers_[column_num]->column_name();
}

size_t MapWorker::count() const
{
    return map_workers_.size();
}

SQLSMALLINT MapWorker::type(size_t column_num) const
{
    return map_workers_.at(column_num)->data_type();
}

void MapWorker::fill_buffer(size_t col, void * buf, size_t size)
{
    map_workers_[col]->fill_buffer(buf, size);
}

Feeder::Feeder(const std::string& col_name)
    :col_name_(col_name)
{
}

Feeder::~Feeder()
{
}

std::string Feeder::column_name()
{
    return col_name_;
}

ConstFeeder::ConstFeeder(const std::string val)
    :value_(val)
{
}

std::string ConstFeeder::get()
{
    return value_;
}

SQLSMALLINT ConstFeeder::data_type()
{
    return SQL_C_CHAR;
}

void ConstFeeder::fill_buffer(void * buf, size_t size)
{
    *(SQLLEN*)buf = SQL_NTS;
    std::strncpy((char*)buf + sizeof(SQLLEN), value_.c_str(), size);
}

SeqFeeder::SeqFeeder(std::atomic_long& start)
    :next_num_(start)
{
}

std::string SeqFeeder::get()
{
    return std::to_string(next_num_++);
}

SQLSMALLINT SeqFeeder::data_type()
{
    return SQL_C_LONG;
}

void SeqFeeder::fill_buffer(void * buf, size_t size)
{
    *(SQLLEN*)buf = 0;
    *(long*)((char*)buf + sizeof(SQLLEN)) = next_num_++;
}

IntegerRandFeeder::IntegerRandFeeder(long min, long max)
    :dist{min, max}
{
}

std::string IntegerRandFeeder::get()
{
    return std::to_string(dist(gen_));
}

SQLSMALLINT IntegerRandFeeder::data_type()
{
    return SQL_C_LONG;
}

void IntegerRandFeeder::fill_buffer(void * buf, size_t size)
{
    *(SQLLEN*)buf = 0;
    *(long*)((char*)buf + sizeof(SQLLEN)) = dist(gen_);
}

DateRandFeeder::DateRandFeeder(long min_year, long max_year)
    :dist_time_{ [&] {std::tm tm; tm.tm_year = min_year; return std::mktime(&tm); }(),
                 [&] {std::tm tm; tm.tm_year = max_year + 1; return std::mktime(&tm); }() }
{
}

std::string DateRandFeeder::get()
{
    std::ostringstream oss;
    SQL_DATE_STRUCT ds;
    fill_buffer(&ds, sizeof(ds));
    oss << ds.year << '-' << ds.month << '-' << ds.day;
    return oss.str();
}

SQLSMALLINT DateRandFeeder::data_type()
{
    return SQL_C_TYPE_DATE;
}

void DateRandFeeder::fill_buffer(void * buf, size_t size)
{
    std::tm tm;
    time_t t = dist_time_(gen_);
    gmtime_s(&tm, &t);

    *(SQLLEN*)buf = 0;
    SQL_DATE_STRUCT& date = *(SQL_DATE_STRUCT*)((char*)buf + sizeof(SQLLEN));

    date.year = static_cast<SQLSMALLINT>(tm.tm_year);
    date.month = static_cast<SQLSMALLINT>(tm.tm_mon);
    date.day = static_cast<SQLSMALLINT>(tm.tm_mday);
}

TimeRandFeeder::TimeRandFeeder()
    :dist_hour_{0, 23}, dist_minute_{0, 59}, dist_second_{0, 59}
{
}

std::string TimeRandFeeder::get()
{
    std::ostringstream oss;
    oss << dist_hour_(gen_) << ":" << dist_minute_(gen_) << ":" << dist_second_(gen_);
    return oss.str();
}

SQLSMALLINT TimeRandFeeder::data_type()
{
    return SQL_C_TYPE_DATE;
}

void TimeRandFeeder::fill_buffer(void * buf, size_t size)
{
    *(SQLLEN*)buf = 0;
    SQL_TIME_STRUCT& ts = *(SQL_TIME_STRUCT*)((char*)buf + sizeof(SQLLEN));
    ts.hour = (SQLSMALLINT)dist_hour_(gen_);
    ts.minute = (SQLSMALLINT)dist_minute_(gen_);
    ts.second = (SQLSMALLINT)dist_second_(gen_);
}

TimeStampRandFeeder::TimeStampRandFeeder()
    :dist_{0, std::time(nullptr)}
{
}

std::string TimeStampRandFeeder::get()
{
    std::ostringstream oss;
    time_t rand_time = dist_(gen_);
    std::tm& tm = *std::localtime(&rand_time);
    oss << std::put_time(&tm, "%F %T");

    return oss.str();
}

SQLSMALLINT TimeStampRandFeeder::data_type()
{
    return SQL_C_TYPE_TIMESTAMP;
}

void TimeStampRandFeeder::fill_buffer(void * buf, size_t size)
{
    std::tm tm;
    time_t t = dist_(gen_);
    localtime_s(&tm, &t);

    *(SQLLEN*)buf = 0;
    SQL_TIMESTAMP_STRUCT& tms = *(SQL_TIMESTAMP_STRUCT*)((char*)buf + sizeof(SQLLEN));
    tms.year = tm.tm_year + 1900;
    tms.month = tm.tm_mon;
    tms.day = tm.tm_mday;
    tms.hour = tm.tm_hour;
    tms.minute = tm.tm_min;
    tms.second = tm.tm_sec;
}

CharRandFeeder::CharRandFeeder(size_t len)
    :length_{ len },
    chars_{ "`1234567890-=qwertyuiop[]asdfghjkl;'\\zxcvbnm,./~!@#$%^&*()_+QWERTYUIOP{}ASDFGHJKL:\"|ZXCVBNM<>?" },
    chars_len_{chars_.size()},
    dist_{0, chars_len_ - 1}
{
}

std::string CharRandFeeder::get()
{
    std::string result;
    std::generate_n(std::back_inserter(result), length_, [&] { return chars_[dist_(gen_)]; });

    return std::move(result);
}

SQLSMALLINT CharRandFeeder::data_type()
{
    return SQL_C_CHAR;
}

void CharRandFeeder::fill_buffer(void * buf, size_t size)
{
    *(SQLLEN*)buf = SQL_NTS;
    char *p = (char*)buf + sizeof(SQLLEN);

    size_t rnd = 0;
    for (int i = 0; i < length_; ++i) {
        if (i % 60 == 0)
            rnd = gen_();
        else
            rnd = rnd >> 1;
        *p++ = chars_[rnd % chars_len_];
    }
    *p = '\0';
}

NumericRandFeeder::NumericRandFeeder(size_t precission, size_t scale)
    :dist_{0, static_cast<size_t>(std::pow(10, precission) - 1)},
     scale_div_{static_cast<size_t>(std::pow(10, scale))}
{
}

std::string NumericRandFeeder::get()
{
    double d;
    fill_buffer(&d, 0);
    oss_.str("");
    oss_ << d;
    return oss_.str();;
}

SQLSMALLINT NumericRandFeeder::data_type()
{
    return SQL_C_DOUBLE;
}

void NumericRandFeeder::fill_buffer(void * buf, size_t size)
{
    *(SQLLEN*)buf = 0;
    double& d = *(double*)((char*)buf + sizeof(SQLLEN));
    d = dist_(gen_) * 1.0;
    d /= scale_div_;
}

UTF8RandFeeder::UTF8RandFeeder(size_t len)
    :len_(len), dist_{0x3300, 0x9fff}
{
}

std::string UTF8RandFeeder::get()
{
    std::ostringstream oss;
    
    for (int i = 0; i < len_; ++i) {
        char buf[5] = {};
        oss << utf8_from_code((int)dist_(gen_), buf);
    }

    return oss.str();
}

SQLSMALLINT UTF8RandFeeder::data_type()
{
    return SQL_C_CHAR;
}

void UTF8RandFeeder::fill_buffer(void * buf, size_t size)
{
    *(SQLLEN*)buf = 0;
    char *p = (char*)buf + sizeof(SQLLEN);
    for (int i = 0; i < len_; ++i) {
        p = utf8_from_code((int)dist_(gen_), p);
    }
    *p = '\0';
}

// caller should ensure at least 4 bytes provided.
char* UTF8RandFeeder::utf8_from_code(int cp, char *buf)
{
    if (cp <= 0x7F) { *buf++ = cp; }
    else if (cp <= 0x7FF) { *buf++ = (cp >> 6) + 192; *buf++ = (cp & 63) + 128; }
    else if (0xd800 <= cp && cp <= 0xdfff) {} //invalid block of utf8
    else if (cp <= 0xFFFF) { *buf++ = (cp >> 12) + 224; *buf++ = ((cp >> 6) & 63) + 128; *buf++ = (cp & 63) + 128; }
    else if (cp <= 0x10FFFF) { *buf++ = (cp >> 18) + 240; *buf++ = ((cp >> 12) & 63) + 128; *buf++ = ((cp >> 6) & 63) + 128; *buf++ = (cp & 63) + 128; }
    return buf;
}

GBKRandFeeder::GBKRandFeeder(size_t len)
    :len_(len), dist_{ 0x0000, 0xffff }
{
}

std::string GBKRandFeeder::get()
{
    char buf[3] = {};
    std::ostringstream oss;
    for (int i = 0; i < len_; ++i) {
        oss << rand_one_gbk_char(buf);
    }
    return oss.str();
}

SQLSMALLINT GBKRandFeeder::data_type()
{
    return SQL_C_CHAR;
}

void GBKRandFeeder::fill_buffer(void * buf, size_t size)
{
    *(SQLLEN*)buf = 0;
    char *p = (char*)buf + sizeof(SQLLEN);
    for (int i = 0; i < len_; ++i,p+=2) {
        rand_one_gbk_char(p);
    }
    *p = '\0';
}

char * GBKRandFeeder::rand_one_gbk_char(char * buf)
{
    short *ps = reinterpret_cast<short*>(buf);
    *ps = static_cast<short>(dist_(gen_));

    *ps |= 0x8100 | 0x40;
    if (*ps & 0x7f) {
        *ps -= 1;
    }

    if (0xa0 < (unsigned char)buf[1] && (unsigned char)buf[1] < 0xa8) {
        if (0x39 < (unsigned char)buf[0] && (unsigned char)buf[0] < 0xa1) {
            buf[1] += 8;
        }
    }

    if (0xa0 < (unsigned char)buf[0]) {
        if (0xa9 < (unsigned char)buf[1] && (unsigned char)buf[1] < 0xb0) {
            buf[1] += 8;
        }
        if (0xf7 < (unsigned char)buf[1]) {
            buf[1] -= 8;
        }
    }
    return buf;
}

FastUniformIntDistribution::FastUniformIntDistribution(FastUniformIntDistribution::result_type min, 
    FastUniformIntDistribution::result_type max)
    :dist_(max - min + 1)
{
}

RandomFeeder::RandomFeeder()
    :gen_(std::random_device()())
{
}
