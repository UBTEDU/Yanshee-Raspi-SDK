#Makefile
#ROOTDIR definition must be placed before include 

#CC = arm-linux-gnueabihf-gcc
#include ../config.mk

CFLAGS = -Wall -g -O0 -fPIC 


SOURCES = RobotApi.c 

OBJS = $(SOURCES:.c=.o)
DEPENDS = .depend

INCLUDE_PATH = -I. -I../include -I../utils/
CFLAGS +=  $(INCLUDE_PATH) 


SHLIB = librobot.so

LIBS = $(SHLIB)
LDFLAGS += -shared -L../libs/robotlogstr -lrobotlogstr -L../libs/json -ljson -L../utils/ -lrobotutils


all:$(SHLIB) doxygen

%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
	
$(SHLIB):$(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

doxygen:
	@doxygen Doxyfile

clean:
	@rm -f $(OBJS) $(LIBS) $(DEPENDS)
	@rm -rf YANSHEE_USER_MANUAL
	@rm -rf latex 

	
