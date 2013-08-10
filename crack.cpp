#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>

#include "crack.h"
#include "passthrough.h"
#include "opts.h"

stdin_filler::stdin_filler(const char* file) {
    fd = open(file, O_RDONLY);
    if (fd == -1) {
        std::perror("open");
        std::exit(1);
    }
}

stdin_filler::~stdin_filler() {
    close(fd);
}

void stdin_filler::fill(process& p) {
    char buf[512];
    int numbytes;
    do {
        numbytes = read(fd, buf, 512);
        if (numbytes == -1) {
            perror("read");
            std::exit(1);
        }
        if (!writeall(p.in(), buf, numbytes)) {
            //it is easy to imagine this write failing - so if that happens
            //just kill the process, as it's not useful to us...
            if (settings.verbose) {
                std::cerr << "write failed to subprocess, killing...\n";
            }
            p.term();
            break;
        }
    } while (numbytes != 0);
    lseek(fd, 0, SEEK_SET); //return to beginning of file
}

