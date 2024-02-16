CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: hello_world

hello_world: hello.c | $(BUILDDIR)
	gcc $(CFLAGS) hello.c -o $(BUILDDIR)/hello_world

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
