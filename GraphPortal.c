#include <stdio.h>
#include "MapGraph.h"

extern Graph *globalGraph;

void graphPortal()
{
    int choice;

    do
    {
        printf("\n🗺️ === GRAPH TOOLS === 🗺️\n");
        printf("\n");
        printf("1️⃣  Display Bus Route Graph\n");
        printf("2️⃣  Add Route Manually ➕\n");
        printf("3️⃣  🔙 Back to Main Menu\n");
        printf("--------------------------\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
        case 1:
            printGraph(globalGraph);
            break;
        case 2:
            addRouteManually(globalGraph);
            break;
        case 3:
            printf("Returning to main menu...\n");
            break;
        default:
            printf("Invalid choice. Try again.\n");
        }

    } while (choice != 3);
}
