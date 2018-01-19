#Makefile
CFLAGS = -Wall -ggdb -g3 -fPIC

SOURCES = RobotMsg.c RobotMsgQueue.c

OBJS = $(SOURCES:.c=.o)
DEPENDS = .depend

INCLUDE_PATH = -I. -I../include
CFLAGS +=  $(INCLUDE_PATH) 
LDFLAGS = -shared

SHLIB = librobotutils.so
LIBS = $(SHLIB)

all: $(SHLIB)
	
(OBJS):
	$(CC) $(CFLAGS) -c -o $@ $(OBJS) -lm

$(SHLIB): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)


clean:
	rm -f $(OBJS) $(LIBS) $(DEPENDS)

	
