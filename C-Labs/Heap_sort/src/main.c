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

void heapify(int* arr, size_t rootNode, size_t sz) {
    size_t lChild = rootNode * 2 + 1;
    size_t rChild = rootNode * 2 + 2;
    size_t largestNode = rootNode;

    if (lChild < sz && arr[lChild] > arr[largestNode]) {
        largestNode = lChild;
    }
    if (rChild < sz && arr[rChild] > arr[largestNode]) {
        largestNode = rChild;
    }
    if (arr[largestNode] != arr[rootNode]) {
        // move largest child to the root
        swap(&arr[rootNode], &arr[largestNode]);
        // recursively heapify child that has been changed
        heapify(arr, largestNode, sz);
    }
}

void makeHeap(int* arr, size_t sz) {
    // heapify nodes from bottom to top
    for (int i = sz / 2 - 1; i >= 0; --i) {
        heapify(arr, i, sz);
    }
}

void heapSort(int* arr, size_t sz) {
    makeHeap(arr, sz);
    while (sz > 0) {
        --sz; // reduce heap size
        swap(&arr[0], &arr[sz]); // move top element behind heap
        heapify(arr, 0, sz); // restore heap state
    }

}

int main(void) {
    int64_t elementsN;
    if (1 != scanf("%li", &elementsN)) {
        badInput();
    }
    int* elements = malloc(sizeof(int) * elementsN);
    readArray(elements, elementsN);
    
    heapSort(elements, elementsN);

    printArray(elements, elementsN);
    free(elements);
    return EXIT_SUCCESS;
}
