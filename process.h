#ifndef PROCESS_H_INC
#define PROCESS_H_INC

#include <unistd.h>
#include <cstdlib>


class process {
public:
    enum flags {
        null = 0,
        redir_in = 1 << 0,
        redir_out = 1 << 1,
        redir_err = 1 << 2
    };
    process(char**, flags = null);
    template <class Callable>
    process(Callable c, flags f = null) {
        init(f);
        if (pid == 0) {
            c();
            std::exit(0);
        }
    }
    int wait();
    bool wait_noblock(int*);
private:
    void init(flags);
    int in;
    int out;
    int err;
    pid_t pid;
};

#endif
