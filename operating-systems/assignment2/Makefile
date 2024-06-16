# Nikolaos Kontogeorgis - csd4655

FLAGS = -Wall -g -pthread -lrt

all: study

study: study.c
	@gcc $(FLAGS) $< -o $@
	@echo "Program compiled, ./study to run it"

clean:
	@rm -f study