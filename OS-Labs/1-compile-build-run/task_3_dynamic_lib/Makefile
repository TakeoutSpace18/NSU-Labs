CC=gcc
CFLAGS=$(shell cat ../../build-cflags.txt)

PROJECT=hello_world_dynamic
EXECUTABLE=hello_world_dynamic
BUILDDIR=$(CURDIR)/build

.PHONY: all
all: $(PROJECT)

# project
$(PROJECT): main.c $(BUILDDIR)/libhello.so | $(BUILDDIR)
	$(CC) $(CFLAGS) -L$(BUILDDIR) main.c -lhello -o $(BUILDDIR)/$(EXECUTABLE)

# shared library
$(BUILDDIR)/libhello.so: hello_dynamic_lib.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -shared -fPIC hello_dynamic_lib.c -o build/libhello.so

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)