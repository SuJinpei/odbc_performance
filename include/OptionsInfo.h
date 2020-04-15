#ifndef OPTIONS_INFO_H
#define OPTIONS_INFO_H

#include "OptionMeta.h"

#include <map>
#include <string>

class OptionsInfo
{
public:
    void add_option_info(const std::string& name, const std::string& help_info, bool is_value_option = false, bool is_operation = false, bool is_optional = true, std::string default_value = "");
    void set_option_info(const std::string& name, const std::string& help_info, bool is_value_option = false, bool is_operation = false, bool is_optional = true, std::string default_value = "");
    void add_option_info(const std::string& name, OptionMeta option_meta);
    void set_option_info(const std::string& name, OptionMeta option_meta);
    bool contains_option(const std::string& name) const;

    const OptionMeta& option(const std::string& name) const;
    const std::map<std::string, OptionMeta> options_info() const;

private:
    std::map<std::string, OptionMeta> options_info_;
};
#endif
