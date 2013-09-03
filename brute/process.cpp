#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <algorithm>
#include "process.h"
#include "log.h"

exitstatus::exitstatus(int s) : status(s) {}

bool exitstatus::running() const {
    return !(WIFEXITED(status) || WIFSIGNALED(status));
}

bool exitstatus::exited() const {
    return WIFEXITED(status);
}

bool exitstatus::terminated() const {
    return WIFSIGNALED(status);
}

int exitstatus::returnval() const {
    return WEXITSTATUS(status);
}

int exitstatus::termsig() const {
    return WTERMSIG(status);
}


void process::init(process::flags settings) {
    int inp[2]{-1, -1}, outp[2]{-1, -1}, errp[2]{-1, -1};
    if (settings & flags::redir_in) {
        if (pipe(inp) != 0) {
            perror("pipe");
        }
    }
    if (settings & flags::redir_out) {
        if (pipe(outp) != 0) {
            perror("pipe");
        }
    }
    if (settings & flags::redir_err) {
        if (pipe(errp) != 0) {
            perror("pipe");
        }
    }
    launch = clock::now();
    pid_m = fork();
    if (pid_m == -1) {
        perror("fork");
    }
    else if (pid_m == 0) {
        //child
        if (settings & flags::redir_in) {
            dup2(inp[0], STDIN_FILENO);
        }
        if (settings & flags::redir_out) {
            dup2(outp[1], STDOUT_FILENO);
        }
        if (settings & flags::redir_err) {
            dup2(errp[1], STDERR_FILENO);
        }
    }
    else {
        //parent
        in_fd = inp[1];
        close(inp[0]);
        out_fd = outp[0];
        close(outp[1]);
        err_fd = errp[0];
        close(errp[1]);
    }
}

process::process(char** args, process::flags f) {
    init(f);
    if (pid_m == 0) { //child
        execvp(args[0], args);
        perror("execvp");
        std::exit(1);
    }
}

process::process(process&& p) {
    swap(p); //be careful - destructor for p still gets called, even though
             //the object we're swapping in is invalid.
}

process::~process() {
    /* We guard this in if (pid()) {} because if we constructed another object
     * by moving *this, then we will be zeroed out.  If pid == 0 then this is
     * either the child process (in which the parent's fds should be closed
     * and we shouldn't worry about this) OR we move constructed an object from
     * *this, in which case it effectively zeroed-out the object and we do NOT
     * want to close standard input accidentally!
     */
    if (pid()) {
        clog(1) << "Destroying process handler for pid " << pid() << '\n';
        if (in_fd != -1) {
            close(in_fd);
            in_fd = -1;
        }
        if (out_fd != -1) {
            close(out_fd);
            out_fd = -1;
        }
        if (err_fd != -1) {
            close(err_fd);
            err_fd = -1;
        }
    }
}

void process::swap(process& p) {
    std::swap(in_fd, p.in_fd);
    std::swap(out_fd, p.out_fd);
    std::swap(err_fd, p.err_fd);
    std::swap(pid_m, p.pid_m);
    std::swap(launch, p.launch);
}

exitstatus process::wait() {
    int status;
    waitpid(pid_m, &status, 0);
    return exitstatus{status};
}

exitstatus process::wait_noblock() {
    int status;
    waitpid(pid_m, &status, WNOHANG);
    return exitstatus{status};
}

int process::in() {
    return in_fd;
}

int process::out() {
    return out_fd;
}

int process::err() {
    return err_fd;
}

pid_t process::pid() const {
    return pid_m;
}

process::time process::launchtime() const {
    return launch;
}

void process::term() {
    ::kill(pid_m, SIGTERM);
}

void process::kill() {
    ::kill(pid_m, SIGKILL);
}

wait_t wait() {
    int status;
    pid_t pid = wait(&status);
    return {pid, exitstatus{status}};
}
