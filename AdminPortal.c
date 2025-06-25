#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AdminPortal.h"

#define MAX 100

typedef struct LogNode {
    char message[100];
    struct LogNode *next;
} LogNode;

typedef struct Bus {
    int busID;
    char driverName[50];
    LogNode *logs; // Stack for logs
    struct Bus *next;
} Bus;

Bus *head = NULL;

void pushLog(LogNode **top, const char *message) {
    LogNode *newLog = (LogNode *)malloc(sizeof(LogNode));
    strcpy(newLog->message, message);
    newLog->next = *top;
    *top = newLog;
}

void printLogs(LogNode *top) {
    printf("--- Bus Logs ---\n");
    while (top) {
        printf(" - %s\n", top->message);
        top = top->next;
    }
}

void saveBusesToFile() {
    FILE *file = fopen("admin_buses.txt", "w");
    if (!file) {
        printf("Error saving buses.\n");
        return;
    }

    Bus *temp = head;
    while (temp) {
        fprintf(file, "%d,%s\n", temp->busID, temp->driverName);

        LogNode *log = temp->logs;
        while (log) {
            fprintf(file, "LOG:%s\n", log->message);
            log = log->next;
        }
        fprintf(file, "END\n"); // End of logs for this bus
        temp = temp->next;
    }

    fclose(file);
}

void loadBusesFromFile() {
    FILE *file = fopen("admin_buses.txt", "r");
    if (!file) return;

    char line[150];
    Bus *lastBus = NULL;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        if (strncmp(line, "LOG:", 4) == 0) {
            if (lastBus) {
                pushLog(&lastBus->logs, line + 4);
            }
        } else if (strcmp(line, "END") == 0) {
            lastBus = NULL;
        } else {
            int id;
            char driver[50];
            sscanf(line, "%d,%[^\n]", &id, driver);

            Bus *newBus = (Bus *)malloc(sizeof(Bus));
            newBus->busID = id;
            strcpy(newBus->driverName, driver);
            newBus->logs = NULL;
            newBus->next = head;
            head = newBus;
            lastBus = newBus;
        }
    }

    fclose(file);
}

void addBus() {
    int id;
    char driver[50];
    printf("Enter Bus ID: ");
    scanf("%d", &id);
    printf("Enter Driver Name: ");
    scanf(" %[^\n]", driver);

    Bus *newBus = (Bus *)malloc(sizeof(Bus));
    newBus->busID = id;
    strcpy(newBus->driverName, driver);
    newBus->logs = NULL;
    pushLog(&newBus->logs, "Bus added");
    newBus->next = head;
    head = newBus;

    saveBusesToFile();
    printf("Bus added successfully!\n");
}

void removeBus() {
    int id;
    printf("Enter Bus ID to remove: ");
    scanf("%d", &id);

    Bus *temp = head, *prev = NULL;
    while (temp && temp->busID != id) {
        prev = temp;
        temp = temp->next;
    }

    if (!temp) {
        printf("Bus not found!\n");
        return;
    }

    if (prev) prev->next = temp->next;
    else head = temp->next;

    free(temp);
    saveBusesToFile();
    printf("Bus removed successfully!\n");
}

void assignDriver() {
    int id;
    char driver[50];
    printf("Enter Bus ID: ");
    scanf("%d", &id);
    printf("Enter New Driver Name: ");
    scanf(" %[^\n]", driver);

    Bus *temp = head;
    while (temp) {
        if (temp->busID == id) {
            strcpy(temp->driverName, driver);
            pushLog(&temp->logs, "Driver reassigned");
            saveBusesToFile();
            printf("Driver updated.\n");
            return;
        }
        temp = temp->next;
    }

    printf("Bus not found!\n");
}

void viewBuses() {
    Bus *temp = head;
    if (!temp) {
        printf("❌ No buses available.\n");
        return;
    }

    printf("\n🚌 --- Bus List ---\n");
    while (temp) {
        printf("🆔 ID: %d | 👨‍✈️ Driver: %s\n", temp->busID, temp->driverName);
        temp = temp->next;
    }
}

void viewLogs() {
    int id;
    printf("Enter Bus ID: ");
    scanf("%d", &id);
    Bus *temp = head;
    while (temp) {
        if (temp->busID == id) {
            printLogs(temp->logs);
            return;
        }
        temp = temp->next;
    }
    printf("Bus not found!\n");
}

void adminPortal() {
    loadBusesFromFile(); // 🔁 Load buses when admin portal starts

    int choice;
    do {
        printf("\n👮 === ADMIN CONTROL PANEL === 🔐\n\n");
        printf("1️⃣  Add Bus 🚌\n");
        printf("2️⃣  Remove Bus ❌\n");
        printf("3️⃣  Assign/Reassign Driver 👨‍✈️\n");
        printf("4️⃣  View All Buses 📋\n");
        printf("5️⃣  View Bus Logs 🗒️\n");
        printf("6️⃣  🔙 Back to Main Menu\n");
        printf("--------------------------\n");
        printf("Enter choice: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1: addBus(); break;
            case 2: removeBus(); break;
            case 3: assignDriver(); break;
            case 4: viewBuses(); break;
            case 5: viewLogs(); break;
            case 6: printf("Exiting Admin Portal...\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 6);
}
