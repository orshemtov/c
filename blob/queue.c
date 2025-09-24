#include "queue.h"

Queue *create_queue()
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    if (!queue) return NULL;

    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;

    return queue;
}

void free_queue(Queue *queue)
{
    if (!queue) return;

    Node *current = queue->head;
    while (current)
    {
        Node *temp = current;
        current = current->next;
        free(temp);
    }

    free(queue);
}

int enqueue(Queue *queue, void *data)
{
    if (!queue) return -1;

    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node) return -1;

    new_node->data = data;
    new_node->next = NULL;

    if (queue->tail)
    {
        queue->tail->next = new_node;
    }
    else
    {
        queue->head = new_node;
    }

    queue->tail = new_node;
    queue->size++;

    return 0;
}

void *dequeue(Queue *queue)
{
    if (!queue || !queue->head) return NULL;

    Node *temp = queue->head;
    void *data = temp->data;

    queue->head = queue->head->next;
    if (!queue->head)
    {
        queue->tail = NULL;
    }

    free(temp);
    queue->size--;

    return data;
}