#
# Makefile for newyacc
#
# AUTHOR:	J.Callahan
# DATE:		06/28/89
#
# Copyright (C) 1989 by University of Maryland Board of Regents
# and Jack Callahan <a private citizen>. All rights reserved.
# UNIX is a trademark of AT&T Bell Laboratories.
#
#

NEWYACC= newyacc
NYMEMUTILS= ../utils/memutils.o
INCLUDES= -I../../include
CFLAGS= -O -w

all: $(NEWYACC)

$(NEWYACC): y.tab.o main.o $(NYMEMUTILS)
	 $(CC) $(CFLAGS) -o $(NEWYACC) y.tab.o main.o $(NYMEMUTILS) -ll
	cp newyacc ../../bin
	cp ../nylexfix.lex ../../bin
	cp ../nytabcfix.byacc ../../bin
	cp ../nytabcfix.yacc ../../bin

headerlex.yy.c: newyacc.l
	$(LEX) newyacc.l
	sed -e s/yylex/headeryylex/  < lex.yy.c > headerlex.yy.c
	rm -f lex.yy.c

y.tab.o: y.tab.c headerlex.yy.c
	$(CC) $(CFLAGS) $(INCLUDES) -c y.tab.c

y.tab.c: newyacc.y
	$(YACC) $(YFLAGS) newyacc.y

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $*.c

sources:

print:

clean:
	@rm -rf $(CLEAN) headerlex.yy.c newyacc y.tab.c *.o *~ *.a

reallyclean:
	@rm -rf headerlex.yy.c $(RCLEAN) headerlex.yy.c newyacc y.tab.c *.o *.a *~

install:
