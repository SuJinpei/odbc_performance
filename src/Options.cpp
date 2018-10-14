#include "Options.h"

#include <iostream>

bool Options::has_option(std::string name) const
{
    return ops_.find(name) != ops_.end();
}

std::string Options::option_value(std::string name) const
{
    return ops_.at(name);
}

void Options::add_option(std::string name, std::string value)
{
    if (!(ops_.insert({ name, value })).second) {
        odb_error("option '", name, "' duplicated");
    }
}

void Options::set_option(std::string name, std::string value)
{
    ops_[name] = value;
}

void Options::print() const
{
    std::cout << "Options" << std::endl;
    std::cout << "============================" << std::endl;
    for (auto opt : ops_) {
        std::cout << opt.first << ":" << opt.second << std::endl;
    }
}
