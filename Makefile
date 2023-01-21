all: compile

compile:
	gcc -std=c99 -o assemble assembler.c

clean:
	rm assemble