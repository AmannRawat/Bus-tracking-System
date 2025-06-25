#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STOPS 100
#define MAX_NAME 50
#define MAX_CAPACITY 5

typedef struct {
    char stopName[MAX_NAME];
} Stop;

typedef struct {
    int busID;
    Stop stops[MAX_STOPS];
    int stopCount;
    int currentStopIndex;
    int studentsBoarded;
} BusRoute;

BusRoute route;

// Load stops from file (each line = route for busID)
void loadRoute(int busID) {
    FILE* file = fopen("routes.txt", "r");
    if (!file) {
        printf("Could not open routes.txt\n");
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        char* token;
        char* stops[MAX_STOPS];
        int count = 0;

        token = strtok(line, ",\n");
        while (token) {
            strncpy(route.stops[count++].stopName, token, MAX_NAME);
            token = strtok(NULL, ",\n");
        }

        route.busID = busID;
        route.stopCount = count;
        route.currentStopIndex = 0;
        route.studentsBoarded = 0;
        break; // Load only one route for demo
    }

    fclose(file);
}

// Check if students are waiting at this stop
int studentsWaitingAtStop(const char* stop) {
    FILE* file = fopen("students.txt", "r");
    if (!file) return 0;

    int roll, busNo;
    char name[50], studentStop[50];
    while (fscanf(file, "%d,%[^,],%[^,],%d\n", &roll, name, studentStop, &busNo) != EOF) {
        if (strcmp(studentStop, stop) == 0 && busNo == route.busID) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

// Simulate moving to next stop
void moveToNextStop() {
    if (route.currentStopIndex >= route.stopCount) {
        printf("Route completed.\n");
        return;
    }

    const char* current = route.stops[route.currentStopIndex].stopName;
    printf("\n Current Location: %s\n", current);

    if (studentsWaitingAtStop(current)) {
        printf(" Pick-up Alert: Students waiting at this stop!\n");
        if (route.studentsBoarded < MAX_CAPACITY) {
            route.studentsBoarded++;
            printf(" Student boarded. Total onboard: %d\n", route.studentsBoarded);
        } else {
            printf(" Bus is full! Cannot board more students.\n");
        }
    }

    if (route.currentStopIndex + 1 < route.stopCount) {
        const char* next = route.stops[route.currentStopIndex + 1].stopName;
        printf("  Next Stop: %s\n", next);
    } else {
        printf(" Final stop reached.\n");
    }

    if (route.studentsBoarded == MAX_CAPACITY) {
        printf(" Bus is FULL. No more students can be boarded.\n");
    }

    route.currentStopIndex++;
}

int main() {
    int busID;
    printf("Enter your Bus ID: ");
    scanf("%d", &busID);

    loadRoute(busID);

    int choice;
    do {
        printf("\n--- Driver Portal ---\n");
        printf("1. Show Current & Next Stop\n2. Move to Next Stop\n3. Exit\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (route.currentStopIndex < route.stopCount)
                    printf("Current: %s | Next: %s\n",
                           route.stops[route.currentStopIndex].stopName,
                           route.currentStopIndex + 1 < route.stopCount ?
                           route.stops[route.currentStopIndex + 1].stopName : "None");
                else
                    printf("All stops completed.\n");
                break;
            case 2:
                moveToNextStop();
                break;
            case 3:
                printf("Exiting driver portal.\n");
                break;
            default:
                printf("Invalid choice.\n");
        }

    } while (choice != 3);

    return 0;
}