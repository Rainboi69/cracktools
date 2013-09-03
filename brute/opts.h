#ifndef OPTS_H_INC
#define OPTS_H_INC

extern struct settings_t {
    const char* progname = nullptr;
    char ** child_cmd = nullptr;
    const char* stdin_prepend = nullptr;
    unsigned verbose = 0;
    unsigned long timeout = 5;
    unsigned long num_procs = 5;
} settings;

void help();
void process_args(int, char**);

#endif

