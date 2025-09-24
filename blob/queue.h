#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>

typedef struct Node
{
    void *data;
    struct Node *next;
} Node;

typedef struct
{
    size_t size;
    Node *head;
    Node *tail;
} Queue;

Queue *create_queue();
void free_queue(Queue *queue);
int enqueue(Queue *queue, void *data);
void *dequeue(Queue *queue);

#endif