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
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(BUILDDIR)

$(BUILDDIR)/libhello.so: hello_dynamic_lib.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -shared hello_dynamic_lib.c -o build/libhello.so

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)