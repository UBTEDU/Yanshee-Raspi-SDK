#Makefile
#ROOTDIR definition must be placed before include 

CFLAGS = -Wall -g -O0 -fPIC

OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

DEPENDS = .depend

INCLUDE_PATH = -I. -I../../include/ -I../../utils/ -I/usr/include/python2.7 
CFLAGS +=  $(INCLUDE_PATH) 


SHLIB = _RobotApi.so

LIBS = $(SHLIB)
LDFLAGS += -shared -L../../libs/robotlogstr -lrobotlogstr -L../../libs/json -ljson -L../../utils/ -lrobotutils


all: $(SHLIB)
	

$(SHLIB):$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) $(LIBS) $(DEPENDS)
	rm -f *.py *.c *.h *.pyc
	
