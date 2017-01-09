CC = gcc
INC = -I../  

#LIBS = -lm -lsysmp
LIBS = -lm -lpthread
LIBDIR = ../
#CFLAGS = $(INC) -g -O4 -L$(LIBDIR)
CFLAGS = $(INC) -g -O0 -L$(LIBDIR)

EXECUTABLES = example example2 example3 example4 example5 example6 example7 example8 exampleChannel

OBJS = $(LIBDIR)cthread.o $(LIBDIR)scheduler.o $(LIBDIR)kernelthread.o $(LIBDIR)channel.o 

all: $(OBJS) $(EXECUTABLES)

.c: $(OBJS) 
	$(CC) $(CFLAGS) -o $* $*.c $(OBJS) $(LIBS)

clean:
	rm $(EXECUTABLES); 

