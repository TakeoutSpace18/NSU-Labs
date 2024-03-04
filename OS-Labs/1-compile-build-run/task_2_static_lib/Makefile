CC=gcc
CFLAGS=$(shell cat ../../build-cflags.txt)

PROJECT=hello_world_static
EXECUTABLE=hello_world_static
BUILDDIR=$(CURDIR)/build

.PHONY: all
all: $(PROJECT)

# project
$(PROJECT): main.c $(BUILDDIR)/hello_static_lib.a | $(BUILDDIR)
	$(CC) $(CFLAGS) main.c $(BUILDDIR)/hello_static_lib.a -o $(BUILDDIR)/$(EXECUTABLE)

# static library
$(BUILDDIR)/hello_static_lib.a: $(BUILDDIR)/hello_static_lib.o | $(BUILDDIR)
	ar rcs -o $(BUILDDIR)/hello_static_lib.a $(BUILDDIR)/hello_static_lib.o

$(BUILDDIR)/hello_static_lib.o: hello_static_lib.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c hello_static_lib.c -o $(BUILDDIR)/hello_static_lib.o

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)