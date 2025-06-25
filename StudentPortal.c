#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MapGraph.h"
#include "StudentPortal.h"

Graph *globalGraph = NULL;
extern Graph *globalGraph;
#define MAX 100

// Function to auto-assign bus number based on stop
int getBusNumber(char *stopName)
{
    FILE *file = fopen("bus_stops.txt", "r");
    if (file == NULL)
    {
        printf("Could not open bus_stops.txt\n");
        return -1;
    }

    char line[300];
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = 0;

        char tempLine[300];
        strcpy(tempLine, line);

        char *token = strtok(tempLine, ",");
        if (token == NULL)
            continue;

        while (*token == ' ')
            token++;
        char stop[100];
        strcpy(stop, token);

        if (strcmp(stopName, stop) == 0)
        {
            token = strtok(NULL, ",");
            if (token == NULL)
            {
                fclose(file);
                return -1;
            }

            while (token != NULL)
            {
                while (*token == ' ')
                    token++;
                int bus = atoi(token);
                if (bus > 0)
                {
                    fclose(file);
                    return bus;
                }
                token = strtok(NULL, ",");
            }
        }
    }

    fclose(file);
    return -1;
}

void registerStudent()
{
    FILE *fp = fopen("students.txt", "a");
    if (fp == NULL)
    {
        printf("Error opening student file!\n");
        return;
    }

    int roll;
    char name[50], stop[50];
    printf("Enter Roll Number: ");
    scanf("%d", &roll);
    getchar();
    printf("Enter Name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';
    printf("Enter Bus Stop: ");
    fgets(stop, sizeof(stop), stdin);
    stop[strcspn(stop, "\n")] = '\0';

    int busNo = getBusNumber(stop);
    if (busNo <= 0)
    {
        printf("❌ Invalid stop! Cannot assign a bus.\n");
    }
    else
    {
        fprintf(fp, "%d,%s,%s,%d\n", roll, name, stop, busNo);
        printf("✅ Student registered successfully! 🧑‍🎓 Assigned 🚌 Bus No: %d\n", busNo);
    }

    fclose(fp);
}

void showStudents()
{
    FILE *fp = fopen("students.txt", "r");
    if (fp == NULL)
    {
        printf("No student data found!\n");
        return;
    }

    int roll, bus;
    char name[50], stop[50];

    printf("\n📋 --- Registered Students ---\n");
    while (fscanf(fp, "%d,%[^,],%[^,],%d\n", &roll, name, stop, &bus) != EOF)
    {
        printf("🎓 Roll: %d | 🧑 Name: %s | 🛑 Stop: %s | 🚌 Bus No: %d\n", roll, name, stop, bus);
    }

    fclose(fp);
}

extern Graph *globalGraph;
extern int getStopIndex(Graph *graph, char *name);

void trackMyBus()
{
    char studentID[50];
    printf("Enter your student ID: ");
    scanf("%s", studentID);

    FILE *studentFile = fopen("students.txt", "r");
    if (!studentFile)
    {
        printf("Error opening students.txt\n");
        return;
    }

    char line[256], *stopName = NULL;
    int busNumber = -1;
    char studentName[50] = "";

    int rollNum = atoi(studentID); // Convert ID to int

    while (fgets(line, sizeof(line), studentFile))
    {
        int roll, bus;
        char name[50], stop[50];

        if (sscanf(line, "%d,%[^,],%[^,],%d", &roll, name, stop, &bus) == 4)
        {
            if (roll == rollNum)
            {
                stopName = strdup(stop);
                busNumber = bus;
                strcpy(studentName, name);
                break;
            }
        }
    }

    fclose(studentFile);

    if (!stopName || busNumber == -1)
    {
        printf("❌ Student not found or invalid data.\n");
        if (stopName)
            free(stopName);
        return;
    }
    printf("🔍 Tracking for 👤 %s | 🚌 Bus No: %d | 📍 Stop: %s\n\n", studentName, busNumber, stopName);

    FILE *busFile = fopen("bus_status.txt", "r");
    if (!busFile)
    {
        printf("Error opening bus_status.txt\n");
        free(stopName);
        return;
    }

    FILE *tempFile = fopen("bus_status_temp.txt", "w");
    if (!tempFile)
    {
        printf("Error creating temp file.\n");
        fclose(busFile);
        free(stopName);
        return;
    }

    int foundBus = 0;

    while (fgets(line, sizeof(line), busFile))
    {
        char originalLine[256];
        strcpy(originalLine, line);

        char *pipePtr = strrchr(originalLine, '|');
        if (!pipePtr)
        {
            fprintf(tempFile, "%s", originalLine);
            continue;
        }

        *pipePtr = '\0';
        int currIndex = atoi(pipePtr + 1);
        int currentBus = atoi(originalLine);

        if (currentBus != busNumber)
        {
            fprintf(tempFile, "%s|%d\n", originalLine, currIndex);
            continue;
        }

        foundBus = 1;

        char routeCopy[256];
        strcpy(routeCopy, originalLine);
        char *routeParts[50];
        int partCount = 0;

        char *token = strtok(routeCopy, ",");
        while (token && partCount < 50)
        {
            routeParts[partCount++] = token;
            token = strtok(NULL, ",");
        }

        int studentStopIndex = -1;
        for (int i = 1; i < partCount; i++)
        {
            if (strcmp(routeParts[i], stopName) == 0)
            {
                studentStopIndex = i - 1;
                break;
            }
        }

        if (studentStopIndex == -1)
        {
            printf("Error: Stop '%s' not found in Bus %d's route.\n", stopName, busNumber);
        }
        else
        {
            int totalTime = 0;
            for (int i = currIndex + 1; i <= studentStopIndex; i++)
            {
                int src = getStopIndex(globalGraph, routeParts[i]);
                int dest = getStopIndex(globalGraph, routeParts[i + 1]);
                Node *temp = globalGraph->array[src].head;
                while (temp)
                {
                    if (temp->dest == dest)
                    {
                        totalTime += temp->weight;
                        break;
                    }
                    temp = temp->next;
                }
            }

            if (currIndex < studentStopIndex)
            {
                printf("🚌 Bus %d is approaching your stop 📍 '%s'\n", busNumber, routeParts[studentStopIndex + 1]);
                printf("📍 Current stop: %s\n", routeParts[currIndex + 1]);
                printf("📌 Upcoming stops:\n");

                for (int i = currIndex + 1; i <= studentStopIndex; i++)
                {
                    int src = getStopIndex(globalGraph, routeParts[i]);
                    int dest = getStopIndex(globalGraph, routeParts[i + 1]);
                    Node *temp = globalGraph->array[src].head;
                    int t = 0;
                    while (temp)
                    {
                        if (temp->dest == dest)
                        {
                            t = temp->weight;
                            break;
                        }
                        temp = temp->next;
                    }
                    printf("  %s (%d min)\n", routeParts[i + 1], t);
                }

                printf("⏳ ETA: %d minutes\n", totalTime);
            }
            else if (currIndex == studentStopIndex)
            {
                printf("✅ Bus %d is currently at your stop: 📍 %s\n", busNumber, routeParts[currIndex + 1]);
            }
            else
            {
                int totalPassedTime = 0;
                for (int i = studentStopIndex + 1; i <= currIndex; i++)
                {
                    int src = getStopIndex(globalGraph, routeParts[i]);
                    int dest = getStopIndex(globalGraph, routeParts[i + 1]);
                    // printf("DEBUG: Trying to find edge from '%s' to '%s'\n", routeParts[i], routeParts[i + 1]);
                    // printf("DEBUG: src = %d, dest = %d\n", src, dest);
                    Node *temp = globalGraph->array[src].head;
                    while (temp)
                    {
                        if (temp->dest == dest)
                        {
                            totalPassedTime += temp->weight;
                            break;
                        }
                        temp = temp->next;
                    }
                    if (!temp)
                    {
                        printf(" Warning: No edge from '%s' to '%s'. Assuming 5 minutes.\n", routeParts[i], routeParts[i + 1]);
                        totalPassedTime += 5;
                    }
                }

                printf("⚠️ Bus %d has already passed your stop.\n", busNumber);
                printf("🕒 It passed about %d minutes ago.\n", totalPassedTime);
                printf("📍 Current location: %s\n", routeParts[currIndex + 1]);
            }
        }

        if (currIndex + 1 < partCount - 1)
        {
            currIndex++;
        }
        else
        {
            printf("🔁 Bus %d completed its route. Resetting to starting point.\n", busNumber);
            currIndex = 0;
        }
        fprintf(tempFile, "%s|%d\n", originalLine, currIndex);
    }

    fclose(busFile);
    fclose(tempFile);
    free(stopName);

    remove("bus_status.txt");
    rename("bus_status_temp.txt", "bus_status.txt");

    if (!foundBus)
    {
        printf("Bus %d not found in route data.\n", busNumber);
    }
}
void resetAllBusesToStart()
{
    FILE *input = fopen("bus_status.txt", "r");
    FILE *output = fopen("bus_status_temp.txt", "w");

    if (!input || !output)
    {
        printf("Error accessing bus status file.\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), input))
    {
        char *pipePtr = strrchr(line, '|');
        if (pipePtr)
        {
            *pipePtr = '\0';
        }
        fprintf(output, "%s|0\n", line); // reset index to 0
    }

    fclose(input);
    fclose(output);
    remove("bus_status.txt");
    rename("bus_status_temp.txt", "bus_status.txt");

    printf("🔁 All buses have been reset to the start of their routes.\n");
}

void studentPortal()
{
    int choice;
    do
    {
        printf("\n🎓 === STUDENT PORTAL === 🎓\n");
        printf("\n");
        printf("1️⃣  Register Student\n");
        printf("2️⃣  Show Registered Students\n");
        printf("3️⃣  Track My Bus 🚌\n");
        printf("4️⃣  Reset All Buses ⏮️\n");
        printf("5️⃣  🔙 Back to Main Menu\n");
        printf("---------------------------\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf("\n🔐 Registering Student...\n");
            registerStudent();
            break;
        case 2:
            printf("\n📚 Showing All Registered Students...\n");
            showStudents();
            break;
        case 3:
            printf("\n🔎 Tracking Your Bus...\n");
            trackMyBus();
            break;
        case 4:
            printf("\n🔄 Resetting All Buses to Start...\n");
            resetAllBusesToStart();
            break;
        case 5:
            printf("👋 Returning to Main Menu...\n");
            break;
        default:
            printf("⚠️ Invalid choice. Please try again.\n");
        }
    } while (choice != 5);
}
// int main()
// {
//     globalGraph = createGraph(MAX_STOPS);
//     loadRoutesFromFile(globalGraph);
//     int choice;
//     do
//     {
//         printf("\n--- Student Portal ---\n");
//         printf("1. Register Student\n2. Show Students\n3. Track My Bus\n4. Reset All Buses to Start\n5. Exit\n");
//         printf("Enter choice: ");
//         scanf("%d", &choice);

//         switch (choice)
//         {
//         case 1:
//             registerStudent();
//             break;
//         case 2:
//             showStudents();
//             break;
//         case 3:
//             trackMyBus();
//             break;
//         case 4:
//             resetAllBusesToStart();
//             break;
//         case 5:
//             printf("Goodbye!\n");
//             break;
//         default:
//             printf("Invalid choice\n");
//         }
//     } while (choice != 5);

//     return 0;
// }
