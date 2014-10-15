CC = gcc
DFLAGS =
CFLAGS = -Wall $(DFLAGS)
LFLAGS = -lpthread -Wall $(DFLAGS)
objects = pool_impl.o
testobj = test_pool.o $(objects)

lib : libpool.a
test : test_pool

libpool.a : $(objects)
	ar rcs libpool.a pool_impl.o

test_pool : $(testobj)
	$(CC) $(LFLAGS) $(testobj) -o test_pool

test_pool.o : test_pool.c pool.h
pool_impl.o : pool_impl.c pool.h

.PHONY : clean
clean : 
	-rm test_pool $(testobj)
