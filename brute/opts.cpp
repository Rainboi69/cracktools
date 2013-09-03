#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>

#include "opts.h"

settings_t settings; //allocate space in this translation unit

const char * shortopts = "s:v::ht:n:";

const option longopts[] = {
    { "stdin", required_argument, NULL, 's' },
    { "help", no_argument, NULL, 'h' },
    { "verbose", optional_argument, NULL, 'v' },
    { "timeout", required_argument, NULL, 't' },
    { "numprocs", required_argument, NULL, 'n' },
    { NULL, no_argument, NULL, 0}
};

void help() {
    std::cerr << "Usage: "
        << settings.progname << " [OPTION]... -- [CMD]\n"
        << "Attempt to brute force [CMD]\n"
        "\n"
        "Mandatory arguments for long options are mandatory for short options"
                " too.\n"
        "   -s, --stdin=FILE   Prepend the contents of FILE to each "
                "subprocess'\n"
        "                      standard input stream.\n"
        "   -h, --help         Display this message.\n"
        "   -v, --verbose(=N)  Enable verbose logging at level N (N=1 by "
                "default).\n"
        "   -t, --timeout=T    Consider a process live after it has survived"
                " for T\n"
        "                      ms without crashing.\n"
        "   -n, --numprocs=N   Spawn N subprocesses at a time.\n"
        "\n"
        "Report bugs or suggest improvements at "
                "<https://github.com/rbmj/cracktools>\n"
        "";
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
            if (optarg) {
                char * errptr;
                auto n = strtol(optarg, &errptr, 10);
                if (n <= 0 || optarg == errptr) {
                    std::cerr << "Invalid argument for verbose: " << optarg 
                              << '\n';
                    std::exit(1);
                }
                settings.verbose = n;
            }
            else {
                settings.verbose = 1;
            }
            break;
        case 'n':
            {
                char * errptr;
                auto n = strtol(optarg, &errptr, 10);
                if (n <= 0 || optarg == errptr) {
                    std::cerr << "Invalid argument for timeout: " << optarg 
                              << '\n';
                    std::exit(1);
                }
                settings.num_procs = n;
            }
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
            break;
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

