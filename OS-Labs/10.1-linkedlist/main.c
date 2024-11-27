#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

void print_usage(const char *prog_name) {
    printf("Usage: %s [options]\n", prog_name);
    printf("Options:\n");
    printf("  -l, --list-size SIZE   Set the size of the list (required)\n");
    printf("  -h, --help             Show this help message and exit\n");
}

int main(int argc, char *argv[]) {
    int list_size = -1;
    int opt;
    int option_index = 0;

    static struct option long_options[] = {
        {"list-size", required_argument, 0, 'l'},
        {"help",      no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "l:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'l':
                list_size = atoi(optarg);
                if (list_size <= 0) {
                    fprintf(stderr, "Error: list size must be a positive integer.\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;
            case '?':
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (list_size == -1) {
        fprintf(stderr, "Error: --list-size is required.\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    printf("List size: %d\n", list_size);

    // You can now use the list_size variable in your program logic
    return EXIT_SUCCESS;
}
