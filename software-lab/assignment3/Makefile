all: list hash

GCC = gcc -Wall -ansi -pedantic -g

list:
	$(GCC) -c ./src/symtablelist.c -o ./src/symtablelist.o

hash:
	$(GCC) -c ./src/symtablehash.c -o ./src/symtablehash.o

clean:
	-rm -f *.exe ./src/*.o