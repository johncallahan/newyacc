#
# Makefile for libny.a
#

CFLAGS = -O2 -w
OBJS = newyaccA.o newyaccB.o newyaccC.o newyaccU.o nylex.o

INCLUDES= -I ../../include
NYLIB= libny.a
NYUTILS= ../utils/utils.o
NYMEMUTILS= ../utils/memutils.o

all = $(NYLIB)

$(NYLIB): $(OBJS) $(NYUTILS) $(NYMEMUTILS)
	@rm -f $(NYLIB)
	@ar q $(NYLIB) $(OBJS) $(NYUTILS) $(NYMEMUTILS)
	ranlib $(NYLIB)
	cp $(NYLIB) ../../lib

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $*.c

sources:

print:

clean:
	@rm -rf $(OBJS) $(CLEAN) libny.a *.o *~

reallyclean:
	@rm -rf $(RCLEAN) libny.a *.o *~

install:
