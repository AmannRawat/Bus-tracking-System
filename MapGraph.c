#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_STOPS 100
#define MAX_NAME 50
#define FILE_NAME "route_times.txt"

typedef struct Node
{
    int dest;
    int weight; // time in minutes between stops
    struct Node *next;
} Node;

typedef struct List
{
    Node *head;
} List;

typedef struct Graph
{
    int V;
    List *array;
    char stopNames[MAX_STOPS][MAX_NAME];
    int stopCount;
} Graph;

Node *newNode(int dest, int weight)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->dest = dest;
    node->weight = weight;
    node->next = NULL;
    return node;
}

Graph *createGraph(int V)
{
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    graph->V = V;
    graph->stopCount = 0;
    graph->array = (List *)malloc(V * sizeof(List));
    for (int i = 0; i < V; i++)
        graph->array[i].head = NULL;
    return graph;
}

int getStopIndex(Graph *graph, char *name)
{
    for (int i = 0; i < graph->stopCount; i++)
    {
        if (strcmp(graph->stopNames[i], name) == 0)
            return i;
    }
    if (graph->stopCount >= MAX_STOPS) {
    printf("Maximum stops reached!\n");
    return -1;
}
    strcpy(graph->stopNames[graph->stopCount], name);
    return graph->stopCount++;
}

// void addEdge(Graph* graph, int src, int dest, int weight) {
//     Node* node = newNode(dest, weight);
//     node->next = graph->array[src].head;
//     graph->array[src].head = node;

//     node = newNode(src, weight);
//     node->next = graph->array[dest].head;
//     graph->array[dest].head = node;
// }
void addEdge(Graph *graph, const char *srcName, const char *destName, int weight)
{
    int src = getStopIndex(graph, (char *)srcName);
    int dest = getStopIndex(graph, (char *)destName);

    if (src == -1)
    {
        src = graph->stopCount;
        strcpy(graph->stopNames[src], srcName);
        graph->stopCount++;
    }
    if (dest == -1)
    {
        dest = graph->stopCount;
        strcpy(graph->stopNames[dest], destName);
        graph->stopCount++;
    }

    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    // Undirected Graph: Add reverse edge
    Node *reverseNode = (Node *)malloc(sizeof(Node));
    reverseNode->dest = src;
    reverseNode->weight = weight;
    reverseNode->next = graph->array[dest].head;
    graph->array[dest].head = reverseNode;
}

void loadRoutesFromFile(Graph *graph)
{
    FILE *file = fopen("route_times.txt", "r");
    if (!file)
    {
        printf("Error opening route_times.txt\n");
        return;
    }

    char line[500];
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = '\0'; // remove newline

        char *token = strtok(line, ",");
        char *prevStop = NULL;

        while (token)
        {
            char stopName[100];
            int weight = 5; // default weight

            char *colon = strchr(token, ':');
            if (colon)
            {
                *colon = '\0';
                strcpy(stopName, token);
                weight = atoi(colon + 1);
            }
            else
            {
                strcpy(stopName, token); // If no weight, assume it's just a stop
            }

            if (prevStop)
            {
                addEdge(graph, prevStop, stopName, weight);
            }

            prevStop = strdup(stopName); // remember this stop for next edge
            token = strtok(NULL, ",");
        }
    }

    fclose(file);
}

void printGraph(Graph *graph)
{
    for (int v = 0; v < graph->stopCount; ++v)
    {
        Node *temp = graph->array[v].head;
        printf("%s: ", graph->stopNames[v]);
        while (temp)
        {
            printf("-> %s (%d min) ", graph->stopNames[temp->dest], temp->weight);
            temp = temp->next;
        }
        printf("\n");
    }
}

void addRouteManually(Graph *graph)
{
    char input[1024];
    char *stops[MAX_STOPS];
    int count = 0;

    printf("Enter stops separated by commas (e.g., Stop1,Stop2,Stop3): ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    char *token = strtok(input, ",");
    while (token && count < MAX_STOPS)
    {
        stops[count++] = strdup(token);
        token = strtok(NULL, ",");
    }

    char choice;
    printf("Do you want to enter times manually (y/n)? ");
    scanf(" %c", &choice);
    getchar();

    FILE *file = fopen(FILE_NAME, "a");
    if (!file)
    {
        printf("Error opening file for route saving!\n");
        return;
    }

    fprintf(file, "%s", stops[0]);

    for (int i = 0; i < count - 1; i++)
    {
        int time;
        if (choice == 'y' || choice == 'Y')
        {
            printf("Enter time (in minutes) from %s to %s: ", stops[i], stops[i + 1]);
            scanf("%d", &time);
            getchar();
        }
        else
        {
            time = (rand() % 6) + 5; // Random between 5 and 10
        }

       addEdge(graph, stops[i], stops[i + 1], time);  //

        fprintf(file, ":%d,%s", time, stops[i + 1]);
    }
    fprintf(file, "\n");
    fclose(file);

    for (int i = 0; i < count; i++)
        free(stops[i]);
    printf("Route added successfully!\n");
}
// int main() {
//     srand(time(NULL));
//     Graph* graph = createGraph(MAX_STOPS);
//     loadRoutesFromFile(graph);

//     int choice;
//     while (1) {
//         printf("\n1. Add New Route\n2. Display Graph\n3. Exit\nEnter choice: ");
//         scanf("%d", &choice);
//         getchar();

//         switch (choice) {
//             case 1: addRouteManually(graph); break;
//             case 2: printGraph(graph); break;
//             case 3: return 0;
//             default: printf("Invalid choice!\n");
//         }
//     }
// }