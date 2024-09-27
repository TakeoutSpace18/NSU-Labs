#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <getopt.h>

static void
do_help(void)
{
    printf("Send file to receiving server");
    printf("Short form: send-file <filepath> <hostname:port>");
    printf("Usage:\n");
    printf("\t--file,-f\tpath to file to send\n");
    printf("\t--dest-address,-D\tdestination_hostname:port\n");
    printf("\t--help,-h\n");
}

int
main(int argc, char** argv) {

    static struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"file", required_argument, NULL, 'f'},
        {"dest-address", required_argument, NULL, 'D'},
        {NULL, 0, NULL, 0}
    };

    int c;
    int option_index;

    while ((c = getopt_long(argc, argv, "f:D:h",
                            long_options, &option_index)) != -1) {
        switch (c) {
            case 'f':
                // port = strdup(optarg);
                break;
            case 'D':
                // storage_dir = strdup(optarg);
                break;
            case 'h':
                do_help();
                exit(EXIT_SUCCESS);
            default:
                /* getopt_long already issued a suitable error message */
                exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}

