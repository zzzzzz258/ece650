CC=gcc
CFLAGS=-ggdb3 -fPIC
MALLOC_VERSION=FF
WDIR=..

all: mymalloc_test diy_test

mymalloc_test: mymalloc_test.c
	$(CC) $(CFLAGS) -I$(WDIR) -L$(WDIR) -D$(MALLOC_VERSION) -Wl,-rpath=$(WDIR) -o $@ mymalloc_test.c -lmymalloc -lrt

diy_test: diy_test.c
	$(CC) $(CFLAGS) -I$(WDIR) -L$(WDIR) -D$(MALLOC_VERSION) -Wl,-rpath=$(WDIR) -o $@ diy_test.c -lmymalloc -lrt


clean:
	rm -f *~ *.o mymalloc_test diy_test

clobber:
	rm -f *~ *.o
