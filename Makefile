BIN_FILES  = editor broker server suscriptor

CC = gcc


CCGLAGS =	-Wall  -g

LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -lpthread


all: CFLAGS=$(CCGLAGS)
all: $(BIN_FILES)
.PHONY : all

editor: editor.o lines.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

broker: broker.o lines.o rpc_functions.o publications_clnt.o publications_xdr.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

server: publications_xdr.o publications_svc.o publications_server.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o publications_server

suscriptor:
	javac suscriptor.java

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -f $(BIN_FILES) *.o *.class

.SUFFIXES:
.PHONY : clean
