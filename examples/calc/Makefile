#
# Makefile for calculator NewYacc example
#
# You may have to change NYLIBDIR.
#

TARGETS		=	calc

NEWYACC		=	../../bin/newyacc
NYYACC		=	yacc
NYLEXFIX	=	../../bin/nylexfix.lex
NYTABCFIX       =       ../../bin/nytabcfix.byacc
NYFLAGS		=	-Y $(NYYACC)
NYLIB		=	../../src/libny/libny.a
CLEAN		=	core a.out *.bak *~ *.o .noblab \
			y.tab.c y.tab.h nytags.h lex.yy.c \
			headerlex.yy.c y.output ny.temp.y $(TARGETS)
RCLEAN		=	$(CLEAN)

all: $(TARGETS)

calc: .noblab main.o y.tab.o
	$(CC) $(CFLAGS) -o calc main.o y.tab.o $(NYLIB)

y.tab.c: calc.ny
	$(NEWYACC) -f calc.ny $(NYFLAGS)
	$(NYTABCFIX) y.tab.c

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
	@rm -rf $(CLEAN) calc ny.temp.y nytags.h y.output y.tab.c y.tab.h

reallyclean:
	@rm -rf $(RCLEAN) calc ny.temp.y nytags.h y.output y.tab.c y.tab.h

install:


