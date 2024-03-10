CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: copy_backwards

copy_backwards: copy_backwards.c | $(BUILDDIR)
	gcc $(CFLAGS) copy_backwards.c -g -o $(BUILDDIR)/copy_backwards

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
