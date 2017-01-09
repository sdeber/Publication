CC = gcc
INC = -I../  

#LIBS = -lm -lsysmp
LIBS = -lm -lpthread
LIBDIR = ../
CFLAGS = $(INC) -g -O4 -L$(LIBDIR)

EXECUTABLES = commstime matrix threadrun tr_cache contextswitch cs_yield cs_yield_cache threadexec chantest speedup

OBJS = $(LIBDIR)cthread.o $(LIBDIR)scheduler.o $(LIBDIR)kernelthread.o $(LIBDIR)channel.o

all: $(OBJS) $(EXECUTABLES)

.c: $(OBJS) 
	$(CC) $(CFLAGS) -o $* $*.c $(OBJS) $(LIBS)

clean:
	rm $(EXECUTABLES); 

