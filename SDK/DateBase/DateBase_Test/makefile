#!/usr/bin/make -f

#enable this macro only when you want to build it singly
#选择编译器的种类
CROSS_COMPILE_PREFIX=

CC=$(CROSS_COMPILE_PREFIX)gcc
CPP=$(CROSS_COMPILE_PREFIX)g++
RM=rm -rfv
INSTALL=install -c -m 777

#CFLAGS+= -O2 
CFLAGS+= -g
#CFLAGS+= -D_POSIX_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -DMEMWATCH -DMW_STDIO
CFLAGS+= -Wall -W -Wpointer-arith -Wcast-qual
LDFLAGS+= -lDateBase
#所需要的库
#LIBPATH+= -L../../../../bin/lib -Wl,-rpath,./lib
#LIBPATH+=-L../../lib_35xxD-static
LIBPATH+=-L../../lib_35xxD-static
#表示连接路径
INCLUDES+=-I ../
#INCLUDES+=-I../ -I ../../../include/ -I ../../PUB/ -I ../../STR/ -I../../PAR/  -I../../SnapShot/
DFLAGS+= -g -DDEBUG

#目标
BIN=AVL_test

OBJECTS=$(patsubst %.c, %.o, $(wildcard *.c))
OBJECTS+=$(patsubst %.cpp, %.o, $(wildcard *.cpp))

.PHONY:clean test

$(BIN):$(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBPATH) $(CFLAGS)

%.o:%.cpp
	$(CPP) -c $^ -o $@ $(CFLAGS) $(INCLUDES) 
%.o:%.cpp
	$(CC) -c $^ -o $@ $(CFLAGS) $(INCLUDES)

clean:
	$(RM) ${OBJECTS} ${BIN} 

 
