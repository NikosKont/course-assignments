all: parser

parser: scanner.c parser.c datastructs.c quads.c tcode.c alphavm.c
	@gcc $^ -o $@ -lm

parser.c: parser.y
	@bison -d -v  $^

scanner.c: scanner.l
	@flex $^

clean:
	@rm -f al parser scanner.c parser.c parser.h 
