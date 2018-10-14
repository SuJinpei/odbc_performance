#include "Runner.h"
#include <iostream>

int main(int argc, char *argv[]) {
    //// TEST
    //const char *targv[] = { "odb", "-d", "mariaDB:14d40", "-u", "root:trafodion", "-p", "123456:traf123", "-cp", "src=test_odb:tgt=test_odb:parallel=3:max=11000:Istats=2" };
    const char *targv[] = { "odb", "-d", "12d11", "-u", "db__root", "-p", "traf123", "-l", "src=nofile:map=mapfile.txt:tgt=ORDER_LINE:parallel=1:rows=10:max=100:Istats=2" };
    //const char *targv[] = { "odb", "-d", "oracle", "-u", "odb", "-p", "odb", "-l", "src=nofile:map=mapfile.txt:tgt=ORDER_LINE:parallel=10:max=1100000:Istats=2:rows=1000:pseudo" };
    //const char *targv[] = { "odb", "-d", "mariaDB", "-u", "root", "-p", "123456", "-l", "src=test.csv:map=mapfile.txt:tgt=test_odb:parallel=3:max=11000" };
    //const char *targv[] = { "odb", "-d", "14d40", "-u", "trafodion", "-p", "traf123", "-I" };
    //const char *targv[] = { "odb", "-d", "mariaDB", "-u", "root", "-p", "123456", "-I" };
    //const char *targv[] = { "odb", "-d", "oracle", "-u", "odb", "-p", "odb", "-I" };
    
    //try {
         Runner(sizeof(targv)/sizeof(const char*), (char **)targv).run();
    //}
    //catch (std::exception& e) {
    //    std::cout << e.what() << std::endl;
    //}
    //catch (...) {
    //    std::cout << "unknown exception" << std::endl;
    //}

    return 0;
}