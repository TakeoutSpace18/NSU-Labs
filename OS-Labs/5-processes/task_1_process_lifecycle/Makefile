CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: fork

fork: fork.c | $(BUILDDIR)
	gcc $(CFLAGS) fork.c -O0 -g -o $(BUILDDIR)/fork

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)