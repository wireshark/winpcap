# Makefile for cygwin gcc
# Nate Lawson <nate@rootlabs.com>

PCAP_PATH = ../../../lib
CFLAGS = -g -O -mno-cygwin -DDBG

OBJS = TestPacketCapture.o
LIBS = -L ${PCAP_PATH} -lPacket

all: ${OBJS}
	${CC} ${CFLAGS} -o TestPacketCapture.exe ${OBJS} ${LIBS}

clean:
	rm -f ${OBJS} TestPacketCapture.exe

.c.o:
	${CC} ${CFLAGS} -c -o $*.o $<
