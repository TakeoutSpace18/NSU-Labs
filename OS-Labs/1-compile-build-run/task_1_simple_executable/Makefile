CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic
LDFLAGS=
EXECUTABLE=hello_world
BUILDDIR=$(CURDIR)/build

.PHONY: all
all: hello_world

hello_world: $(BUILDDIR)/hello_world.o | $(BUILDDIR)
	$(CC) $(LDFLAGS) $(BUILDDIR)/main.o -o $(BUILDDIR)/$(EXECUTABLE)

$(BUILDDIR)/hello_world.o: hello_world.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)