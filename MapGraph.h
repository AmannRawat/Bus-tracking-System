#ifndef MAP_GRAPH_H
#define MAP_GRAPH_H

#define MAX_STOPS 100
#define MAX_NAME 50

typedef struct Node {
    int dest;
    int weight;
    struct Node* next;
} Node;

typedef struct List {
    Node* head;
} List;

typedef struct Graph {
    int V;
    List* array;
    char stopNames[MAX_STOPS][MAX_NAME];
    int stopCount;
} Graph;

extern Graph* globalGraph;

Graph* createGraph(int V);
void graphPortal();
int getStopIndex(Graph* graph, char* name);
void addEdge(Graph* graph, const char* srcName, const char* destName, int weight);
void addRouteManually(Graph *graph);
void loadRoutesFromFile(Graph* graph);
void printGraph(Graph* graph);

#endif