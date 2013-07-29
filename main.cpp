#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>

#include "process.h"

bool trycrack(process p) {
    //
}

struct settings_t {
    const char* progname = nullptr;
    char ** child_cmd = nullptr;
    const char* stdin_prepend = nullptr;
    bool verbose = false;
    unsigned long timeout = 5;
} settings;

const char * shortopts = "s:vht:";

const struct option longopts[] = {
    { "stdin", required_argument, NULL, 's' },
    { "help", no_argument, NULL, 'h' },
    { "verbose", no_argument, NULL, 'v' },
    { "timeout", required_argument, NULL, 't' },
    { NULL, no_argument, NULL, 0}
};


void help() {
    std::cerr << "help\n";
}

void process_args(int argc, char** argv) {
    settings.progname = argv[0];
    int idx, opt;
    while ((opt = getopt_long(argc, argv, shortopts, longopts, &idx)) != -1)  {
        switch (opt) {
        case 'h':
            help();
            exit(0);
        case 's':
            settings.stdin_prepend = optarg;
            break;
        case 'v':
            settings.verbose = true;
            break;
        case 't':
            {
                char * errptr;
                auto t = strtol(optarg, &errptr, 10);
                if (t <= 0 || optarg == errptr) {
                    std::cerr << "Invalid argument for timeout: " << optarg 
                              << '\n';
                    std::exit(1);
                }
                settings.timeout = t;
            }
        case '?':
            //error will be printed by getopt
            std::exit(1);
            break;
        default:
            assert(false);
            break;
        }
    }
    if (optind == argc) {
        std::cerr << "No child process command given.\n";
        exit(1);
    }
    settings.child_cmd = &(argv[optind]);
}

int main(int argc, char** argv) {
    process_args(argc, argv);
    process child(settings.child_cmd);
    child.wait();
    return 0;
}

