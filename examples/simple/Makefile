#
# Makefile for "simple" NewYacc example
#
# You may have to change NYLIBDIR.
#

TARGETS		=	simple

NEWYACC		=	newyacc
NYYACC		=	yacc
NYLEXFIX	=	nylexfix.lex
NYTABCFIX       =       nytabcfix.yacc
NYFLAGS		=	-Y $(NYYACC)
NYLIB		=	../../src/libny/libny.a
CLEAN		=	core a.out *.bak *~ *.o .noblab \
			y.tab.c y.tab.h nytags.h lex.yy.c \
			headerlex.yy.c y.output ny.temp.y $(TARGETS)
RCLEAN		=	$(CLEAN)

all: $(TARGETS)

simple: .noblab main.o y.tab.o
	$(CC) $(CFLAGS) -o simple main.o y.tab.o $(NYLIB) -ll

y.tab.c: simple.ny lex.yy.c
	$(NEWYACC) -f simple.ny $(NYFLAGS)
	$(NYTABCFIX) y.tab.c

lex.yy.c: simple.l
	lex simple.l
	$(NYLEXFIX) lex.yy.c

main.o: nytags.h

nytags.h:	y.tab.c

.c.o:
	$(CC) $(CFLAGS) -c $*.c

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
