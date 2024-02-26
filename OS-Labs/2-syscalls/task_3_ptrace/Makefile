CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: trace_syscalls

trace_syscalls: trace_syscalls.c | $(BUILDDIR)
	gcc $(CFLAGS) trace_syscalls.c -o $(BUILDDIR)/trace_syscalls

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
