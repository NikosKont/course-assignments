all: sudoku.exe

GCC = gcc -Wall -ansi -pedantic -g

sudoku.exe: src/sudoku.c src/sudoku.h src/grid.o 
	$(GCC) $< src/grid.o -o $@

src/grid.o: src/grid.c src/grid.h
	$(GCC) -c $< -o $@

clean:
	-rm -f *.exe src/*.o