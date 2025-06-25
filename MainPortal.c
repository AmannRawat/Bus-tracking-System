#include <stdio.h>
#include <stdlib.h>

#include "StudentPortal.h"
#include "AdminPortal.h"
#include "MapGraph.h"
#include "GraphPortal.h"
#include "Security.h"

int main()
{
    globalGraph = createGraph(MAX_STOPS); // Load graph at start
    loadRoutesFromFile(globalGraph);
    int choice;

    do
    {
        printf("\n=====================================\n");
        printf("🚍  WELCOME TO BUS TRACKING SYSTEM  🚍\n");
        printf("=====================================\n");
        printf("1️⃣  Student Portal\n");
        printf("2️⃣  Admin Portal\n");
        printf("3️⃣  Graph Tools\n");
        printf("4️⃣  ❌ Exit\n");
        printf("-------------------------------------\n");
        printf("Enter your choice: ");

        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
        case 1:
            studentPortal();
            break;
        case 2:
            if (verifyAdminPassword())
            {
                adminPortal();
            }
            else
            {
                printf("\033[1;31mAccess denied. Returning to main menu.\033[0m\n");
            }
            break;
        case 3:
            graphPortal();
            // graphOperations();
            break;
        case 4:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (choice != 4);

    return 0;
}
