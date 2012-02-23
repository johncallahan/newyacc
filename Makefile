#
# Root makefile for newyacc system
# version 1.0
#

VERSION	=	1.0

# >>> INSTALLATION <<<
#
# Edit the Makefile in this directory, and look for the definitions of
# DESTBINS, DESTSCRIPTS, DESTLIBS, DESTAUXS, and INSTDIR.  Change them
# to be whatever source, binary, library directories into which you want
# newyacc installed as follows (see below for list of installed
# binaries, scripts, libraries, and man pages):
#
#	DESTBINS	where executables are installed
#	DESTSCRIPTS	where executable shell scripts are installed
#	DESTLIBS	where libraries are installed
#	DESTMAN		where manual pages are installed
#	INSTDIR         where this distribution resides
#
# To install NewYacc, do the following:
#
# make		makes newyacc and libny.a
# make install	installs newyacc, libny.a, and the fix scripts
#			in all the right places
# make examples	constructs examples in the examples subdirectory
#
# It can't hurt to use make's -n option first, just to make sure the
# installation will work as you think it should.  After everything is
# made, you can do "make clean" for a recursive cleansing of all
# directories.
#
# The following is a list of binaries, scripts, man files, and
# libraries installed after a "make install":
#
#  newyacc		executable installed in DESTBIN directory
#  libny.a		library installed in DESTLIB directory
#  nylexfix.lex		shell script installed in DESTSCRIPT directory
#				used to alter lex generated lex.yy.c
#				files for use with NewYacc
#  nytabcfix.yacc	shell script installed in DESTSCRIPT directory
#				used to alter yacc generated y.tab.c
#				files for use with NewYacc
#  nytabcfix.byacc	shell script installed in DESTSCRIPT directory
#				used to alter Berkeley yacc generated
#				y.tab.c files for use with NewYacc
#  newyacc.1		man page installed in DESTMAN directory
#
# vvvvvvvvvvvvvvv EDIT MAKEFILE PARAMETERS BELOW vvvvvvvvvvvvvvv
#
DESTBINS	=	/usr/local/bin
DESTSCRIPTS	=	/usr/local/bin
DESTLIBS	=	/usr/local/lib
DESTMAN		=	/usr/local/man
INSTDIR		=	.
#
# ^^^^^^^^^^^^^^^ EDIT MAKEFILE PARAMETERS ABOVE ^^^^^^^^^^^^^^^
#

INCLUDEDIR	=	$(INSTDIR)/include
SRCDIR		=	$(INSTDIR)/src
MANDIR		=	$(INSTDIR)/man
LIBDIR		=	$(SRCDIR)/libny
UTILDIR		=	$(SRCDIR)/utils
EXAMPLESDIR	=	$(INSTDIR)/examples

SRCDIRS		=	$(SRCDIR)/utils \
			$(SRCDIR)/newyacc \
			$(SRCDIR)/libny
EXAMPLESDIRS	=	$(EXAMPLESDIR)/calc \
			$(EXAMPLESDIR)/newyacc/listers \
			$(EXAMPLESDIR)/reverse \
			$(EXAMPLESDIR)/simple

BINS		=	$(SRCDIR)/newyacc/newyacc
SCRIPTS		=	$(SRCDIR)/nylexfix.lex \
			$(SRCDIR)/nytabcfix.yacc \
			$(SRCDIR)/nytabcfix.byacc
MAN		=	$(MANDIR)/newyacc.1 \
			$(MANDIR)/manual.ps.Z
LIBS		=	libny.a

YACC		=	yacc
NEWYACC		=	$(SRCDIR)/newyacc/newyacc
NYLEXFIX	=	$(SRCDIR)/nylexfix.lex
NYTABCFIX	=	$(SRCDIR)/nytabcfix.yacc
NYLIB		=	$(SRCDIR)/libny/libny.a

NYMEMUTILS	=	$(UTILDIR)/memutils.o
NYUTILS		=	$(UTILDIR)/utils.o

INCLUDES	=	-I$(INCLUDEDIR)

CFLAGS		=	-O -w
INSTCOPY	=	-c
NYFLAGS		=	-l -d -Y $(YACC)

CLEAN = *~ *.bak *.o core a.out
CLEANS = "CLEAN=$(CLEAN)"
RCLEAN = $(CLEAN) lex.yy.c y.tab.c y.output ny.temp.y nytags.h y.tab.h
RCLEANS = "RCLEAN=$(RCLEAN)"

MFLAGS	=	INCLUDES=$(INCLUDES) NEWYACC=$(NEWYACC) \
		NYLIB=$(NYLIB) NYLIBDIR=$(DESTLIBS) \
		NYMEMUTILS=$(NYMEMUTILS) NYUTILS=$(NYUTILS) \
		"NYFLAGS=$(NYFLAGS)" NYLEXFIX=$(NYLEXFIX) \
		NYTABCFIX=$(NYTABCFIX) "CFLAGS=$(CFLAGS)" \
		"CLEAN=$(CLEAN)" "RCLEAN=$(RCLEAN)"

# all: .version
#	for Dir in $(SRCDIRS); do \
#		(cd $$Dir ; make $(MFLAGS)); done

all: .version
	cd src/utils ; make 
	cd src/newyacc ; make 
	cd src/libny ; make 

examples: all
	for Dir in $(EXAMPLESDIRS); do \
		(cd $$Dir; echo "making $$Dir"; make $(MFLAGS)); done

.version:
	echo "$(VERSION)" > .version

sources:

print:

clean:
	rm -rf $(CLEAN) lib/*.a
	for Dir in $(SRCDIRS); do \
		(cd $$Dir ; make clean); done
	for Dir in $(EXAMPLESDIRS); do \
		(cd $$Dir ; make clean); done

reallyclean:
	rm -rf $(RCLEAN) bin/* lib/*.a
	for Dir in $(SRCDIRS); do \
		(cd $$Dir; make reallyclean); done
	for Dir in $(EXAMPLESDIRS); do \
		(cd $$Dir; make reallyclean); done

install:
	rm -rf .version
	for File in $(BINS); do \
		(install -s $(INSTCOPY) -s $$File $(DESTBINS)); done
	for Lib in $(LIBS); do \
		(install $(INSTCOPY) $(LIBDIR)/$$Lib $(DESTLIBS) ; ranlib $(DESTLIBS)/$$Lib); done
	for File in $(SCRIPTS); do \
		(install $(INSTCOPY) $$File $(DESTSCRIPTS)); done
	for File in $(MAN); do \
		(install $(INSTCOPY) $$File $(DESTMAN)); done
