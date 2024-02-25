CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: hello_world_asm

hello_world_asm: hello_asm.c | $(BUILDDIR)
	gcc $(CFLAGS) hello_asm.c -o $(BUILDDIR)/hello_world_asm

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
