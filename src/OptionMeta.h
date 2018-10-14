#ifndef OPTION_META_H
#define OPTION_META_H

#include <string>

struct OptionMeta
{
    bool is_value_option;
    bool is_operation;        // operation will be add to execution table.
    bool is_optional;
    std::string help_info;
    std::string default_value;
};
#endif
