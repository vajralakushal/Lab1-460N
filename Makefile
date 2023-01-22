all: compile

compile:
	gcc -std=c99 -g -o assemble assembler.c

clean:
	rm assemble