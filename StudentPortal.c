#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 100

// Function to auto-assign bus number based on stop
// File-based bus number finder
int getBusNumber(char *stopName) {
    FILE *file = fopen("bus_stops.txt", "r");
    if (file == NULL) {
        printf("Could not open bus_stops.txt\n");
        return -1;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline
        line[strcspn(line, "\n")] = 0;

        // Split by comma
        char *token = strtok(line, ",");
        char *stop = token;
        char *busNumStr = strtok(NULL, ",");

        if (stop != NULL && busNumStr != NULL && strcmp(stopName, stop) == 0) {
            fclose(file);
            return atoi(busNumStr);
        }
    }

    fclose(file);
    return -1;  // Not found
}

// Register a student
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
    getchar(); // to consume newline
    printf("Enter Name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';  // remove trailing newline
    printf("Enter Bus Stop: ");
    fgets(stop, sizeof(stop), stdin);
    stop[strcspn(stop, "\n")] = '\0';  // remove trailing newline

    int busNo = getBusNumber(stop);
    if (busNo == 0) {
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

// Menu
int main() {
    int choice;
    do {
        printf("\n--- Student Portal ---\n");
        printf("1. Register Student\n2. Show Students\n3. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: registerStudent(); break;
            case 2: showStudents(); break;
            case 3: printf("Goodbye!\n"); break;
            default: printf("Invalid choice\n");
        }

    } while (choice != 3);

    return 0;
}
