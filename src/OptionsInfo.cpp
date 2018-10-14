#include "Common.h"
#include "OptionsInfo.h"

#include <sstream>
#include <iostream>

void OptionsInfo::add_option_info(const std::string& name, const std::string& help_info, bool is_value_option, bool is_operation, bool is_optional, std::string default_value)
{
    OptionMeta option_meta{ is_value_option, is_operation, is_optional, help_info, default_value };
    add_option_info(name, option_meta);
}

void OptionsInfo::set_option_info(const std::string & name, const std::string & help_info, bool is_value_option, bool is_operation, bool is_optional, std::string default_value)
{
    OptionMeta option_meta{ is_value_option, is_operation, is_optional, help_info, default_value };
    set_option_info(name, option_meta);
}

void OptionsInfo::add_option_info(const std::string & name, OptionMeta option_meta)
{
    auto res = options_info_.insert({ name, option_meta });
    if (!res.second)
        odb_error("duplicated option '", name, "'");
}

void OptionsInfo::set_option_info(const std::string & name, OptionMeta option_meta)
{
    options_info_[name] = option_meta;
}

bool OptionsInfo::contains_option(const std::string& name) const
{
    return options_info_.find(name) != options_info_.end();
}

const OptionMeta& OptionsInfo::option(const std::string& name) const
{
    return options_info_.at(name);
}

const std::map<std::string, OptionMeta> OptionsInfo::options_info() const
{
    return options_info_;
}
