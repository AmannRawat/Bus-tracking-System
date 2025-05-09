#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

typedef struct LogNode {
    char message[100];
    struct LogNode* next;
} LogNode;

typedef struct Bus {
    int busID;
    char driverName[50];
    LogNode* logs; // Stack for logs
    struct Bus* next;
} Bus;

Bus* head = NULL;

// Push to log stack
void pushLog(LogNode** top, const char* message) {
    LogNode* newLog = (LogNode*)malloc(sizeof(LogNode));
    strcpy(newLog->message, message);
    newLog->next = *top;
    *top = newLog;
}

// Print stack (latest to oldest)
void printLogs(LogNode* top) {
    printf("--- Bus Logs ---\n");
    while (top) {
        printf(" - %s\n", top->message);
        top = top->next;
    }
}

// Add a new bus
void addBus() {
    int id;
    char driver[50];
    printf("Enter Bus ID: ");
    scanf("%d", &id);
    printf("Enter Driver Name: ");
    scanf(" %[^\n]", driver);

    Bus* newBus = (Bus*)malloc(sizeof(Bus));
    newBus->busID = id;
    strcpy(newBus->driverName, driver);
    newBus->logs = NULL;
    pushLog(&newBus->logs, "Bus added");
    newBus->next = head;
    head = newBus;

    printf("Bus added successfully!\n");
}

// Remove a bus
void removeBus() {
    int id;
    printf("Enter Bus ID to remove: ");
    scanf("%d", &id);

    Bus* temp = head, *prev = NULL;
    while (temp != NULL && temp->busID != id) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Bus not found!\n");
        return;
    }

    if (prev == NULL) head = temp->next;
    else prev->next = temp->next;

    free(temp);
    printf("Bus removed successfully!\n");
}

// Assign a new driver
void assignDriver() {
    int id;
    char driver[50];
    printf("Enter Bus ID: ");
    scanf("%d", &id);
    printf("Enter New Driver Name: ");
    scanf(" %[^\n]", driver);

    Bus* temp = head;
    while (temp) {
        if (temp->busID == id) {
            strcpy(temp->driverName, driver);
            pushLog(&temp->logs, "Driver reassigned");
            printf("Driver updated.\n");
            return;
        }
        temp = temp->next;
    }

    printf("Bus not found!\n");
}

// View all buses
void viewBuses() {
    Bus* temp = head;
    printf("--- Bus List ---\n");
    while (temp) {
        printf("Bus ID: %d | Driver: %s\n", temp->busID, temp->driverName);
        temp = temp->next;
    }
}

// View logs for a bus
void viewLogs() {
    int id;
    printf("Enter Bus ID: ");
    scanf("%d", &id);
    Bus* temp = head;
    while (temp) {
        if (temp->busID == id) {
            printLogs(temp->logs);
            return;
        }
        temp = temp->next;
    }
    printf("Bus not found!\n");
}

// Main admin menu
int main() {
    int choice;
    do {
        printf("\n--- Admin Portal ---\n");
        printf("1. Add Bus\n2. Remove Bus\n3. Assign/Reassign Driver\n4. View Buses\n5. View Bus Logs\n6. Exit\n");
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

    return 0;
}