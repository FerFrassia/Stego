CC=gcc
CXX=g++
LD=ld
RM=rm -f
CFLAGS=-g -Wall
CXXFLAGS=-g -Wall
LDFLAGS=-lm

CPPSRCS=main.cpp BMPImage.cpp LSBStream.cpp LSBReader.cpp LSBWriter.cpp
CPPOBJS=$(subst .cpp,.o,$(CPPSRCS))

CSRCS=libbmp.c
COBJS=$(subst .c,.o,$(CSRCS))

all: stego

stego: $(CPPOBJS) $(COBJS)
		$(CXX) $(CXXFLAGS) -o stego $(CPPOBJS) $(COBJS) $(LDFLAGS)

cdepend: .cdepend

.cdepend: $(CSRCS)
		$(RM) ./.cdepend
		$(CC) $(CFLAGS) -MM $^>>./.cdepend;

cppdepend: .cppdepend

.cppdepend: $(CPPSRCS)
		$(RM) ./.cppdepend
		$(CXX) $(CXXFLAGS) -MM $^>>./.cppdepend;

clean:
		$(RM) $(COBJS) $(CPPOBJS)

distclean: clean
		$(RM) *~ .cdepend
		$(RM) *~ .cppdepend

include .cdepend
include .cppdepend