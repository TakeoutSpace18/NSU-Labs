CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: self_exec allocation

self_exec: self_exec.c | $(BUILDDIR)
	gcc $(CFLAGS) self_exec.c -O0 -o $(BUILDDIR)/self_exec

allocation: allocation.c | $(BUILDDIR)
	gcc $(CFLAGS) allocation.c -O0 -o $(BUILDDIR)/allocation

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)