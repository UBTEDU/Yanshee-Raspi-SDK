#Makefile
#ROOTDIR definition must be placed before include 

#CC = arm-linux-gnueabihf-gcc
#include ../config.mk

CFLAGS = -Wall -g -O0 -fPIC 

OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
DEPENDS = .depend

INCLUDE_PATH = -I. -I../../include/ -I../utils/ -I/usr/include/python2.7 
CFLAGS +=  $(INCLUDE_PATH) 


SHLIB = _RobotApi.so

LIBS = $(SHLIB)
LDFLAGS += -shared -L../libs/robotlogstr -lrobotlogstr -L../libs/json -ljson -L../utils/ -lrobotmsg


all:swig $(SHLIB)
	
ECHO:
	@echo $(SUBDIRS)	
	
swig:
	@cp ../../sdk/RobotApi.c ./
	swig -python RobotApi.i

$(SHLIB):$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) $(LIBS) $(DEPENDS)

	
