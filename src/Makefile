CC=gcc
LD=ld
RM=rm -f
CFLAGS=-g -Wall
LDFLAGS=-g -lm

SRCS=main.c libbmp.c
OBJS=$(subst .c,.o,$(SRCS))

all: stego

stego: $(OBJS)
		$(CC) $(CFLAGS) -o stego $(OBJS) $(LDFLAGS)

depend: .depend

.depend: $(SRCS)
		$(RM) ./.depend
		$(CC) $(CFLAGS) -MM $^>>./.depend;

clean:
		$(RM) $(OBJS)

distclean: clean
		$(RM) *~ .depend

include .depend