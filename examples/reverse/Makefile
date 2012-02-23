#
# Makefile for trivlist NewYacc example
#
# You may have to change NYLIBDIR.
#

TARGETS		=	reverse

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

reverse: 	.noblab main.o y.tab.o
	$(CC) $(CFLAGS) -o reverse main.o y.tab.o $(NYLIB) -ll

y.tab.c: 	demo.ny lex.yy.c
	$(NEWYACC) -f demo.ny $(NYFLAGS)
	$(NYTABCFIX) y.tab.c

lex.yy.c: 	demo.l
	lex demo.l
	$(NYLEXFIX) lex.yy.c

main.o: 	nytags.h

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
