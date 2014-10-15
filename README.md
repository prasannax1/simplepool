simplepool
==========

Simple posix thread pooling

This is a library I made as a practice exercise to study pthreads.

Usage:

1. Copy pool.h & pool\_impl.c to source dir
2. #include "pool.h"
3. call new\_thread\_pool() with number of threads as arg
4. do start\_task with run function
5. do destroy\_pool on exit
6. make sure pool\_impl.o is built and linked to your executable
7. use -lpthread option

Works best if you create fixed number of threads that stick around for a
long time, like some sort of server
