#include <stdbool.h>
#include <stdlib.h>

#include "KnapsackProblem.h"
#include "ErrorHandling.h"

static int Max(int a, int b) {
    return a > b ? a : b;
}

static int** AllocateIntMatrix(size_t n, size_t m) {
    int** matrix = malloc(sizeof(int*) * n);
    if (!matrix) ExitWithError(ALLOCATION_ERROR);

    for (size_t i = 0; i < n; ++i) {
        matrix[i] = calloc(m, sizeof(int));
        if (!matrix[i]) ExitWithError(ALLOCATION_ERROR);
    }
    return matrix;
}

static void DestroyIntMatrix(int** matrix, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        free(matrix[i]);
    }
    free(matrix);
}


static int GetBestTotalCost(int** costTable, KnapsackData* data, int step, int load);
static void CollectIncludedItems(int** costTable, KnapsackData* data, KnapsackAnswer* ans);

KnapsackAnswer* KnapsackProblem(int* weights, int* prices, int itemsCount, int capacity) {
    int** costTable = AllocateIntMatrix(itemsCount + 1, capacity + 1);
    KnapsackData data = {itemsCount, capacity, weights, prices};

    for (int step = 1; step <= itemsCount; ++step) {
        for (int load = 0; load <= capacity; ++load) {
            costTable[step][load] = GetBestTotalCost(costTable, &data, step, load);
        }
    }

    KnapsackAnswer* ans = calloc(1, sizeof(KnapsackAnswer));
    if (!ans) ExitWithError(ALLOCATION_ERROR);

    ans->bestTotalCost = costTable[itemsCount][capacity];

    CollectIncludedItems(costTable, &data, ans);

    DestroyIntMatrix(costTable, itemsCount + 1);
    return ans;
}

static int GetBestTotalCost(int** costTable, KnapsackData* data, int step, int load) {
    // get current item weight and price
    int w = data->weights[step - 1]; // indices start from 0, while step starts from 1.
    int p = data->prices[step - 1];

    int leaveItem = costTable[step - 1][load];
    if (load - w >= 0) {
        int takeItem = costTable[step - 1][load - w] + p;
        return Max(leaveItem, takeItem);
    }
    return leaveItem;
}

static void CollectIncludedItems(int** costTable, KnapsackData* data, KnapsackAnswer* ans) {
    bool* included = calloc(data->itemsCount, sizeof(bool));
    if (!included) ExitWithError(ALLOCATION_ERROR);

    int load = data->capacity;

    for (int step = data->itemsCount; step > 0; --step) {
        if (costTable[step][load] == costTable[step - 1][load]) {
            included[step - 1] = false;
        }
        else  {
            included[step - 1] = true;
            ++ans->includedItemsCount;
            load -= data->weights[step - 1];
        }
    }

    ans->includedItems = malloc(ans->includedItemsCount * sizeof(int));
    if (!ans->includedItems) ExitWithError(ALLOCATION_ERROR);

    int idx = 0;
    for (int i = 0; i < data->itemsCount; ++i) {
        if (included[i]) {
            ans->includedItems[idx++] = i;
        }
    }
    free(included);
}

void DestroyKnapsackAnswer(KnapsackAnswer* ans) {
    free(ans->includedItems);
    free(ans);
}
