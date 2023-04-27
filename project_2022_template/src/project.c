#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_INPUT_LEN 150 // Assume the user will not input a command longer than 150 characters
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
    if (month < 0 || month > 12) {
        printf("Month cannot be less than 1 or greater than 12.\n");
        return;
    }
    if (hour < 0 || hour > 23) {
        printf("Hour cannot be negative or greater than 23.\n");
        return;
    }
    if (day < 0 || day > 31) {
        printf("Day cannot be less than 1 or greater than 31.\n");
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
        printf("The time slot %02d.%02d at %02d is already allocated.\n", day, month, hour);
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
    /* Same check as in add_meeting to match the output of the submission checker, this is super redundant because you cannot create a meeting 
    with invalid values to begin with therefore the delete command will output the meeting not found error anyways and do nothing */
    if (month < 0 || month > 12) {
        printf("Month cannot be less than 1 or greater than 12.\n");
        return;
    }
    if (hour < 0 || hour > 23) {
        printf("Hour cannot be negative or greater than 23.\n");
        return;
    }
    if (day < 0 || day > 31) {
        printf("Day cannot be less than 1 or greater than 31.\n");
        return;
    }
    int deleted = 0;
    // Removes meeting from the allocated memory
    for (int i = 0; i < num_meetings; i++) {
        if (meetings[i].month == month && meetings[i].day == day && meetings[i].hour == hour) {
            for (int j = i; j < num_meetings-1; j++) {
                meetings[j] = meetings[j+1];
            }
            // Reduce the number of meetings and meeting capacity left after the deletion
            num_meetings--;
            meetings_capacity--; 
            /* If you look through all my failed submissions you will notice that I completely forgot that 
            I didn't decrease the meeting capacity each time something is deleted which completely breaks memory allocation D:, good thing I figured it out*/
            meetings = (meeting*) realloc(meetings, num_meetings * sizeof(meeting));  
            deleted = 1;
            printf("SUCCESS\n");
            break;
        }
    }
    if (!deleted) {
        printf("The time slot %02d.%02d at %02d is not in the calendar.\n", day, month, hour);
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
    // Write to file with user defined filename
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file.\n");
        return;
    }
    // Loop to add each meeting stored in memory into the file one by one
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
        printf("Cannot open file %s for reading.\n", filename);
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
I will leave it here, not that it matters*/
void quit_program() {
    // Frees the dynamically allocated meetings to avoid memory leaks upon program termination
    free(meetings);
    printf("SUCCESS\n");
    exit(0);
}


int main() {
    char input[MAX_INPUT_LEN];
    char command, desc[MAX_DESC_LEN], filename[MAX_FILENAME_LEN];
    int month, day, hour;
    int sscanf_retval; // variable to hold the return value of sscanf

    while (1) {
        fgets(input, MAX_INPUT_LEN, stdin);
        
        // remove trailing newline character
        input[strcspn(input, "\n")] = '\0';

        sscanf_retval = sscanf(input, " %c", &command);

        if (sscanf_retval == EOF || sscanf_retval == 0) { // handle invalid input
            printf("Invalid command\n");
            continue;
        }

        switch (command) {
            case 'A':;
                /* assign tokens instead of using sscanf on the whole command to avoid using a defined split pattern, allowing us to detect and report if there are more arguments than needed, 
                sscanf does not suffice for the initial split because it will ignore the extra arguments due to the set format, 
                this will be repeated in all cases that have arguments, sscanf is then used on the month, day, and hour arguments (if present) to check that they are actually numbers*/
                char* atok = strtok(input, " ");
                if (atok == NULL) {
                    printf("A should be followed by exactly 4 arguments.\n");
                    continue;
                }
                int a_argc = 0;
                while (atok != NULL) {
                    a_argc++;
                    if (a_argc == 1) {
                        command = atok[0];
                    } else if (a_argc == 2) {
                        // Copy to avoid losing data
                        strncpy(desc, atok, MAX_DESC_LEN);

                    } else if (a_argc == 3) {
                        if (sscanf(atok, "%d", &month) != 1) { // Check if the input month is actually a number
                            printf("Invalid month argument.\n");
                            break;
                        }
                    } else if (a_argc == 4) {
                        if (sscanf(atok, "%d", &day) != 1) { // Check if the input for day is actually a number
                            printf("Invalid day argument.\n");
                            break;
                        }
                    } else if (a_argc == 5) {
                        if (sscanf(atok, "%d", &hour) != 1) { // Check if the input for hour is actually a number
                            printf("Invalid hour argument.\n");
                            continue;
                        }
                    } else {
                        break;
                    }
                    atok = strtok(NULL, " ");
                }
                if (a_argc != 5) {
                    printf("A should be followed by exactly 4 arguments.\n");
                    break;
                }
                add_meeting(desc, month, day, hour, 1);
                break;


            case 'D':;
                char* dtok = strtok(input, " ");
                if (dtok == NULL) {
                    printf("D should be followed by exactly 3 arguments.\n");
                    continue;
                }
                int d_argc = 0;
                while (dtok != NULL) {
                    d_argc++;
                    if (d_argc == 1) {
                        command = dtok[0];
                    } else if (d_argc == 2) {
                        if (sscanf(dtok, "%d", &month) != 1) {
                            printf("Invalid month argument.\n");
                            break;
                        }
                    } else if (d_argc == 3) {
                        if (sscanf(dtok, "%d", &day) != 1) {
                            printf("Invalid day argument.\n");
                            break;
                        }
                    } else if (d_argc == 4) {
                        if (sscanf(dtok, "%d", &hour) != 1) {
                            printf("Invalid hour argument.\n");
                            continue;
                        }
                    } else {
                        break;
                    }
                    dtok = strtok(NULL, " ");
                }
                if (d_argc != 4) {
                    printf("D should be followed by exactly 3 arguments.\n");
                    break;
                }
                delete_meeting(month, day, hour);
                break;


            case 'L':;
                print_calendar();
                break;


            case 'W':;
                char* wtok = strtok(input, " ");
                if (wtok == NULL) {
                    printf("W should be followed by exactly 1 argument.\n");
                    break;
                }
                int w_argc = 0;
                while (wtok != NULL) {
                    w_argc++;
                    if (w_argc == 1) {
                        command = wtok[0];
                    } else if (w_argc == 2) {
                        strncpy(filename, wtok, MAX_FILENAME_LEN);
                    } else {
                        break;
                    }
                    wtok = strtok(NULL, " ");
                }
                if (w_argc != 2) {
                    printf("W should be followed by exactly 1 argument.\n");
                    break;
                }
                save_to_file(filename);
                break;


            case 'O':;
                char* otok = strtok(input, " ");
                if (otok == NULL) {
                printf("O should be followed by exactly 1 argument.\n");
                break;
                }
                int o_argc = 0;
                while (otok != NULL) {
                    o_argc++;
                    if (o_argc == 1) {
                        command = otok[0];
                    } else if (o_argc == 2) {
                        strncpy(filename, otok, MAX_FILENAME_LEN);
                    } else {
                        break;
                    }
                    otok = strtok(NULL, " ");
                }
                if (o_argc != 2) {
                    printf("O should be followed by exactly 1 argument.\n");
                    break;
                }
                load_from_file(filename);
                break;


            case 'Q':;
                quit_program();
                break;


            default:;
                printf("Invalid command %c\n", command);
        }
    }

    return 0;
}
