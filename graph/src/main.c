#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int **matrix;
    int size;
} adj_matrix_t;

adj_matrix_t *create_graph(int size)
{
    if (size <= 0) return NULL;

    adj_matrix_t *graph = malloc(sizeof(adj_matrix_t));
    if (!graph)
    {
        perror("Failed to allocate memory for graph");
        return NULL;
    }

    graph->size = size;
    graph->matrix = malloc(size * sizeof(int *));

    if (!graph->matrix)
    {
        perror("Failed to allocate memory for adjacency matrix rows");
        free(graph);
        return NULL;
    }

    for (int i = 0; i < size; i++)
    {
        graph->matrix[i] = malloc(size * sizeof(int));
        if (!graph->matrix[i])
        {
            perror("Failed to allocate memory for adjacency matrix columns");
            for (int j = 0; j < i; j++)
                free(graph->matrix[j]);
            free(graph->matrix);
            free(graph);
            return NULL;
        }
    }

    return graph;
}

void free_graph(adj_matrix_t *graph)
{
    if (!graph) return;

    for (int i = 0; i < graph->size; i++)
        free(graph->matrix[i]);
    free(graph->matrix);
    free(graph);
}

void add_edge(adj_matrix_t *graph, int src, int dest, int weight)
{
    if (!graph) return;
    if (src < 0 || src >= graph->size || dest < 0 || dest >= graph->size) return;

    graph->matrix[src][dest] = weight;
}

void remove_edge(adj_matrix_t *graph, int src, int dest)
{
    if (!graph) return;
    if (src < 0 || src >= graph->size || dest < 0 || dest >= graph->size) return;

    graph->matrix[src][dest] = 0;
}

bool has_edge(adj_matrix_t *graph, int src, int dest)
{
    if (!graph) return false;
    if (src < 0 || src >= graph->size || dest < 0 || dest >= graph->size) return false;

    return graph->matrix[src][dest] != 0;
}

void print_graph(adj_matrix_t *graph)
{
    if (!graph) return;

    for (int i = 0; i < graph->size; i++)
    {
        for (int j = 0; j < graph->size; j++)
        {
            printf("%d ", graph->matrix[i][j]);
        }
        printf("\n");
    }
}

void print_neighbors(adj_matrix_t *graph, int vertex)
{
    if (!graph || vertex < 0 || vertex >= graph->size)
    {
        printf("Invalid vertex\n");
        return;
    }

    printf("Neighbors of vertex %d: ", vertex);
    bool has_neighbors = false;

    for (int i = 0; i < graph->size; i++)
    {
        if (graph->matrix[vertex][i] != 0)
        {
            printf("%d(weight:%d) ", i, graph->matrix[vertex][i]);
            has_neighbors = true;
        }
    }

    if (!has_neighbors)
    {
        printf("None");
    }
    printf("\n");
}

int main()
{
    const int vertices = 10;

    adj_matrix_t *graph = create_graph(vertices);
    if (!graph) return EXIT_FAILURE;

    add_edge(graph, 0, 1, 5);
    add_edge(graph, 1, 2, 3);
    add_edge(graph, 2, 0, 2);
    add_edge(graph, 3, 4, 4);
    add_edge(graph, 4, 5, 1);
    add_edge(graph, 5, 6, 6);
    add_edge(graph, 6, 7, 7);
    add_edge(graph, 7, 8, 8);
    add_edge(graph, 8, 9, 9);

    print_graph(graph);

    has_edge(graph, 0, 1) ? printf("Edge exists between 0 and 1\n") : printf("No edge between 0 and 1\n");
    has_edge(graph, 1, 3) ? printf("Edge exists between 1 and 3\n") : printf("No edge between 1 and 3\n");

    for (int i = 0; i < vertices; i++)
    {
        print_neighbors(graph, i);
    }

    free_graph(graph);

    return EXIT_SUCCESS;
}