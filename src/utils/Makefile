#
# Makefile for utility object files
#
INCLUDES= -I../../include

all: memutils.o utils.o

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $*.c

sources:

print:

clean:
	@rm -rf *.o core a.out $(CLEAN)

reallyclean:
	@rm -rf $(RCLEAN)

install:
