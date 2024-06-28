CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: server client

server: udp_echo_server.c | $(BUILDDIR)
	gcc $(CFLAGS) udp_echo_server.c -O0 -I ../ -g -o $(BUILDDIR)/udp_echo_server

client: udp_client.c | $(BUILDDIR)
	gcc $(CFLAGS) udp_client.c -O0 -I ../ -g -o $(BUILDDIR)/udp_client

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
