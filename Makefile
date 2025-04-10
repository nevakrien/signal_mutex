CC = gcc
CFLAG = -g3 -Wall

sig_semiphor.o: sig_semiphor.c sig_semiphor.h
	$(CC) $(CFLAG) -c sig_semiphor.c

test: main.c sig_semiphor.o
	$(CC) $(CFLAG) main.c sig_semiphor.o -otest