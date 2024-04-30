CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: read_pagemap

read_pagemap: read_pagemap.c | $(BUILDDIR)
	gcc $(CFLAGS) read_pagemap.c -g -o $(BUILDDIR)/read_pagemap

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
