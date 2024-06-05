CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: address_space_structure

address_space_structure: address_space_structure.c | $(BUILDDIR)
	gcc $(CFLAGS) address_space_structure.c -O0 -g -o $(BUILDDIR)/address_space_structure

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)