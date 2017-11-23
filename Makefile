
CC=gcc
CFLAGS=-g 
CPPFLAGS=-Wall -Werror

PROGS=resend shell

.PHONY: all
all: $(PROGS)

resend: resend.o
shell: shell.o shell-utils.o

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%: %.o
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS)

.PHONY: clean
clean:
	$(RM) $(PROGS) *.o
