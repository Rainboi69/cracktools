#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include "process.h"

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
    pid = fork();
    if (pid == -1) {
        perror("fork");
    }
    else if (pid == 0) {
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
        in = inp[1];
        out = outp[0];
        err = errp[0];
    }
}

process::process(char** args, process::flags f) {
    init(f);
    if (pid == 0) { //child
        execvp(args[0], args);
        perror("execvp");
        std::exit(1);
    }
}

static int wait_return(int status) {
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status)) {
        return -WTERMSIG(status);
    }
    assert(false);
    return 0;
}

int process::wait() {
    int status;
    waitpid(pid, &status, 0);
    return wait_return(status);
}

bool process::wait_noblock(int* out) {
    int status;
    bool ret = waitpid(pid, &status, WNOHANG) == pid;
    if (out) *out = wait_return(status);
    return ret;
}
