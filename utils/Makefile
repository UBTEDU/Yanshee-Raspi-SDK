#Makefile
CFLAGS = -Wall -g -O0 -fPIC 


SOURCES = RobotMsg.c RobotMsgQueue.c

OBJS = $(SOURCES:.c=.o)
DEPENDS = .depend

INCLUDE_PATH = -I. -I../include
CFLAGS +=  $(INCLUDE_PATH)

SHLIB = librobotutils.so

LIBS = $(SHLIB)
LDFLAGS= -shared -L../libs/robotlogstr -lrobotlogstr -L../libs/json -ljson 


all:$(SHLIB)

	
ECHO:
	@echo $(SUBDIRS)	
	
$(SHLIB):$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

doxygen:
	@doxygen Dexygen

clean:
	rm -f $(OBJS) $(LIBS) $(DEPENDS)

	
