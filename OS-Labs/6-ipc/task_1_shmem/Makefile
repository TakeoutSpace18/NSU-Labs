CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: shmem

shmem: shmem.c | $(BUILDDIR)
	gcc $(CFLAGS) shmem.c -O0 -g -o $(BUILDDIR)/shmem

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)