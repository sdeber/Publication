CC = gcc

OUTPUT = cas_benchmark.out cas2_benchmark.out tas_benchmark.out fetchandadd_benchmark.out

all : $(OUTPUT)
$(OUTPUT) : %.out : %.c
	$(CC) -lpthread -o $@ $<
clean : 
	rm *.out