#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define PM_PFN(entry) ((entry) & 0x7fffffffffffff)
#define PM_SWAPTYPE(entry) ((entry) & 0x1f)
#define PM_SWAPOFFSET(entry) (((entry) & 0x7fffffffffffff) >> 5)
#define PM_SOFTDIRTY_PTE(entry) (((entry) >> 55) & 0x1)
#define PM_EXCLUSIVELY_MAPPED(entry) (((entry) >> 56) & 0x1)
#define PM_FILEPAGE(entry) (((entry) >> 61) & 0x1)
#define PM_SWAPPED(entry) (((entry) >> 62) & 0x1)
#define PM_PRESENT(entry) (((entry) >> 63) & 0x1)

#define PM_ENTRY_SIZE 8


void print_usage()
{
    printf("usage:\n\tread_pagemap <pid>\n\tread_pagemap self");
}

void print_pmentry(int64_t vaddr, int64_t entry, uint64_t pagecount)
{
    printf("%llx\t%llx\t%llu\t%c\t%c\t%c\t%c\t%c\t",
        vaddr,
        PM_PFN(entry),
        pagecount,
        PM_PRESENT(entry) ? 'x' : ' ',
        PM_SOFTDIRTY_PTE(entry) ? 'x' : ' ',
        PM_EXCLUSIVELY_MAPPED(entry) ? 'x' : ' ',
        PM_FILEPAGE(entry) ? 'x' : ' ',
        PM_SWAPPED(entry) ? 'x' : ' ');

    if (PM_SWAPPED(entry)) {
        printf("%c\t%c", PM_SWAPOFFSET(entry));
    }
    printf("\n");
}

int64_t read_pagemap_entry(FILE *pagemap, uint64_t vaddr)
{
    int64_t offset = vaddr / sysconf(_SC_PAGESIZE) * PM_ENTRY_SIZE;

    if (fseek(pagemap, offset, SEEK_SET) == -1) {
        perror("fseek(): failed to seek page map entry");
        return -1;
    }

    int64_t pagemap_entry;
    ssize_t nread;
    if ((nread = fread(&pagemap_entry, sizeof(pagemap_entry), 1, pagemap)) != 1) {
        perror("fread(): failed to read pagemap entry");
        return -1;
    }

    return pagemap_entry;
}

uint64_t read_pagecount(FILE *kpagecount, int64_t pmentry)
{
    int64_t offset = PM_PFN(pmentry) * sizeof(uint64_t);

    if (fseek(kpagecount, offset, SEEK_SET) == -1) {
        perror("fseek(): failed to seek pagecount");
        return -1;
    }

    uint64_t pagecount;
    ssize_t nread;
    if ((nread = fread(&pagecount, sizeof(pagecount), 1, kpagecount)) != 1) {
        perror("fread(): failed to read pagemap entry");
        return -1;
    }

    return pagecount;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        print_usage();
        return EXIT_FAILURE;
    }


    pid_t pid;
    if (strcmp(argv[1], "self") == 0) {
        pid = getpid();
    } else {
        pid = atoi(argv[1]);
    }

    char pagemap_filename[30];
    snprintf(pagemap_filename, 30, "/proc/%i/pagemap", pid);
    FILE *pagemap = fopen(pagemap_filename, "r");
    if (pagemap == NULL) {
        perror("open(pagemap)");
        return EXIT_FAILURE;
    }

    char maps_filename[30];
    snprintf(maps_filename, 30, "/proc/%i/maps", pid);
    FILE *maps = fopen(maps_filename, "r");
    if (maps == NULL) {
        perror("open(maps)");
        return EXIT_FAILURE;
    }

    FILE *kpagecount = fopen("/proc/kpagecount", "r");
    if (kpagecount == NULL) {
        perror("open(kpagecount)");
        return EXIT_FAILURE;
    }

    printf("addr\t\tPFN\tpagecnt\tpresent\tsd\texcl-mp\tfilepage\tswapped\tswtype\tswoffset\n");

    size_t bufsize = 256 * sizeof(char);
    char *line = (char*)malloc(bufsize);

    ssize_t nread;
    while ((nread = getline(&line, &bufsize, maps)) != -1) {
        char *addr_end = strchr(line, '-');
        if (addr_end == NULL) {
            fprintf(stderr, "%s: %s address delimeter not found", maps_filename, line);
            continue;
        }

        int64_t vaddr_start = strtoll(line, NULL, 16);
        int64_t vaddr_stop = strtoll(line + 13, NULL, 16);

        for (int64_t vaddr = vaddr_start; vaddr < vaddr_stop; vaddr += sysconf(_SC_PAGESIZE)) {
            int64_t pmentry = read_pagemap_entry(pagemap, vaddr);
            if (pmentry == -1) {
                continue;
            }

            uint64_t pcount = read_pagecount(kpagecount, pmentry);

            print_pmentry(vaddr,pmentry, pcount);

        }

    }

    printf("sd - soft dirty pte\n");
    printf("excl-mp - exclusively mapped page\n");
    printf("pagecnt - data from /proc/kpagecount\n");

    free(line);
    fclose(maps);
    fclose(pagemap);
    fclose(kpagecount);
    return EXIT_SUCCESS;
}
