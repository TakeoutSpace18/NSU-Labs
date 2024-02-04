CC=gcc
CFLAGS=$(shell cat ../../build-cflags.txt)

EXECUTABLE=hello_world
BUILDDIR=$(CURDIR)/build

.PHONY: all
all: hello_world

hello_world: $(BUILDDIR)/hello.o | $(BUILDDIR)
	$(CC) $(CFLAGS) $(BUILDDIR)/hello.o -o $(BUILDDIR)/$(EXECUTABLE)

$(BUILDDIR)/hello.o: hello.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)