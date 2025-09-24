#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    Queue *q = create_queue();

    enqueue(q, (void *)(intptr_t)10);
    enqueue(q, (void *)(intptr_t)20);

    printf("Dequeued: %d\n", (int)(intptr_t)dequeue(q));
    printf("Dequeued: %d\n", (int)(intptr_t)dequeue(q));

    free_queue(q);

    return EXIT_SUCCESS;
}