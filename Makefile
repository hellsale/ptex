CC = gcc
CFLAGS = -Wall -g
ALL = ptex man
ptex: ptex.o parser.o
ptex.o: ptex.c ptex.h
parser.o: parser.c ptex.h
man: ptex.1
	groff -man -Tlatin1 ptex.1 > man
all: $(ALL)
clean:
	rm -f *.o $(ALL) foo bar core
