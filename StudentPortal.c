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
            printf("Status: 🟡 On the way (ETA: %d stops away)\n\n", studentIndex - currentIndex);
            busesOnWay++;
        } else if (currentIndex == studentIndex) {
            printf("Status: 🟢 At your stop!\n\n");
        } else {
            printf("Status: 🔴 Already passed your stop.\n\n");
            busesPassed++;
        }
    }

    fclose(busFile);

    printf("Total Buses to your stop: %d\n", totalBuses);
    printf("Buses on the way: %d\n", busesOnWay);
    printf("Buses passed: %d\n", busesPassed);
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
            case 3: trackBus(); 
                    break;
            case 4: printf("Goodbye!\n"); 
                    break;
            default: printf("Invalid choice\n");
        }

    } while (choice != 4);

    return 0;
}
