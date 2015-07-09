#
# Makefile --- pmcdsz log datafile (c) PMC 1999 - 2015
#

CC	= gcc
LIBDIR = /dd/daydream-2.10/lib/
MAINDIR = /dd/daydream-2.10/main/

#CFLAGS = -I$(MAINDIR) -I$(LIBDIR) -DUNIX -Wall -pipe -fomit-frame-pointer
CFLAGS = -I$(MAINDIR) -I$(LIBDIR) -I/code/include/

LIBS	= 

all:		pmcdata log pmctop wst

clean:          rm pmcdata log pmctop wst core

pmcdata:        pmcdata.c
	        $(CC) $(CFLAGS) -o $@ $< -L$(LIBDIR) 

log:        	log.c
	        $(CC) $(CFLAGS) -o $@ $<

pmctop:        	pmctop.c
	        $(CC) $(CFLAGS) -fwritable-strings -o $@ $< -L$(LIBDIR) 

wst:        	wst.c
	        $(CC) $(CFLAGS) -fwritable-strings -o $@ $< -L$(LIBDIR) 
