#include <stdlib.h>
#include <stdio.h>

int main() {
    char buf[256];
    gets(buf);
    printf("Hello, %s\n", buf);
    return 0;
}

