CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: server client

server: udp_echo_server.c utils.h | $(BUILDDIR)
	gcc $(CFLAGS) udp_echo_server.c utils.h -O0 -g -o $(BUILDDIR)/udp_echo_server

client: udp_client.c utils.h | $(BUILDDIR)
	gcc $(CFLAGS) udp_client.c utils.h -O0 -g -o $(BUILDDIR)/udp_client

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)