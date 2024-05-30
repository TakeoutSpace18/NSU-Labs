CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: read_pagemap copy_on_write

read_pagemap: read_pagemap.c | $(BUILDDIR)
	gcc $(CFLAGS) read_pagemap.c -g -o $(BUILDDIR)/read_pagemap

copy_on_write: copy_on_write.c | $(BUILDDIR)
	gcc $(CFLAGS) copy_on_write.c -g -o $(BUILDDIR)/copy_on_write

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
