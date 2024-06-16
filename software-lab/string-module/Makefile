CC = gcc
CFLAGS = -Wall -ansi -pedantic -g

all: mystring_ars.o mystring_ptrs.o

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f *.exe ./src/*.o