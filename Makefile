BIN_FILES  = editor broker server

CC = gcc


CCGLAGS =	-Wall  -g

LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -lpthread


all: CFLAGS=$(CCGLAGS)
all: $(BIN_FILES)
.PHONY : all

editor: editor.o lines.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

broker: broker.o lines.o rpc_functions.o
	$(CC) $(LDFLAGS) broker.o lines.o rpc_functions.o $(LDLIBS) -o $@

server: publications_clnt.o publications_xdr.o publications_svc.o publications_server.o
	$(CC) $(LDFLAGS) publications_svc.o publications_xdr.o publications_server.o -o publications_server

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -f $(BIN_FILES) *.o

.SUFFIXES:
.PHONY : clean
