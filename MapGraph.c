#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STOPS 100
#define MAX_NAME 50
#define FILE_NAME "routes.txt"

typedef struct Node {
    int dest;
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

Node* newNode(int dest) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->dest = dest;
    node->next = NULL;
    return node;
}

Graph* createGraph(int V) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->V = V;
    graph->stopCount = 0;
    graph->array = (List*)malloc(V * sizeof(List));
    for (int i = 0; i < V; i++) graph->array[i].head = NULL;
    return graph;
}

int getStopIndex(Graph* graph, char* name) {
    for (int i = 0; i < graph->stopCount; i++) {
        if (strcmp(graph->stopNames[i], name) == 0) return i;
    }
    strcpy(graph->stopNames[graph->stopCount], name);
    return graph->stopCount++;
}

void addEdge(Graph* graph, int src, int dest) {
    Node* node = newNode(dest);
    node->next = graph->array[src].head;
    graph->array[src].head = node;

    node = newNode(src);
    node->next = graph->array[dest].head;
    graph->array[dest].head = node;
}

void saveRouteToFile(char stops[][MAX_NAME], int count) {
    FILE* file = fopen(FILE_NAME, "a");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s", stops[i]);
        if (i != count - 1) fprintf(file, ",");
    }
    fprintf(file, "\n");
    fclose(file);
}

void loadRoutesFromFile(Graph* graph) {
    FILE* file = fopen(FILE_NAME, "r");
    if (!file) return;

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        char* token;
        char* stops[MAX_STOPS];
        int count = 0;

        token = strtok(line, ",\n");
        while (token != NULL) {
            stops[count++] = strdup(token);
            token = strtok(NULL, ",\n");
        }

        for (int i = 0; i < count - 1; i++) {
            int src = getStopIndex(graph, stops[i]);
            int dest = getStopIndex(graph, stops[i + 1]);
            addEdge(graph, src, dest);
        }
        for (int i = 0; i < count; i++) free(stops[i]);
    }
    fclose(file);
}

void printGraph(Graph* graph) {
    for (int v = 0; v < graph->stopCount; ++v) {
        Node* temp = graph->array[v].head;
        printf("%s: ", graph->stopNames[v]);
        while (temp) {
            printf("-> %s ", graph->stopNames[temp->dest]);
            temp = temp->next;
        }
        printf("\n");
    }
}

int main() {
    Graph* graph = createGraph(MAX_STOPS);
    loadRoutesFromFile(graph);

    int choice;
    while (1) {
        printf("\n1. Add New Route\n2. Display Graph\n3. Exit\nEnter choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 1) {
            char input[1024];
            printf("Enter stops separated by commas (e.g., Stop1,Stop2,Stop3): ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;

            char* stops[MAX_STOPS];
            int count = 0;
            char* token = strtok(input, ",");
            while (token != NULL) {
                stops[count++] = strdup(token);
                token = strtok(NULL, ",");
            }

            for (int i = 0; i < count - 1; i++) {
                int src = getStopIndex(graph, stops[i]);
                int dest = getStopIndex(graph, stops[i + 1]);
                addEdge(graph, src, dest);
            }
            char routeToSave[MAX_STOPS][MAX_NAME];
            for (int i = 0; i < count; i++) {
                strncpy(routeToSave[i], stops[i], MAX_NAME);
                free(stops[i]);
            }
            saveRouteToFile(routeToSave, count);
        }
        else if (choice == 2) {
            printGraph(graph);
        }
        else break;
    }
    return 0;
}
