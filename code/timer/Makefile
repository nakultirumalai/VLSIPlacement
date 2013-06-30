OBJS = bison.o flex.o main.o 
CC   = g++
CFLAGS = -g -Wall -ansi -pedantic

myTimer: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o myTimer -lfl

flex.o: lex.yy.c 
	$(CC) $(CFLAGS) -c lex.yy.c -o flex.o

lex.yy.c: libParser.lxx
	flex libParser.lxx 

bison.o: libParser.tab.cxx
	$(CC) $(CFLAGS) -c libParser.tab.cxx -o bison.o

libParser.tab.cxx: libParser.yxx
	bison -d -v libParser.yxx

main.o: main.C
	$(CC) $(CFLAGS) -c main.C -o main.o

clean: 
	rm -f *.o *.tab.* myTimer *.yy.* *.output
