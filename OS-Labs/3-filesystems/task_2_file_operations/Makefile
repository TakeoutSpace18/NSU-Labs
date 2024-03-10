CFLAGS=$(shell cat ../../build-cflags.txt)

BUILDDIR=$(CURDIR)/build

.PHONY: all
all: file_operations create_hardlinks

file_operations: file_operations.c | $(BUILDDIR)
	gcc $(CFLAGS) file_operations.c -g -o $(BUILDDIR)/file_operations

create_hardlinks: file_operations
	ln -f $(BUILDDIR)/file_operations $(BUILDDIR)/create_dir
	ln -f $(BUILDDIR)/file_operations $(BUILDDIR)/print_dir_entries
	ln -f $(BUILDDIR)/file_operations $(BUILDDIR)/remove_dir
	ln -f $(BUILDDIR)/file_operations $(BUILDDIR)/create_file
	ln -f $(BUILDDIR)/file_operations $(BUILDDIR)/print_file
	ln -f $(BUILDDIR)/file_operations $(BUILDDIR)/remove_file
	ln -f $(BUILDDIR)/file_operations $(BUILDDIR)/create_symlink
	ln -f $(BUILDDIR)/file_operations $(BUILDDIR)/print_symlink

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
