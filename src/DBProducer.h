#ifndef D_B_PRODUCER_H
#define D_B_PRODUCER_H

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <assert.h>

#include "DataProducer.h"
#include "DBConnection.h"

class DBProducer : public DataProducer
{
public:
    DBProducer(Options& options, DBConnection& conn);

    // Inherited via DataProducer
    virtual DataBuffer produce_data(DataBuffer && data) override;

private:
    DBConnection& db_conn;
};
#endif
