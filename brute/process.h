#ifndef PROCESS_H_INC
#define PROCESS_H_INC

#include <unistd.h>
#include <cstdlib>
#include <chrono>

class exitstatus {
public:
    explicit exitstatus(int);
    bool running() const;
    bool exited() const;
    bool terminated() const;
    int returnval() const;
    int termsig() const;
private:
    int status;
};

class process {
public:
    typedef std::chrono::system_clock clock;
    typedef clock::time_point time;
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
        if (pid_m == 0) {
            c();
            std::exit(0);
        }
    }
    process(process&&);
    ~process();
    exitstatus wait();
    exitstatus wait_noblock();
    int in();
    int out();
    int err();
    pid_t pid() const;
    void term();
    void kill();
    void swap(process&);
    time launchtime() const;
private:
    void init(flags);
    int in_fd;
    int out_fd;
    int err_fd;
    pid_t pid_m;
    time launch;
};

//allow enum to be or'd with strong typing
static inline process::flags operator|(process::flags a, process::flags b) {
    return (process::flags)((int)a | (int)b);
}

//nicer wrapper for wait
struct wait_t {
    pid_t pid;
    exitstatus status;
};

wait_t wait();

#endif
