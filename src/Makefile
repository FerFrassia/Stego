CC=gcc
CXX=g++
LD=ld
RM=rm -f
CFLAGS=-g -Wall
CXXFLAGS=-g -Wall
LDFLAGS=-g -lm

EXECS=stego metrics

STEGO_SRCS=stego.cpp \
	Text/Text.cpp \
	BMPImage/BMPImage.cpp \
	StegoEncoding/StegoEncoding.cpp \
	LSBStream/LSBStream.cpp LSBStream/LSBReader.cpp LSBStream/LSBWriter.cpp
STEGO_OBJS=$(subst .cpp,.o,$(STEGO_SRCS))

METRICS_SRCS=metrics.cpp
METRICS_OBJS=$(subst .cpp,.o,$(METRICS_SRCS))

LIBBMP_SRCS=BMPImage/libbmp.c
LIBBMP_OBJS=$(subst .c,.o,$(LIBBMP_SRCS))


all: stego metrics

stego: $(STEGO_OBJS) $(LIBBMP_OBJS)
		$(CXX) $(CXXFLAGS) -o stego $(STEGO_OBJS) $(LIBBMP_OBJS) $(LDFLAGS)

metrics: $(METRICS_OBJS) $(LIBBMP_OBJS)
		$(CXX) $(CXXFLAGS) -o metrics $(METRICS_OBJS) $(LIBBMP_OBJS) $(LDFLAGS)

cdepend: .cdepend

.cdepend: $(LIBBMP_SRCS)
		$(RM) ./.cdepend
		$(CC) $(CFLAGS) -MM $^>>./.cdepend;

cppdepend: .cppdepend

.cppdepend: $(STEGO_SRCS) $(METRICS_SRCS)
		$(RM) ./.cppdepend
		$(CXX) $(CXXFLAGS) -MM $^>>./.cppdepend;

clean:
		$(RM) $(STEGO_OBJS) $(METRICS_OBJS) $(LIBBMP_OBJS)

distclean: clean
		$(RM) *~ .cdepend
		$(RM) *~ .cppdepend

include .cdepend
include .cppdepend