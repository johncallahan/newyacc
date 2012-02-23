#
# Makefile for NewYacc counters
#
# You may have to change NYLIBDIR.
#

TARGETS		=	listrules listtrans

NEWYACC		=	newyacc
NYYACC		=	yacc
NYLEXFIX	=	nylexfix.lex
NYTABCFIX       =       nytabcfix.yacc
NYFLAGS		=	-Y $(NYYACC)
NYLIB		=	../../../src/libny/libny.a
CLEAN		=	core a.out *.bak *~ *.o .noblab \
			y.tab.c y.tab.h nytags.h lex.yy.c \
			headerlex.yy.c y.output ny.temp.y $(TARGETS)
RCLEAN		=	$(CLEAN)

all: $(TARGETS)

listrules: .noblab listrules.o y.tab.o
	$(CC) $(CFLAGS) -o listrules listrules.o y.tab.o $(NYLIB) -ll

listtrans: .noblab listtrans.o y.tab.o
	$(CC) $(CFLAGS) -o listtrans listtrans.o y.tab.o $(NYLIB) -ll

y.tab.c: newyacc.ny headerlex.yy.c
	$(NEWYACC) -f newyacc.ny $(NYFLAGS)
	$(NYTABCFIX) y.tab.c

headerlex.yy.c: newyacc.l
	lex newyacc.l
	$(NYLEXFIX) lex.yy.c
	sed -e s/yylex\(/headeryylex\(/ < lex.yy.c > headerlex.yy.c
	rm -f lex.yy.c

listrules.o: nytags.h
listtrans.o: nytags.h

.c.o:
	$(CC) $(CFLAGS) -c $*.c

nytags.h:	y.tab.c


.noblab:
	@echo ' '
	@echo 'The newyacc library is listed here as being ' $(NYLIB)
	@echo 'If this is a nonstandard location, then you will need to'
	@echo 'edit this Makefile script.'
	@echo ' '
	touch .noblab


sources:

print:

clean:
	@rm -rf $(CLEAN)

reallyclean:
	@rm -rf $(RCLEAN)

install:

