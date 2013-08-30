#ifndef LOG_H_INC
#define LOG_H_INC

#include <iostream>

class log_t {
private:
    bool print;
    std::ostream& str;
    log_t(bool p) : print(p), str(std::cout) {}
public:
    log_t(const log_t& l) = default;
    static log_t get(unsigned);
    template <class T>
    log_t& operator<<(const T& t) {
        if (print) {
            str << t;
        }
        return *this;
    }
};

log_t clog(unsigned);

#endif
