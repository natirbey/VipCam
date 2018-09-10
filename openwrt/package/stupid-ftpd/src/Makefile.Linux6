
#
#
# Makefile for the linux version of stupid-ftpd
#
#
#


CC=gcc
OBJS=ftpcommand.o ftpdconfig.o command.o ls.o stupid-ftpd.o
DOBJS=ftpcommand.do ftpdconfig.do command.do ls.do stupid-ftpd.do
POBJS=ftpcommand.po ftpdconfig.po command.po ls.po stupid-ftpd.po
LIBS=
CFLAGS=-O2 -Wall -Wstrict-prototypes
DCFLAGS=-g -DDEBUG -Wall -Wstrict-prototypes
PCFLAGS=-g -DDEBUG -Wall -Wstrict-prototypes -Wcast-align -Wwrite-strings -Wconversion -Waggregate-return -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs
EXEC=stupid-ftpd.Linux6

.SUFFIXES: .c .o .do .po

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) $(LIBS)

debug: $(DOBJS)
	$(CC) $(DCFLAGS) -o $(EXEC) $(DOBJS) $(LIBS)

pedantic: $(POBJS)
	$(CC) $(PCFLAGS) -o $(EXEC) $(POBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(DOBJS) $(POBJS) $(EXEC) *~

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

.c.do:
	$(CC) $(DCFLAGS) -c -o $@ $<

.c.po:
	$(CC) $(PCFLAGS) -c -o $@ $<

install:
	install -m 755 -s ./stupid-ftpd /usr/local/bin/stupid-ftpd
	install -m 700 -d /etc/stupid-ftpd
	install -m 755 -d /usr/local/stupid-ftpd
	install -m 600 ./stupid-ftpd.conf /etc/stupid-ftpd/stupid-ftpd.conf
