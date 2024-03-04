CC=gcc
CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: hello_world_dynamic_runtime

# project
hello_world_dynamic_runtime: main.c $(BUILDDIR)/libhello.so | $(BUILDDIR)
	gcc $(CFLAGS) main.c -o $(BUILDDIR)/hello_world_dynamic_runtime

# shared library
$(BUILDDIR)/libhello.so: hello_dynamic_lib.c | $(BUILDDIR)
	gcc $(CFLAGS) -shared -fPIC hello_dynamic_lib.c -o $(BUILDDIR)/libhello.so

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)