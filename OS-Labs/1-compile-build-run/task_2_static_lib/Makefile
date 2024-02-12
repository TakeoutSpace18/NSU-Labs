CC=gcc
CFLAGS=$(shell cat ../../build-cflags.txt)

PROJECT=hello_world_static
EXECUTABLE=hello_world_static
BUILDDIR=$(CURDIR)/build

.PHONY: all
all: $(PROJECT)

# project
$(PROJECT): $(BUILDDIR)/main.o $(BUILDDIR)/hello_static_lib.a | $(BUILDDIR)
	$(CC) $(CFLAGS) $(BUILDDIR)/main.o -l:$(BUILDDIR)/hello_static_lib.a -o $(BUILDDIR)/$(EXECUTABLE)

# static library
$(BUILDDIR)/hello_static_lib.a: hello_static_lib.o | $(BUILDDIR)
	ar rcs -o $(BUILDDIR)/hello_static_lib.a $(BUILDDIR)/hello_static_lib.o

# main.o
$(BUILDDIR)/main.o: main.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# hello_static_lib.o
$(BUILDDIR)/hello_static_lib.o: hello_static_lib.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)