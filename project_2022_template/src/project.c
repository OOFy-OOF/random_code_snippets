#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DESC_LEN 100 // Assume the user will not use a description longer than 100 characters
#define MAX_FILENAME_LEN 100 // Assume the user will not use a file title longer than 100 characters

// Basic meeting struct to be stored in a dynamic array
typedef struct meeting { 
    char description[100];
    int month;
    int day;
    int hour;
} meeting;

meeting *meetings;
int num_meetings = 0; 
int meetings_capacity = 0;


void add_meeting(char description[], int month, int day, int hour,int print_success) {
    // The three following ifs check that the values for time of the meeting are valid
    if (hour < 0 || hour > 23) {
        printf("Error: Invalid input value\n");
        return;
    }
    if (month < 0 || month > 12) {
        printf("Error: Invalid input value\n");
        return;
    }
    if (day < 0 || day > 31) {
        printf("Error: Invalid input value\n");
        return;
    }

    // Find the index to insert the new meeting
    int index = 0;
    while (index < num_meetings && (meetings[index].month < month ||
           (meetings[index].month == month && meetings[index].day < day) ||
           (meetings[index].month == month && meetings[index].day == day && meetings[index].hour < hour))) {
        index++;
    }
    
    // Check if there is already a meeting scheduled at the same time
    if (index < num_meetings && meetings[index].month == month && meetings[index].day == day && meetings[index].hour == hour) {
        printf("Error: Meeting already scheduled at this time.\n");
        return;
    }
    
    // Check if the array needs to be resized and reallocate memory accordingly
    if (num_meetings == meetings_capacity) {
        meetings_capacity += 1;
        meetings = realloc(meetings, meetings_capacity * sizeof(meeting));
        if (meetings == NULL) {
            exit(1);
        }
    }

    // Shift struct array to make space for the new meeting
    for (int i = num_meetings - 1; i >= index; i--) {
        meetings[i + 1] = meetings[i];
    }
    
    // Create the new meeting and insert it at the correct time
    strcpy(meetings[index].description, description);
    meetings[index].month = month;
    meetings[index].day = day;
    meetings[index].hour = hour;
    num_meetings++;
    
    /* This is to suppress the spam of SUCCESS messages when add_meeting is called by load_from_file 
    as it loads in the previously saved meetings from a file by adding each back into memory individually*/
    if (print_success) { 
        printf("SUCCESS\n");
    }
}


void delete_meeting(int month, int day, int hour) {
    int deleted = 0;
    // Removes meeting from the allocated memory
    for (int i = 0; i < num_meetings; i++) {
        if (meetings[i].month == month && meetings[i].day == day && meetings[i].hour == hour) {
            for (int j = i; j < num_meetings-1; j++) {
                meetings[j] = meetings[j+1];
            }
            num_meetings--;
            meetings = (meeting*) realloc(meetings, num_meetings * sizeof(meeting)); 
            deleted = 1;
            printf("SUCCESS\n");
            break;
        }
    }
    if (!deleted) {
        printf("Error: No meeting scheduled at this time.\n");
    }
}


void print_calendar() {
    for (int i = 0; i < num_meetings; i++) {
        // Prints the meeting list in the desired format required by the assignment
        printf("%s %02d.%02d at %02d\n", meetings[i].description, meetings[i].day, meetings[i].month, meetings[i].hour); 
        
    }
    printf("SUCCESS\n");
}


void save_to_file(char filename[]) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file.\n");
        return;
    }
    // Loop to add each meeting into the file until there are none left
    for (int i = 0; i < num_meetings; i++) { 
        fprintf(file, "%s %02d.%02d at %02d\n", meetings[i].description, meetings[i].day, meetings[i].month, meetings[i].hour);     
    }
    fclose(file);
    printf("SUCCESS\n");
}


void load_from_file(char filename[]) {
    // Open file as read
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file.\n");
        return;
    }
    num_meetings = 0;
    char description[MAX_DESC_LEN];
    int month, day, hour;
    while (fscanf(file, "%99s %d.%d at %d\n", description, &day, &month, &hour) == 4) {
        add_meeting(description, month, day, hour, 0);
    }
    fclose(file);
    printf("SUCCESS\n");
}


/* Not quite clear on if the quit function should return SUCCESS as well since we are technically terminating the program with exit, 
I will leave a commented out version here, not that it matters*/
void quit_program() {
    // Frees the dynamically allocated meetings to avoid memory leaks upon program termination
    free(meetings);
    // printf("SUCCESS\n");
    exit(0);
}


// Main function is pretty straight forward, read the first letter with scanf and split into different cases for the commands each letter corresponds to
int main() {
    char command, desc[MAX_DESC_LEN], filename[MAX_FILENAME_LEN];
    int month, day, hour;


    while (1) {
        scanf(" %c", &command);
        switch (command) {
            case 'A':
                scanf("%s %d %d %d", desc, &month, &day, &hour);
                add_meeting(desc, month, day, hour,1);
                break;
            case 'D':
                scanf("%d %d %d", &month, &day, &hour);
                delete_meeting(month, day, hour);
                break;
            case 'L':
                print_calendar();
                break;
            case 'W':
                scanf("%s", filename);
                save_to_file(filename);
                break;
            case 'O':
                scanf("%s", filename);
                load_from_file(filename);
                break;
            case 'Q':
                quit_program();
                break;
        }
    }

    return 0;
}



