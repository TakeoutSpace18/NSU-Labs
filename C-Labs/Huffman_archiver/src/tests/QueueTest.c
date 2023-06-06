#include <stdlib.h>
#include <stdio.h>

#include "../Queue.h"

int main(void) {

    Queue* q = CreateQueue();
    Push(q, 1);
    Push(q, 2);
        printf("%i\n", Pop(q));
        printf("%i\n", Pop(q));
    Push(q, 3);
    Push(q, 4);
    Push(q, 5);
    Push(q, 6);

    MakeEmpty(q);

    while(!IsEmpty(q)) {
        printf("%i\n", Pop(q));
    }

    Push(q, 523);
    Push(q, 52);
    printf("%i\n", Pop(q));
    printf("%i\n", Pop(q));

    DestroyQueue(q);
    return 0;
}
