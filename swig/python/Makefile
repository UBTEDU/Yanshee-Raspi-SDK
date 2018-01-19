#Makefile
#ROOTDIR definition must be placed before include 

CFLAGS = -Wall -g -O0 -fPIC 
SOURCES = RobotApi.c RobotApi_wrap.c
#OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
OBJS = $(SOURCES:.c=.o)

DEPENDS = .depend

INCLUDE_PATH = -I. -I../../include/ -I../../utils/ -I/usr/include/python2.7 
CFLAGS += $(INCLUDE_PATH) 

SHLIB = _RobotApi.so

LIBS = $(SHLIB)
LDFLAGS += -shared -L../../libs/robotlogstr -lrobotlogstr -L../../libs/json -ljson -L../../utils/ -lrobotutils


all: $(SHLIB)
	
%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
	
$(SHLIB):$(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) $(LIBS) $(DEPENDS)
	rm -f *.py *.c
	
