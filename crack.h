#ifndef CRACK_H_INC
#define CRACK_H_INC

#include "process.h"

class stdin_filler {
public:
    explicit stdin_filler(const char*);
    ~stdin_filler();
    void fill(process& p);
private:
    int fd;
};

#endif
