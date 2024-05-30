#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE 256

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: suid_bit <path_to_file");
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("fopen()");
        return EXIT_FAILURE;
    }

    char buf[BUFSIZE];

    size_t nread;
    while ((nread = fread(buf, 1, BUFSIZE, file)) > 0) {
        fwrite(buf, nread, 1, stdout);
    }

    fclose(file);

    printf("User id: %u\n", getuid());
    printf("Effective user id: %u\n", geteuid());

    return EXIT_SUCCESS;
}
