PCAP_PATH = ../../lib
CFLAGS = -g -O -mno-cygwin -I ../../include

OBJS = kdump.o
LIBS = -L ${PCAP_PATH} -lwpcap

all: ${OBJS}
	${CC} ${CFLAGS} -o kdump.exe ${OBJS} ${LIBS}

clean:
	rm -f ${OBJS} kdump.exe

.c.o:
	${CC} ${CFLAGS} -c -o $*.o $<
