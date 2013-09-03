#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <vector>
#include <chrono>
#include <algorithm>
#include <signal.h>

#include "passthrough.h"
#include "process.h"
#include "opts.h"
#include "log.h"
#include "crack.h"

void handle_alarm(int signo) {
    clog(1) << "Caught SIGALRM\n";
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
    stdin_filler filler{settings.stdin_prepend};
    for (int i = 0; i < settings.num_procs; ++i) {
        procvec.emplace_back(program, process::redir_in | process::redir_out);
        clog(1) << "Spawned process with pid " << procvec.back().pid() 
            << '\n';
        filler.fill(procvec.back());
    }
    auto time = std::chrono::milliseconds{settings.timeout};
    while (true) {
        alarm(2); //lets us wake up if we're stuck waiting for too long
        auto w = wait();
        if (w.pid == -1) {
            if (errno == EINTR) {
                //alarm signal most likely - check process times
            }
            else {
                std::perror("wait");
                std::exit(1);
            }
        }
        else {
            clog(1) << "Child exited with pid " << w.pid << '\n';
            auto pid_match = [w](const process& p) { return p.pid() == w.pid; };
            auto it = std::find_if(begin(procvec), end(procvec), pid_match);
            if (it == end(procvec)) {
                std::cerr << "unsupervised child process\n";
                continue;
            }
            process p{program, process::redir_in | process::redir_out};
            filler.fill(p);
            it->swap(p);
            clog(1) << "Spawned process with pid " << it->pid() << '\n';
        }
        //check process exit times
        auto live = std::find_if(std::begin(procvec), std::end(procvec), 
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
            break;
        }
    }
    return true;
}

int main(int argc, char** argv) {
    process_args(argc, argv);
    //ignore SIGPIPE - handle errors at write()
    struct sigaction sa_pipe{0};
    sa_pipe.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa_pipe, NULL);
    //set alarm handler
    struct sigaction sa_alarm{0};
    sa_alarm.sa_handler = handle_alarm;
    sigaction(SIGALRM, &sa_alarm, NULL);

    trycrack(settings.child_cmd);
    return 0;
}

