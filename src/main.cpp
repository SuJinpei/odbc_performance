#include "Runner.h"
#include <iostream>

int main(int argc, char *argv[]) {
    //// TEST
    //const char *targv[] = { "odb", "-d", "mariaDB:14d40", "-u", "root:trafodion", "-p", "123456:traf123", "-cp", "src=test_odb:tgt=test_odb:parallel=3:max=11000:Istats=2" };
    //const char *targv[] = { "odb", "-d", "12d11", "-u", "db__root", "-p", "traf123", "-l", "src=nofile:map=map2.txt:tgt=testi:parallel=2:max=100000000:Istats=2:pseudo" };
    //const char *targv[] = { "odb", "-d", "12d11", "-u", "db__root", "-p", "traf123", "-l", "src=nofile:map=map2.txt:tgt=testi:parallel=2:max=100000000:Istats=2:pseudo" };
    const char *targv[] = { "odb", "-d", "23d11", "-u", "trafodion", "-p", "traf123", "-l", "src=nofile:map=map2.txt:tgt=seabase.testts:parallel=1:max=10:Istats=2" };
    //const char *targv[] = { "odb", "-d", "oracle", "-u", "odb", "-p", "odb", "-l", "src=nofile:map=mapfile.txt:tgt=ORDER_LINE:Istats=2:parallel=1:max=100:rows=10" };
    //const char *targv[] = { "odb", "-d", "mariaDB", "-u", "root", "-p", "123456", "-l", "src=test.csv:map=mapfile.txt:tgt=test_odb:parallel=3:max=11000" };
    //const char *targv[] = { "odb", "-d", "12d11", "-u", "db__root", "-p", "traf123", "-I" };
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