#ifndef OPTION_H
#define OPTION_H

#include "Common.h"

#include <string>
#include <map>

class Options
{
public:
    bool has_option(std::string name) const;
    std::string option_value(std::string name) const;
    void add_option(std::string name, std::string value = "");
    void set_option(std::string name, std::string value);
    void print() const;

private:
    std::map<std::string, std::string> ops_;
};
#endif
