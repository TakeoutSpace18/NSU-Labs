CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: pipe

pipe: pipe.c | $(BUILDDIR)
	gcc $(CFLAGS) pipe.c -O0 -g -o $(BUILDDIR)/pipe

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)