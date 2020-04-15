#ifndef MAP_RULES_H
#define MAP_RULES_H

#include "Common.h"

#include <utility>
#include <string>
#include <vector>
#include <memory>
#include <random>
#include <functional>
#include <atomic>
#include <ctime>

class FastUniformIntDistribution {
public:
    typedef size_t result_type;
    FastUniformIntDistribution(result_type min, result_type max);

    template<typename Generator>
    result_type operator()(Generator& gen) {
        return gen() % dist_;
    }

private:
    result_type dist_;
};

class Feeder
{
public:
    Feeder(const std::string& col_name = "");
    Feeder(Feeder&& f) = default;
    Feeder& operator=(Feeder&& f) = default;
    virtual ~Feeder();

    virtual std::string get() = 0;
    virtual SQLSMALLINT data_type() = 0;
    virtual void fill_buffer(void *buff, size_t size) = 0;

    virtual std::string column_name();

private:
    std::string col_name_;
};

class RandomFeeder : public Feeder {
protected:
    RandomFeeder();
    std::mt19937_64 gen_;
};

// CONST
class ConstFeeder : public Feeder {
public:
    ConstFeeder(const std::string val);
    std::string get() override;
    SQLSMALLINT data_type();
    void fill_buffer(void *buf, size_t size);

private:
    const std::string value_;
};

// SEQ
class SeqFeeder : public Feeder {
public:
    SeqFeeder(std::atomic_long& start);
    std::string get() override;
    SQLSMALLINT data_type();
    void fill_buffer(void *buf, size_t size);

private:
    std::atomic_long& next_num_; // may concurrent access.
};

// IRAND
class IntegerRandFeeder : public RandomFeeder {
public:
    IntegerRandFeeder(long min, long max);
    std::string get() override;
    SQLSMALLINT data_type();
    void fill_buffer(void *buf, size_t size);

private:
    std::uniform_int_distribution<long> dist;
};

// DRAND
class DateRandFeeder : public RandomFeeder {
public:
    DateRandFeeder(long min_year, long max_year);
    std::string get() override;
    SQLSMALLINT data_type();
    void fill_buffer(void *buf, size_t size);

private:
    std::uniform_int_distribution<time_t> dist_time_;
};

// TMRAND
class TimeRandFeeder : public RandomFeeder {
public:
    TimeRandFeeder();
    std::string get();
    SQLSMALLINT data_type();
    void fill_buffer(void *buf, size_t size);

private:
    FastUniformIntDistribution dist_hour_;
    FastUniformIntDistribution dist_minute_;
    FastUniformIntDistribution dist_second_;
};

// TSRAND
class TimeStampRandFeeder : public RandomFeeder {
public:
    TimeStampRandFeeder();
    std::string get();
    SQLSMALLINT data_type();
    void fill_buffer(void *buf, size_t size);

private:
    std::uniform_int_distribution<time_t> dist_;
};

// CRAND
class CharRandFeeder : public RandomFeeder {
public:
    CharRandFeeder(size_t len);
    std::string get();
    SQLSMALLINT data_type();
    void fill_buffer(void *buf, size_t size);

private:
    size_t length_;
    std::string chars_;
    size_t chars_len_;
    FastUniformIntDistribution dist_;
};

// NRAND
class NumericRandFeeder : public RandomFeeder {
public:
    NumericRandFeeder(size_t precission, size_t scale);
    std::string get();
    SQLSMALLINT data_type();
    void fill_buffer(void *buf, size_t size);

private:
    size_t scale_div_;
    FastUniformIntDistribution dist_;
    std::ostringstream oss_;
};

// UTF8RAND
class UTF8RandFeeder : public RandomFeeder {
public:
    UTF8RandFeeder(size_t len);
    std::string get();
    SQLSMALLINT data_type();
    void fill_buffer(void *buf, size_t size);

    char* utf8_from_code(int cp, char* buf);

private:
    size_t len_;
    FastUniformIntDistribution dist_;
};

// GBKRAND
class GBKRandFeeder : public RandomFeeder {
public:
    GBKRandFeeder(size_t len);
    std::string get();
    SQLSMALLINT data_type();
    void fill_buffer(void *buf, size_t size);
    char* rand_one_gbk_char(char *buf);

private:
    size_t len_;
    FastUniformIntDistribution dist_;
};

// MapWorker is thread safe for parallel maping.
class MapWorker {
public:
    MapWorker(std::vector<std::unique_ptr<Feeder>>&& map_workers);
    size_t count() const;
    std::string map(size_t column_num) const;
    std::string column_name(size_t column_num) const;
    SQLSMALLINT type(size_t column_num) const;

    void fill_buffer(size_t col, void *buf, size_t size);

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
