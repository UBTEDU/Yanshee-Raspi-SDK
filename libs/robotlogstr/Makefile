#Makefile
CFLAGS = -Wall -ggdb -g3 -fPIC

SOURCES = robotlogstr.c

OBJS = $(SOURCES:.c=.o)
DEPENDS = .depend

INCLUDE_PATH = -I. -I../../include
CFLAGS +=  $(INCLUDE_PATH) 

ARFLAGS = -rv

SHLIB = librobotlogstr.so
LIBS = $(SHLIB)

LDFLAGS += -shared -fPIC

all: $(SHLIB)
	
#$(SHLIB):$(OBJS)
#	$(CC)  $(LDFLAGS) -Wall -g -O0 -o $@ $(OBJS) -lm
%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(SHLIB): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)


clean:
	rm -f $(OBJS) $(LIBS) $(DEPENDS)

	
