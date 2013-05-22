OBJS = *.cpp.o *.c.o *.o *.so.*
SRCS = *.cpp *.c
HDRS = *.h
TMP_FILES = *.c~ *.h~ Makefile~ *.m~

# try to detect 32-bit / 64-bit architecture
# works on Linux only, though
NBITS := $(shell getconf LONG_BIT)
ifeq ($(NBITS),32)
        BIT_STRING = "32-bit target"
        LIB_PATH = /usr/local/lib64
else
        BIT_STRING = "64-bit target"
        LIB_PATH = /usr/local/lib
endif

# targets:

all: clean library test

library:
		$(CC) -c ./tinyrng/TinyRNG.c -fPIC -I./tinyrng
		$(CC) -shared -Wl,-soname,libtinyrng.so.1 -o libtinyrng.so.1 TinyRNG.o

test:
		$(CC) test_32bit.c -o test_32bit -I. -I./tinyrng libtinyrng.so.1 -lrt -lm
		$(CC) test_64bit.c -o test_64bit -I. -I./tinyrng libtinyrng.so.1 -lrt -lm

clean:
		$(RM) $(OBJS) $(TMP_FILES) test_32bit test_64bit libtinyrng.so.1

install:
		cp libtinyrng.so.1 $(LIB_path)/libtinyrng.so.1
