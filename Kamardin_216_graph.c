#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct Bridges
{
    int strong;
    int weak;
} Bridge;

struct Edge
{
    int to_v;
    bool removed;
    struct Node* to_node;
    struct Edge* reversed_edge;
    struct Edge* next;
} Edge;

struct Node
{
    int vertex;
    bool been;
    bool weak;
    bool strong;
    struct Edge* edges;
    struct Edge* reversed_edges;
    struct Node* next;
} Node;

struct Node* createNode(int i, struct Node* graph)
{
    struct Node* node = malloc(sizeof(*node));
    node->vertex = i;
    node->edges = NULL;
    node->reversed_edges = NULL;
    node->been = false;
    node->weak = false;
    node->strong = false;
    node->next = graph;
    return node;
}

struct Edge* createEdge(int to_v, struct Edge* edges)
{
    struct Edge* edge = malloc(sizeof(*edge));
    edge->to_v = to_v;
    edge->removed = false;
    edge->to_node = NULL;
    edge->reversed_edge = NULL;
    edge->next = edges;
    return edge;
}

void connectGraph(struct Node* graph)
{
    int to_v;
    struct Edge* edge;
    struct Node* to_node, *start = graph;
    while (graph != NULL)
    {
        edge = graph->edges;
        while (edge != NULL)
        {
            to_node = start;
            to_v = edge->to_v;
            while (to_node->vertex != to_v) to_node = to_node->next;
            edge->to_node = to_node;
            to_node->reversed_edges = createEdge(graph->vertex, to_node->reversed_edges);
            to_node->reversed_edges->to_node = graph;
            edge->reversed_edge = to_node->reversed_edges;
            edge = edge->next;
        }
        graph = graph->next;
    }
}

struct Node* inputGraph()
{
    struct Node* graph = NULL;
    int vertices, num_of_neighbours, to_v;
    scanf("%d", &vertices);
    for (int i = 1; i <= vertices; ++i)
    {
        graph = createNode(i, graph);
        scanf("%d", &num_of_neighbours);
        for (int j = 0; j < num_of_neighbours; ++j)
        {
            scanf("%d", &to_v);
            graph->edges = createEdge(to_v, graph->edges);
        }
    }
    connectGraph(graph);
    return graph;
}
void dfsWeak(struct Node* node, int w)
{
    struct Edge* edge;
    node->weak = w;
    for (edge = node->edges; edge != NULL; edge = edge->next)if (!edge->removed) if (!edge->to_node->weak) dfsWeak(edge->to_node, w);
    for (edge = node->reversed_edges; edge != NULL; edge = edge->next)if (!edge->removed) if (!edge->to_node->weak) dfsWeak(edge->to_node, w);
}

int weakComponentsCount(struct Node* start)
{
    int w = 0;
    struct Node* node;
    for (node = start; node != NULL; node = node->next)if (!node->weak) dfsWeak(node, ++w);
    return w;
}

void dfs(struct Node* node, struct Edge** path)
{
    struct Edge* edge;
    node->been = true;
    for (edge = node->edges; edge != NULL; edge = edge->next)if (!edge->removed)if (!edge->to_node->been) dfs(edge->to_node, path);
    *path = createEdge(node->vertex, *path);
    (*path)->to_node = node;
}

void dfsReversed(struct Node* node, int s)
{
    struct Edge* edge;
    node->strong = s;
    for (edge = node->reversed_edges; edge != NULL; edge = edge->next) if (!edge->removed) if (!edge->to_node->strong) dfsReversed(edge->to_node, s);
}

void downFlags(struct Node* graph)
{
    while (graph != NULL)
    {
        graph->strong = false;
        graph->been = false;
        graph->weak = false;
        graph = graph->next;
    }
}

void freeEdges(struct Edge* edge)
{
    struct Edge* next;
    while (edge != NULL)
    {
        next = edge->next;
        free(edge);
        edge = next;
    }
}

void freeGraph(struct Node* graph)
{
    struct Node* next;
    while (graph != NULL)
    {
        freeEdges(graph->edges);
        freeEdges(graph->reversed_edges);
        next = graph->next;
        free(graph);
        graph = next;
    }
}

int strongComponentsCount(struct Node* start)
{
    int strongComponents = 0;
    struct Edge* path = NULL, *edge = NULL;
    struct Node* node;
    for (node = start; node != NULL; node = node->next) if (!node->been) dfs(node, &path);
    for (edge = path; edge != NULL; edge = edge->next) if (!edge->to_node->strong) dfsReversed(edge->to_node, ++strongComponents);
    freeEdges(path);
    return strongComponents;
}

struct Bridges countBridges(int w, int s, struct Node* graph)
{
    struct Bridges bridges;
    bridges.strong = 0;
    bridges.weak = 0;
    int currents = -1, currentw = -1;
    struct Node* node;
    struct Edge* edge;
    for (node = graph; node != NULL; node = node->next)
    {
        for (edge = node->edges; edge != NULL; edge = edge->next)
        {
            downFlags(graph);
            edge->removed = true;
            edge->reversed_edge->removed = true;
            currents = strongComponentsCount(graph);
            currentw = weakComponentsCount(graph);
            edge->removed = false;
            edge->reversed_edge->removed = false;
            if (currents > s) ++bridges.strong;
            if (currentw > w) ++bridges.weak;
        }
    }
    return bridges;
}
int main()
{
    struct Node* graph = inputGraph();
    int weakComponents, strongComponents;
    strongComponents = strongComponentsCount(graph);
    weakComponents = weakComponentsCount(graph);
    struct Bridges bridges = countBridges(weakComponents, strongComponents, graph);
    printf("\n%d %d\n%d %d\n", strongComponents, bridges.strong, weakComponents, bridges.weak);
    freeGraph(graph);
    return 0;
}
