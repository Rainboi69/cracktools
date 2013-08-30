#include <iostream>
#include <string>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int random = open("/dev/urandom", O_RDONLY);
    int i;
    read(random, &i, sizeof(i));
    close(random);
    i %= 100;
    if (i == 12) {
        std::cout << "FOOBAR!" << std::endl;
        std::cerr << "BAZ!" << std::endl;
        std::string str;
        while (std::getline(std::cin, str).good()) {
            system(str.c_str());
        }
    }
    return 0;
}

