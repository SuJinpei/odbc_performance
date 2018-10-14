#ifndef _COLUMNMETA_H
#define _COLUMNMETA_H

#include <sqlext.h>
#include <string>

struct ColumnMeta
{
    SQLSMALLINT data_type = 0;
    SQLSMALLINT decimal_digits = 0;
    SQLSMALLINT nullable = 0;
    SQLLEN display_size = 0;
    SQLULEN column_size = 0;
    std::string column_name;
};

#endif
