# Makefile for cygwin gcc
# Nate Lawson <nate@rootlabs.com>

PCAP_PATH = ../../../lib
CFLAGS = -g -O -mno-cygwin -I ../../include

OBJS = TestPacketSend.o
LIBS = -L ${PCAP_PATH} -lPacket

all: ${OBJS}
	${CC} ${CFLAGS} -o TestPacketSend.exe ${OBJS} ${LIBS}

clean:
	rm -f ${OBJS} TestPacketSend.exe

.c.o:
	${CC} ${CFLAGS} -c -o $*.o $<
