#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <vector>
#include <chrono>
#include <algorithm>

#include "passthrough.h"
#include "process.h"
#include "opts.h"
#include "log.h"

//TODO:  Find out why std::find doesn't work (fuckton of compile errors)
//TODO:  Find out why need to explicitly qualify with ::find to use global find
//       (std::find is potential overload for find() w/o using namespace std;)
//

template <class It, class Predicate>
It find(It begin, It end, Predicate p) {
    while (begin != end) {
        if (p(*begin)) {
            break;
        }
        ++begin;
    }
    return begin;
}

bool process_live(const process& p) {
    using std::chrono::milliseconds;
    using std::chrono::duration_cast;
    auto now = process::clock::now();
    auto time = now - p.launchtime();
    return duration_cast<milliseconds>(time).count() > settings.timeout;
}

bool trycrack(char** program) {
    std::vector<process> procvec;
    for (int i = 0; i < settings.num_procs; ++i) {
        procvec.emplace_back(program, process::redir_in | process::redir_out);
        clog(1) << "Spawned process with pid " << procvec.back().pid() 
            << '\n';
    }
    auto time = std::chrono::milliseconds{settings.timeout};
    while (true) {
        auto w = wait();
        if (w.pid == -1) {
            if (errno == EINTR) {
                continue; //alarm signal - check process times
            }
            std::perror("wait");
            std::exit(1);
        }
        clog(1) << "Child exited with pid " << w.pid << '\n';
        auto pid_match = [w](const process& p) { return p.pid() == w.pid; };
        auto it = ::find(begin(procvec), end(procvec), pid_match);
        if (it == end(procvec)) {
            std::cerr << "unsupervised child process\n";
            continue;
        }
        process p{program, process::redir_in | process::redir_out};
        it->swap(p);
        clog(1) << "Spawned process with pid " << it->pid() << '\n';
        auto live = ::find(std::begin(procvec), std::end(procvec), 
                process_live);
        if (live != end(procvec)) {
            //viable process found *live!
            process& proc = *live;
            std::cout << "Found live process with pid " << proc.pid() << '\n';
            //kill all other processes
            for (auto& i: procvec) {
                if (&i != &proc) {
                    clog(1) << "Killing process with pid " << 
                            i.pid() << '\n';
                    i.kill();
                }
            }
            //connect the consoles
            passthrough(STDIN_FILENO, STDOUT_FILENO, proc.in(), proc.out());
        }
    }
    return true;
}

int main(int argc, char** argv) {
    process_args(argc, argv);
    trycrack(settings.child_cmd);
    return 0;
}

