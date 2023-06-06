#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "KnapsackProblem.h"
#include "ErrorHandling.h"

int main(void) {
    int itemsCount, capacity;
    if (scanf("%i%i", &itemsCount, &capacity) != 2) {
        ExitWithError(INPUT_ERROR);
    }

    int* weights = malloc(sizeof(int) * itemsCount);
    if (!weights) ExitWithError(ALLOCATION_ERROR);

    int* prices = malloc(sizeof(int) * itemsCount);
    if (!prices) ExitWithError(ALLOCATION_ERROR);

    for (int i = 0; i < itemsCount; ++i) {
        if (scanf("%i%i", &weights[i], &prices[i]) != 2) {
            ExitWithError(INPUT_ERROR);
        }
    }

    KnapsackAnswer* ans = KnapsackProblem(weights, prices, itemsCount, capacity);
    printf("%i\n", ans->bestTotalCost);

    for (int i = 0; i < ans->includedItemsCount; ++i) {
        int item = ans->includedItems[i];
        printf("%i %i\n", weights[item], prices[item]);
    }

    DestroyKnapsackAnswer(ans);
    free(weights);
    free(prices);
    return EXIT_SUCCESS;
}
