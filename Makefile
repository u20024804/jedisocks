UNAME := $(shell uname)

RTFLAGS=-lrt
ifeq ($(UNAME), Darwin)
RTFLAGS=-framework CoreServices
endif
OLEVEL=-O2 -DNDEBUG
CFLAGS=-Wall $(OLEVEL) -std=gnu99 -luv
LOCALFILES=local.c
LOCALAPP=js-local
SERVERFILES=server.c
SERVERAPP=js-server
TESTFILE=test.c
TESTAPP=test

all: server local
local: $(LOCALFILES)
	$(CC) $(CFLAGS) -o \
	$(LOCALAPP) $(LOCALFILES) \
	-lcrypto -lm $(RTFLAGS)

server:	$(SERVERFILES)
	$(CC) $(CFLAGS) -o \
	$(SERVERAPP) $(SERVERFILES) \
	-lcrypto -lm $(RTFLAGS)

test: $(TESTFILE)
	$(CC) $(CFLAGS) -o \
	$(TESTAPP) $(TESTFILE) \
	-lcrypto -lm $(RTFLAGS)

clean:
	rm -rf $(SERVERAPP) $(LOCALAPP) $(TESTAPP)
	rm -rf *.o
