.PHONY: all
all: self_discovery

self_discovery: src/self_discovery.c | $(BUILDDIR)
	gcc $(CFLAGS) src/self_discovery.c -O0 -g -o $@

.PHONY: clean
clean:
