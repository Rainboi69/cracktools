#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <vector>
#include <chrono>
#include <algorithm>

#include "process.h"
#include "opts.h"

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
    auto time = process::clock::now() - p.launchtime();
    return duration_cast<milliseconds>(time).count() > settings.timeout;
}

bool trycrack(char** program) {
    std::vector<process> procvec;
    for (int i = 0; i < settings.num_procs; ++i) {
        procvec.emplace_back(program, process::redir_in | process::redir_out);
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
        auto it = ::find(begin(procvec), end(procvec),
                [w](const process& p) { return p.pid() == w.pid; });
        if (it == end(procvec)) {
            std::cerr << "unsupervised child process\n";
            continue;
        }
        process p{program, process::redir_in | process::redir_out};
        it->swap(p);
        auto live = ::find(begin(procvec), end(procvec), process_live);
        if (live != end(procvec)) {
            //viable process found *live!
        }
    }
    return true;
}

int main(int argc, char** argv) {
    process_args(argc, argv);
    return 0;
}

