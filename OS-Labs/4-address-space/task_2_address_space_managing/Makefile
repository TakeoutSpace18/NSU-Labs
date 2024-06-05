CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: self_exec stack_heap_allocation mmap_allocation

self_exec: self_exec.c | $(BUILDDIR)
	gcc $(CFLAGS) self_exec.c -O0 -g -o $(BUILDDIR)/self_exec

stack_heap_allocation: stack_heap_allocation.c | $(BUILDDIR)
	gcc $(CFLAGS) stack_heap_allocation.c -O0 -g -o $(BUILDDIR)/stack_heap_allocation

mmap_allocation: stack_heap_allocation.c | $(BUILDDIR)
	gcc $(CFLAGS) mmap_allocation.c -O0 -g -o $(BUILDDIR)/mmap_allocation

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)