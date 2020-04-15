#ifndef INTERACTIVE_QUERY_H
#define INTERACTIVE_QUERY_H

#include "Job.h"
#include "DBConnection.h"

class InteractiveQueryJob : public Job
{
public:
    InteractiveQueryJob();

    void run();

    // Inherited via Job
    virtual std::string name() const override;

protected:
    std::ostream& prompt(std::ostream& os);

private:
    void show_table_info(std::string table_name);

    std::string prompt_ = "SQL> ";
    DBConnection conn_;
};
#endif
