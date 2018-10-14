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

Feeder::Feeder()
{
}

Feeder::~Feeder()
{
}

ConstFeeder::ConstFeeder(const std::string val)
    :value_(val)
{
}

std::string ConstFeeder::get()
{
    return value_;
}

SeqFeeder::SeqFeeder(std::atomic_long& start)
    :next_num_(start)
{
}

std::string SeqFeeder::get()
{
    return std::to_string(next_num_++);
}

IntegerRandFeeder::IntegerRandFeeder(long min, long max)
    :dist{min, max}
{
}

std::string IntegerRandFeeder::get()
{
    return std::to_string(dist(gen_));
}

DateRandFeeder::DateRandFeeder(long min_year, long max_year)
    :dist_year_{min_year, max_year}, dist_month_{1, 12},
    dist_28_{1, 28}, dist_29_{1, 29}, dist_30_{1, 30}, dist_31_{1, 31}
{
}

std::string DateRandFeeder::get()
{
    int year = dist_year_(gen_);
    int month = dist_month_(gen_);
    int day = 0;

    switch (month)
    {
    case 2:
        if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0)) { // Leap Year
            day = dist_29_(gen_);
        }
        else {
            day = dist_28_(gen_);
        }
        break;
    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
        day = dist_31_(gen_);
        break;
    default:
        day = dist_30_(gen_);
        break;
    }

    std::ostringstream oss;
    oss << year << "-" << month << "-" << day;
    return oss.str();
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

std::string TimeStampRandFeeder::get()
{
    std::ostringstream oss;

    std::uniform_int_distribution<time_t> dist{ 0, std::time(nullptr) };
    time_t rand_time = dist(gen_);
    std::tm& tm = *std::localtime(&rand_time);
    oss << std::put_time(&tm, "%F %T");

    return oss.str();
}

CharRandFeeder::CharRandFeeder(size_t len)
    :length_(len)
{
    chars_ = "`1234567890-=qwertyuiop[]asdfghjkl;'\\zxcvbnm,./~!@#$%^&*()_+QWERTYUIOP{}ASDFGHJKL:\"|ZXCVBNM<>?";
    dist_ = std::uniform_int_distribution<size_t>{ 0, chars_.size() };
}

std::string CharRandFeeder::get()
{
    std::string result;
    std::generate_n(std::back_inserter(result), length_, [&] { return chars_[dist_(gen_)]; });

    return result;
}

NumericRandFeeder::NumericRandFeeder(size_t precission, size_t scale)
    :prec_{precission}, scale_{scale}
{
    double max = 1.0;
    for (size_t i = 0; i < precission - scale; ++i) {
        max *= 10;
    }


    double delta = 5.0 / std::pow(10, (scale + 1));
    dist_ = std::uniform_real_distribution<double>{ -max + delta, max - delta };
    buf_ = new char[precission + 3];

    std::ostringstream oss;
    oss << "%." << scale << "f";
    format_ = oss.str();
}

NumericRandFeeder::~NumericRandFeeder()
{
    if (buf_) {
        delete buf_;
    }
}

std::string NumericRandFeeder::get()
{
    std::snprintf(buf_, (prec_ + 3), format_.c_str(), dist_(gen_));
    return std::string(buf_);
}
