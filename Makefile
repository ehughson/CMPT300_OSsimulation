CC = gcc
PROG = main
OBJS = main.o List.o

main: $(OBJS)
	$(CC) -o $(PROG) $(OBJS)

List.o: List.c List.h
	$(CC) -c List.c

main.o: main.c 
	$(CC) $(CFLAGS) -c main.c 

clean: 
	rm *.o
	rm main