#pragma once

typedef struct {
    int bestTotalCost;
    int* includedItems;
    int includedItemsCount;
} KnapsackAnswer;

typedef struct {
    int itemsCount;
    int capacity;
    int* weights;
    int* prices;
} KnapsackData;

KnapsackAnswer* KnapsackProblem(int* weights, int* prices, int itemsCount, int capacity);
void DestroyKnapsackAnswer(KnapsackAnswer* ans);
