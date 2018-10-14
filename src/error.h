#ifndef ERROR_H
#define ERROR_H

#include <sstream>
#include <iostream>

std::ostream& oss_output(std::ostream& os);

template <typename T, typename ...Args>
std::ostream& oss_output(std::ostream& os, T v, Args... args) {
    os << v;
    return oss_output(os, args...);
}

template<typename T, typename ...Args>
void odb_error(T t, Args... args) {
    std::ostringstream oss;
    oss_output(oss, t, args...);
    throw std::runtime_error(oss.str());
}

template<typename T, typename ...Args>
void odb_warning(T t, Args... args) {
    std::ostringstream oss;
    oss_output(oss, t, args...);
    std::cout << "WARNING:" << oss.str() << std::endl;
}

#endif
