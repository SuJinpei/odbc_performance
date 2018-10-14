#ifndef MAP_RULES_H
#define MAP_RULES_H

#include <utility>
#include <string>
#include <vector>
#include <memory>
#include <random>
#include <functional>
#include <atomic>

class Feeder
{
public:
    Feeder();
    Feeder(Feeder&& f) = default;
    Feeder& operator=(Feeder&& f) = default;
    virtual ~Feeder();

    virtual std::string get() = 0;

};

class RandomFeeder : public Feeder {
protected:
    std::random_device gen_;
};

// CONST
class ConstFeeder : public Feeder {
public:
    ConstFeeder(const std::string val);
    std::string get() override;

private:
    const std::string value_;
};

// SEQ
class SeqFeeder : public Feeder {
public:
    SeqFeeder(std::atomic_long& start);
    std::string get() override;

private:
    std::atomic_long& next_num_; // may concurrent access.
};

// IRAND
class IntegerRandFeeder : public RandomFeeder {
public:
    IntegerRandFeeder(long min, long max);
    std::string get() override;

private:
    std::uniform_int_distribution<int> dist;
};

// DRAND
class DateRandFeeder : public RandomFeeder {
public:
    DateRandFeeder(long min_year, long max_year);
    std::string get() override;

private:
    std::uniform_int_distribution<int> dist_year_;
    std::uniform_int_distribution<int> dist_month_;
    std::uniform_int_distribution<int> dist_28_;
    std::uniform_int_distribution<int> dist_29_;
    std::uniform_int_distribution<int> dist_30_;
    std::uniform_int_distribution<int> dist_31_;
};

// TMRAND
class TimeRandFeeder : public RandomFeeder {
public:
    TimeRandFeeder();
    std::string get();

private:
    std::uniform_int_distribution<int> dist_hour_;
    std::uniform_int_distribution<int> dist_minute_;
    std::uniform_int_distribution<int> dist_second_;
};

// TSRAND
class TimeStampRandFeeder : public RandomFeeder {
public:
    std::string get();
};

// CRAND
class CharRandFeeder : public RandomFeeder {
public:
    CharRandFeeder(size_t len);
    std::string get();

private:
    size_t length_;
    std::string chars_;
    std::uniform_int_distribution<size_t> dist_;
};

// NRAND
class NumericRandFeeder : public RandomFeeder {
public:
    NumericRandFeeder(size_t precission, size_t scale);
    ~NumericRandFeeder();
    std::string get();

private:
    size_t prec_;
    size_t scale_;
    std::uniform_real_distribution<double> dist_;
    char *buf_;
    std::string format_;
};

// MapWorker is thread safe for parallel maping.
class MapWorker {
public:
    MapWorker(std::vector<std::unique_ptr<Feeder>>&& map_workers);
    std::string map(size_t column_num) const;

private:
    std::vector<std::unique_ptr<Feeder>> map_workers_;
};

class MapRules
{
public:
    MapRules(std::string map_file_name);
    ~MapRules();

    MapWorker create_map_worker();

private:
    std::vector<std::function<std::unique_ptr<Feeder>()>> rules_;
    std::vector<std::unique_ptr<std::atomic_long>> seqs_;
};

#endif // MAP_RULES_H