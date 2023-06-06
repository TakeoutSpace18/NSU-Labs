#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

int badInput() {
    printf("bad input\n");
    exit(0);
}

void printArray(int* arr, size_t sz) {
    for (size_t i = 0; i < sz; ++i) {
        printf("%i ", arr[i]);
    }
    printf("\n");
}

void readArray(int* arr, size_t sz) {
    for (size_t i = 0; i < sz; ++i) {
        if (1 != scanf("%i", &arr[i])) {
            badInput();
        }
    }
}

void swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

/* splits array into (<= pivot) and (>= pivot) groups
 and returns rightmost index of the first group.*/
int partition(int* arr, int start, int end) {
    int pivot = arr[(start + end) / 2]; // choose pivot in center
    int hi = end;
    int lo = start;

    while (lo <= hi) {
        while (arr[lo] < pivot) ++lo;
        while (arr[hi] > pivot) --hi;
        if (lo >= hi) {
            return hi;
        }
        swap(&arr[lo], &arr[hi]);
        ++lo;
        --hi;
    }
    return hi;
}

void quickSort(int* arr, int start, int end) {
    if (start >= end) return;

    int dividerIndex = partition(arr, start, end);

    quickSort(arr, start, dividerIndex);
    quickSort(arr, dividerIndex + 1, end);
}

int main(void) {
    int arraySize;
    if (1 != scanf("%i", &arraySize)) {
        badInput();
    }
    int* array = malloc(sizeof(int) * arraySize);
    readArray(array, arraySize);

    quickSort(array, 0, arraySize-1);

    printArray(array, arraySize);
    free(array);
    return EXIT_SUCCESS;
}
