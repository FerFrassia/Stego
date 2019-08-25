CC=gcc
CXX=g++
LD=ld
RM=rm -f
CFLAGS=-g -Wall
CPPLAGS=-g -Wall
LDFLAGS=-g -lm

CPPSRCS=main.cpp BMPImage.cpp
CPPOBJS=$(subst .cpp,.o,$(CPPSRCS))

CSRCS=libbmp.c
COBJS=$(subst .c,.o,$(CSRCS))

all: stego

stego: $(CPPOBJS) $(COBJS)
		$(CXX) $(CPPFLAGS) -o stego $(CPPOBJS) $(COBJS) $(LDFLAGS)

cdepend: .cdepend

.cdepend: $(CSRCS)
		$(RM) ./.cdepend
		$(CC) $(CFLAGS) -MM $^>>./.cdepend;

cppdepend: .cppdepend

.cppdepend: $(CPPSRCS)
		$(RM) ./.cppdepend
		$(CXX) $(CPPFLAGS) -MM $^>>./.cppdepend;

clean:
		$(RM) $(COBJS) $(CPPOBJS)

distclean: clean
		$(RM) *~ .cdepend
		$(RM) *~ .cppdepend

include .cdepend
include .cppdepend