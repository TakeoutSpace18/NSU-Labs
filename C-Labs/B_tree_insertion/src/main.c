#include <stdlib.h>
#include <stdio.h>

#include "b_tree.h"
#include "error_handling.h"

int main(void) {
    exitCode_t exitCode = SUCCESS;

    BTree_t *tree = NULL;
    FILE *input = fopen("in.txt", "r");
    FILE *output = fopen("out.txt", "w");
    if (!input || !output) {
        return_defer(IO_ERROR);
    }

    size_t half_degree, nodesCount;
    if (fscanf(input, "%zu%zu", &half_degree, &nodesCount) != 2) {
        return_defer(WRONG_INPUT);
    }

    tree = btree_create(half_degree);
    if (!tree) {
        return_defer(ALLOCATION_ERROR);
    }

    for (size_t i = 0; i < nodesCount; ++i) {
        int value;
        if (fscanf(input, "%i", &value) != 1) {
            return_defer(WRONG_INPUT);
        }

        exitCode = btree_insert(tree, value);
        if (exitCode != SUCCESS) {
            return_defer(exitCode);
        }

    }
    fprintf(output, "%zu\n", btree_getHeight(tree));

defer:
    printExitCodeDescription(stderr, exitCode);
    if (input) {
        fclose(input);
    }
    if (output) {
        fclose(output);
    }
    btree_destroy(&tree);
    return exitCode;
}
