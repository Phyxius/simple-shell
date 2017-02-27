CC=gcc
CFLAGS=-Wall -Werror -g -D_GNU_SOURCE -std=c11
RM=/bin/rm -f

all: simsh1 simsh2

simsh1: simsh1.c libsimsh1.c libsimsh1.h aux_files/chop_line.c aux_files/chop_line.h aux_files/list.c aux_files/list.h
	$(CC) $(CFLAGS) -o $@ simsh1.c libsimsh1.c libsimsh1.h aux_files/chop_line.c aux_files/chop_line.h aux_files/list.c aux_files/list.h

simsh2: simsh2.c libsimsh2.c libsimsh2.h aux_files/chop_line.c aux_files/chop_line.h aux_files/list.c aux_files/list.h
	$(CC) $(CFLAGS) -o $@ simsh2.c libsimsh2.c libsimsh2.h aux_files/chop_line.c aux_files/chop_line.h aux_files/list.c aux_files/list.h

clean:
	$(RM) simsh1 simsh2