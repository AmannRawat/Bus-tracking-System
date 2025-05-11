#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 100
// Track Bus for a student
void trackBus() {
    int inputRoll;
    printf("Enter your Roll Number to track bus: ");
    scanf("%d", &inputRoll);

    FILE *fp = fopen("students.txt", "r");
    if (fp == NULL) {
        printf("Student file not found.\n");
        return;
    }

    int roll, found = 0, busNo;
    char name[50], stop[50];

    while (fscanf(fp, "%d,%[^,],%[^,],%d\n", &roll, name, stop, &busNo) != EOF) {
        if (roll == inputRoll) {
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("Student not found!\n");
        return;
    }

    FILE *busFile = fopen("bus_status.txt", "r");
    if (busFile == NULL) {
        printf("Could not open bus_status.txt\n");
        return;
    }

    char line[300];
    int busesOnWay = 0, busesPassed = 0, totalBuses = 0;

    printf("\nTracking for: %s | Stop: %s | Assigned Bus No: %d\n", name, stop, busNo);
    printf("------------------------------------------------------\n");

    while (fgets(line, sizeof(line), busFile)) {
        char *busPart = strtok(line, "|");
        char *indexPart = strtok(NULL, "\n");
        if (!busPart || !indexPart) continue;

        int currentIndex = atoi(indexPart);

        char *stopToken = strtok(busPart, ",");
        int busNumber = atoi(stopToken);

        if (busNumber != busNo) continue; // Only show the student's assigned bus

        char route[MAX][50];
        int routeLen = 0;
        while ((stopToken = strtok(NULL, ",")) != NULL) {
            strcpy(route[routeLen++], stopToken);
        }

        int studentIndex = -1;
        for (int i = 0; i < routeLen; i++) {
            if (strcmp(route[i], stop) == 0) {
                studentIndex = i;
                break;
            }
        }

        if (studentIndex == -1) {
            printf("Stop not found in route.\n");
            continue;
        }

        totalBuses++;
        printf("Bus %d is currently at: %s\n", busNumber, route[currentIndex]);

        if (currentIndex < studentIndex) {
            printf("Status:  On the way (ETA: %d stops away)\n\n", studentIndex - currentIndex);
            busesOnWay++;
        } else if (currentIndex == studentIndex) {
            printf("Status:  At your stop!\n\n");
        } else {
            printf("Status:  Already passed your stop.\n\n");
            busesPassed++;
        }
    }

    fclose(busFile);

    printf("Total Buses to your stop: %d\n", totalBuses);
    printf("Buses on the way: %d\n", busesOnWay);
    printf("Buses passed: %d\n", busesPassed);
}

// Track Bus for a student (simulated STATIC movement)
void trackBus() {
    int inputRoll;
    printf("Enter your Roll Number to track bus: ");
    scanf("%d", &inputRoll);

    FILE *fp = fopen("students.txt", "r");
    if (fp == NULL) {
        printf("Student file not found.\n");
        return;
    }

    int roll, found = 0, busNo;
    char name[50], stop[50];

    while (fscanf(fp, "%d,%[^,],%[^,],%d\n", &roll, name, stop, &busNo) != EOF) {
        if (roll == inputRoll) {
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("Student not found!\n");
        return;
    }

    FILE *busFile = fopen("bus_status.txt", "r");
    if (busFile == NULL) {
        printf("Could not open bus_status.txt\n");
        return;
    }

    char line[300];
    int busesOnWay = 0, busesPassed = 0, totalBuses = 0;

    printf("\nTracking for: %s | Stop: %s | Assigned Bus No: %d\n", name, stop, busNo);
    printf("------------------------------------------------------\n");

    while (fgets(line, sizeof(line), busFile)) {
        char *busPart = strtok(line, "|");
        char *indexPart = strtok(NULL, "\n");
        if (!busPart || !indexPart) continue;

        int currentIndex = atoi(indexPart);

        char *stopToken = strtok(busPart, ",");
        int busNumber = atoi(stopToken);

        if (busNumber != busNo) continue; // Only show the student's assigned bus

        char route[MAX][50];
        int routeLen = 0;
        while ((stopToken = strtok(NULL, ",")) != NULL) {
            strcpy(route[routeLen++], stopToken);
        }

        int studentIndex = -1;
        for (int i = 0; i < routeLen; i++) {
            if (strcmp(route[i], stop) == 0) {
                studentIndex = i;
                break;
            }
        }

        if (studentIndex == -1) {
            printf("Stop not found in route.\n");
            continue;
        }

        totalBuses++;
        printf("Bus %d is currently at: %s\n", busNumber, route[currentIndex]);

        if (currentIndex < studentIndex) {
            printf("Status:  On the way (ETA: %d stops away)\n\n", studentIndex - currentIndex);
            busesOnWay++;
        } else if (currentIndex == studentIndex) {
            printf("Status:  At your stop!\n\n");
        } else {
            printf("Status:  Already passed your stop.\n\n");
            busesPassed++;
        }
    }

    fclose(busFile);

    printf("Total Buses to your stop: %d\n", totalBuses);
    printf("Buses on the way: %d\n", busesOnWay);
    printf("Buses passed: %d\n", busesPassed);
}



// Track Bus for a student (simulated dynamic movement----more modified version)
void trackMyBus() {
    char studentID[50];
    char line[256];
    printf("Enter your student ID: ");
    scanf("%s", studentID);

    // 1. Find student's stop and all possible buses
    char stopName[100];
    int busNumbers[50];
    int totalBuses = 0;

    FILE *studentFile = fopen("students.txt", "r");
    if (!studentFile) {
        printf("Error opening students.txt\n");
        return;
    }

    int foundStudent = 0;
    while (fgets(line, sizeof(line), studentFile)) {
        char *token = strtok(line, ",");
        if (token && strcmp(token, studentID) == 0) {
            strtok(NULL, ","); // Skip name
            strcpy(stopName, strtok(NULL, ",")); // Get stop
            foundStudent = 1;
            break;
        }
    }
    fclose(studentFile);

    if (!foundStudent) {
        printf("Student not found.\n");
        return;
    }

    // 2. Get all buses for this stop from bus_stops.txt
    FILE *busStopsFile = fopen("bus_stops.txt", "r");
    if (!busStopsFile) {
        printf("Error opening bus_stops.txt\n");
        return;
    }

    while (fgets(line, sizeof(line), busStopsFile)) {
        char tempLine[256];
        strcpy(tempLine, line);
        char *token = strtok(tempLine, ",");
        if (token && strcmp(token, stopName) == 0) {
            while ((token = strtok(NULL, ",")) != NULL) {
                busNumbers[totalBuses++] = atoi(token);
            }
            break;
        }
    }
    fclose(busStopsFile);

    if (totalBuses == 0) {
        printf("No buses serve your stop: %s\n", stopName);
        return;
    }

    // 3. Track buses from bus_status.txt
    FILE *busStatusFile = fopen("bus_status.txt", "r");
    if (!busStatusFile) {
        printf("Error opening bus_status.txt\n");
        return;
    }

    int bestBus = -1, bestBusIndex = -1, minDistance = MAX;
    int passedBus = -1, passedDistance = 0;
    char bestBusRoute[256], passedBusRoute[256];

    while (fgets(line, sizeof(line), busStatusFile)) {
        char originalLine[256];
        strcpy(originalLine, line);
        char *pipePtr = strrchr(originalLine, '|');
        if (!pipePtr) continue;

        *pipePtr = '\0';
        int currIndex = atoi(pipePtr + 1);
        int currentBus = atoi(originalLine);

        // Check if bus serves this stop
        int isRelevantBus = 0;
        for (int i = 0; i < totalBuses; i++) {
            if (busNumbers[i] == currentBus) {
                isRelevantBus = 1;
                break;
            }
        }
        if (!isRelevantBus) continue;

        // Parse route
        char routeCopy[256];
        strcpy(routeCopy, originalLine);
        char *routeParts[50];
        int partCount = 0;
        char *token = strtok(routeCopy, ",");
        while (token && partCount < 50) {
            routeParts[partCount++] = token;
            token = strtok(NULL, ",");
        }

        // Find student's stop in route
        int studentStopIndex = -1;
        for (int i = 1; i < partCount; i++) {
            if (strcmp(routeParts[i], stopName) == 0) {
                studentStopIndex = i - 1;
                break;
            }
        }
        if (studentStopIndex == -1) continue;

        if (currIndex < studentStopIndex) {
            int distance = studentStopIndex - currIndex;
            if (distance < minDistance) {
                minDistance = distance;
                bestBus = currentBus;
                bestBusIndex = currIndex;
                strcpy(bestBusRoute, originalLine);
            }
        } else if (currIndex > studentStopIndex) {
            passedBus = currentBus;
            passedDistance = currIndex - studentStopIndex;
            strcpy(passedBusRoute, originalLine);
        } else {
            printf("\n=== LIVE BUS TRACKING ===\n");
            printf("Bus %d is CURRENTLY at your stop: %s\n", currentBus, stopName);
            fclose(busStatusFile);
            return;
        }
    }
    fclose(busStatusFile);

    // 4. Display detailed tracking info
    printf("\n=== LIVE BUS TRACKING ===\n");
    
    if (bestBus != -1) {
        printf("\n NEXT BUS APPROACHING:\n");
        printf("Bus Number: %d\n", bestBus);
        printf("Current Stop: %s\n", strtok(bestBusRoute, ",") + 1); // Skip bus number
        printf("Stops Remaining: %d\n", minDistance);
        printf("Expected Arrival: Soon\n"); // Can add ETA calculation
    }

    if (passedBus != -1) {
        printf("\n LAST BUS PASSED:\n");
        printf("Bus Number: %d\n", passedBus);
        printf("Current Location: %s\n", strtok(passedBusRoute, ",") + 1);
        printf("Stops Ahead of You: %d\n", passedDistance);
    }

    if (bestBus == -1 && passedBus == -1) {
        printf("No buses currently on route to %s\n", stopName);
    }
}


