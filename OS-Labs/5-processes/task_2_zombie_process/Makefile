CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: zombie

zombie: zombie.c | $(BUILDDIR)
	gcc $(CFLAGS) zombie.c -O0 -g -o $(BUILDDIR)/zombie

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)