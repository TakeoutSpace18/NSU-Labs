CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: server client

server: tcp_echo_server.c | $(BUILDDIR)
	gcc $(CFLAGS) tcp_echo_server.c -I ../ -g -o $(BUILDDIR)/tcp_echo_server

client: tcp_client.c | $(BUILDDIR)
	gcc $(CFLAGS) tcp_client.c -O0 -I ../ -g -o $(BUILDDIR)/tcp_client

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
