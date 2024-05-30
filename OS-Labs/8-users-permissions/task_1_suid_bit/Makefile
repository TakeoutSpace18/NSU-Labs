CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: suid_bit

suid_bit: suid_bit.c | $(BUILDDIR)
	gcc $(CFLAGS) suid_bit.c -O0 -g -o $(BUILDDIR)/suid_bit

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)