#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 100

// Function to auto-assign bus number based on stop
int getBusNumber(char *stopName) {
    FILE *file = fopen("bus_stops.txt", "r");
    if (file == NULL) {
        printf("Could not open bus_stops.txt\n");
        return -1;
    }

    char line[300];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        // Make a copy of line for strtok
        char tempLine[300];
        strcpy(tempLine, line);

        char *token = strtok(tempLine, ",");
        if (token == NULL) continue;

        // Trim leading/trailing spaces
        while (*token == ' ') token++;
        char stop[100];
        strcpy(stop, token);

        if (strcmp(stopName, stop) == 0) {
            token = strtok(NULL, ",");
            if (token == NULL) {
                fclose(file);
                return -1;
            }

            // Get the first valid bus number after the stop name
            while (token != NULL) {
                while (*token == ' ') token++;  // trim leading spaces
                int bus = atoi(token);
                if (bus > 0) {
                    fclose(file);
                    return bus;
                }
                token = strtok(NULL, ",");
            }
        }
    }

    fclose(file);
    return -1; // No bus found
}

void registerStudent() {
    FILE *fp = fopen("students.txt", "a");
    if (fp == NULL) {
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
    if (busNo <= 0) {
        printf("Invalid stop! Cannot assign a bus.\n");
    } else {
        fprintf(fp, "%d,%s,%s,%d\n", roll, name, stop, busNo);
        printf("Student registered successfully! Auto-assigned Bus No: %d\n", busNo);
    }

    fclose(fp);
}

// Display registered students
void showStudents() {
    FILE *fp = fopen("students.txt", "r");
    if (fp == NULL) {
        printf("No student data found!\n");
        return;
    }

    int roll, bus;
    char name[50], stop[50];

    printf("\n--- Registered Students ---\n");
    while (fscanf(fp, "%d,%[^,],%[^,],%d\n", &roll, name, stop, &bus) != EOF) {
        printf("Roll: %d | Name: %s | Stop: %s | Bus No: %d\n", roll, name, stop, bus);
    }

    fclose(fp);
}


// Track Bus for a student (simulated dynamic movement)
void trackMyBus() {
    char studentID[50];
    printf("Enter your student ID: ");
    scanf("%s", studentID);

    // 1. Open students.txt to find the student's bus and stop
    FILE *studentFile = fopen("students.txt", "r");
    if (!studentFile) {
        printf("Error opening students.txt\n");
        return;
    }

    char line[256], *stopName = NULL;
    int busNumber = -1;

    // Search for the student in the file
    while (fgets(line, sizeof(line), studentFile)) {
        char *token = strtok(line, ",");
        if (token && strcmp(token, studentID) == 0) {
            strtok(NULL, ","); // Skip name
            stopName = strdup(strtok(NULL, ",")); // Store stop name
            busNumber = atoi(strtok(NULL, ",")); // Store bus number
            break;
        }
    }
    fclose(studentFile);

    if (!stopName || busNumber == -1) {
        printf("Student not found or invalid data.\n");
        if (stopName) free(stopName);
        return;
    }

    // 2. Open bus_status.txt to read current bus status
    FILE *busFile = fopen("bus_status.txt", "r");
    if (!busFile) {
        printf("Error opening bus_status.txt\n");
        free(stopName);
        return;
    }

    // 3. Create a temporary file for updated status
    FILE *tempFile = fopen("bus_status_temp.txt", "w");
    if (!tempFile) {
        printf("Error creating temp file.\n");
        fclose(busFile);
        free(stopName);
        return;
    }

    int foundBus = 0;

    // 4. Read each bus entry
    while (fgets(line, sizeof(line), busFile)) {
        char originalLine[256];
        strcpy(originalLine, line); // Keep a backup of the original line

        // Find the last '|' to separate route and index
        char *pipePtr = strrchr(originalLine, '|');
        if (!pipePtr) {
            fprintf(tempFile, "%s", originalLine); // Write as-is if no '|'
            continue;
        }

        *pipePtr = '\0'; // Split route and index
        int currIndex = atoi(pipePtr + 1); // Current stop index

        // Check if this is the student's bus
        int currentBus = atoi(originalLine);
        if (currentBus != busNumber) {
            fprintf(tempFile, "%s|%d\n", originalLine, currIndex); // Not the target bus
            continue;
        }

        foundBus = 1;

        // Parse the route (without modifying originalLine)
        char routeCopy[256];
        strcpy(routeCopy, originalLine);
        char *routeParts[50];
        int partCount = 0;

        // Tokenize the route (bus number + stops)
        char *token = strtok(routeCopy, ",");
        while (token && partCount < 50) {
            routeParts[partCount++] = token;
            token = strtok(NULL, ",");
        }

        // Find the student's stop in the route
        int studentStopIndex = -1;
        for (int i = 1; i < partCount; i++) {
            if (strcmp(routeParts[i], stopName) == 0) {
                studentStopIndex = i - 1; // Adjust for 0-based index
                break;
            }
        }

        if (studentStopIndex == -1) {
            printf("Error: Stop '%s' not found in Bus %d's route.\n", stopName, busNumber);
        } else if (currIndex < studentStopIndex) {
            printf("Bus %d is approaching. Current stop: %s\n", busNumber, routeParts[currIndex + 1]);
            printf("Upcoming stops:\n");
            for (int i = currIndex + 1; i <= studentStopIndex; i++) {
                printf(" - %s\n", routeParts[i + 1]);
            }
        } else if (currIndex == studentStopIndex) {
            printf("Bus %d is at your stop: %s\n", busNumber, routeParts[currIndex + 1]);
        } else {
            printf("Bus %d has passed your stop. Current location: %s\n", busNumber, routeParts[currIndex + 1]);
        }

        // 5. Move bus to next stop (if not at the end)
        if (currIndex + 1 < partCount - 1) {
            currIndex++;
        }

        // Write back the original route (before strtok modified it) with updated index
        fprintf(tempFile, "%s|%d\n", originalLine, currIndex);
    }

    fclose(busFile);
    fclose(tempFile);
    free(stopName);

    // 6. Replace old bus_status.txt with the updated one
    remove("bus_status.txt");
    rename("bus_status_temp.txt", "bus_status.txt");

    if (!foundBus) {
        printf("Bus %d not found in route data.\n", busNumber);
    }
}

int main() {
    int choice;
    do {
        printf("\n--- Student Portal ---\n");
        printf("1. Register Student\n2. Show Students\n3. Track My Bus\n4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: registerStudent(); 
                    break;
            case 2: showStudents();
                    break;
            case 3: trackMyBus(); 
                    break;
            case 4: printf("Goodbye!\n"); 
                    break;
            default: printf("Invalid choice\n");
        }

    } while (choice != 4);

    return 0;
}
