#include <errno.h>
#include <cstdio>
#include <cstdlib>
#include <sys/select.h>
#include <unistd.h>
#include "passthrough.h"

bool writeall(int fd, const char* buf, size_t num) {
    while (num > 0) {
        int i = write(fd, buf, num);
        if (i == -1) {
            if (errno == EINTR) {
                continue;
            }
            return false;
        }
        num -= i;
    }
    return num == 0;
}

bool manual_splice(int in, int out) {
    char buf[512];
    int numbytes = read(in, buf, sizeof(buf));
    if (numbytes == 0) {
        //EOF or closed socket...
        return false;
    }
    if (numbytes == -1) {
        std::perror("read");
        std::exit(1);
    }
    if (!writeall(out, buf, numbytes)) {
        std::perror("write");
        std::exit(1);
    }
    return true;
}
    

void passthrough(int pin, int pout, int cin, int cout) {
    fd_set rfds, fds;
    char buf[1024];
    int nfds = ((pin > cout) ? pin : cout) + 1;
    FD_ZERO(&fds);
    FD_SET(pin, &fds);
    FD_SET(cout, &fds);
    while (true) {
        rfds = fds;
        int ret = select(nfds, &rfds, NULL, NULL, NULL);
        if (ret == -1) {
            perror("select");
            std::exit(1);
        }
        if (FD_ISSET(pin, &rfds)) {
            if (!manual_splice(pin, cin)) {
                return;
            }
        }
        if (FD_ISSET(cout, &rfds)) {
            if (!manual_splice(cout, pout)) {
                return;
            }
        }
    }
}

