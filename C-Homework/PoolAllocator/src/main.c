#include <stdlib.h>
#include <stdio.h>

#include "pool_allocator.h"

int main() {
    PoolAllocator_t *allocator = createPoolAllocator(sizeof(void*));
    void *testbench[18];

    printf("allocated:\n");
    for (int i = 0; i < 15; ++i) {
        testbench[i] = allocate(allocator);
        printf("%i: %p\n", i, testbench[i]);
    }

    deallocate(allocator, testbench[1]);
    deallocate(allocator, testbench[0]);
    deallocate(allocator, testbench[12]);
    printf("\ndeallocated: 1, 0, 12\n\n");

    printf("allocated:\n");
    for (int i = 15; i < 18; ++i) {
        testbench[i] = allocate(allocator);
        printf("%i: %p\n", i, testbench[i]);
    }

    return EXIT_SUCCESS;
}

// output:

// allocated:
// 0: 0x5555555596d0
// 1: 0x5555555596d8
// 2: 0x5555555596e0
// 3: 0x5555555596e8
// 4: 0x5555555596f0
// 5: 0x555555559700
// 6: 0x555555559708
// 7: 0x555555559710
// 8: 0x555555559718
// 9: 0x555555559720
// 10: 0x555555559730
// 11: 0x555555559738
// 12: 0x555555559740
// 13: 0x555555559748
// 14: 0x555555559750

// deallocated: 1, 0, 12

// allocated:
// 15: 0x555555559740
// 16: 0x5555555596d0
// 17: 0x5555555596d8
