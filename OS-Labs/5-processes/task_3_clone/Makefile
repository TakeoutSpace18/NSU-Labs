CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: clone clear_stack_dump

clear_stack_dump:
	rm stack_dump.bin

clone: clone.c | $(BUILDDIR)
	gcc $(CFLAGS) clone.c -O0 -g -o $(BUILDDIR)/clone

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)