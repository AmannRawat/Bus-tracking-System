#include <stdio.h>
#include <string.h>
#include <conio.h>  // For _getch()

const char *correctPassword = "admin123";

int verifyAdminPassword() {
    char password[20];
    char ch;
    int index = 0;
    
    printf("🔐 Enter Admin Password: ");
    
    // Read each character without displaying it
    while (1) {
        ch = _getch();  // read a char without echo

        if (ch == 13) {  // Enter key
            password[index] = '\0';
            break;
        } else if (ch == 8 && index > 0) {  // Backspace
            index--;
            printf("\b \b");  // erase last star
        } else if (index < sizeof(password) - 1) {
            password[index++] = ch;
            printf("*");  // Show a star instead of actual character
        }
    }

    // Now check the password
    if (strcmp(password, correctPassword) == 0) {
        printf("\n✅ Access Granted!\n");
        return 1;
    } else {
        printf("\n❌ Wrong password!\n");
        return 0;
    }
}
