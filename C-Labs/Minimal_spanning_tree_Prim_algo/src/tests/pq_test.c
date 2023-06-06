#include <stdlib.h>
#include "../priority_queue.h"

int cmp(const void *a, const void *b) {
    int a_int = *(int*)a;
    int b_int = *(int*)b;
    return a_int - b_int;
}

int main(void) {
    exitCode_t err = SUCCESS;

    PriorityQueue_t *pq = createPQueue(4, sizeof(int), &cmp);

    for (int i = 0; i < 20; ++i) {
        int val = rand() % 100;
        pqAdd(pq, &val);
    }

    while (!pqIsEmpty(pq)) {
        int val;
        err = pqGetTop(pq, &val);
        printf("%i\n", val);
        printExitCodeDescription(stderr, err);
        err = pqPop(pq, &val);
        printf("%i\n", val);
        printExitCodeDescription(stderr, err);
    }

    destroyPQueue(&pq);
    return EXIT_SUCCESS;
}
