#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct Student {
    int id;
    char name[50];
};


void getStudentName(int id, char *name) {
    FILE *fp = fopen("students.txt", "r");
    if (fp == NULL) {
        strcpy(name, "Unknown");
        return;
    }
    
    int studentId;
    char studentName[50];
    while (fscanf(fp, "%d,%s", &studentId, studentName) != EOF) {
        if (studentId == id) {
            strcpy(name, studentName);
            fclose(fp);
            return;
        }
    }
    fclose(fp);
    strcpy(name, "Unknown");
}


void generateAdmitCard(int id, float percentage, int present, int total, int isEligible) {
    char filename[100];
    char studentName[50];
    time_t now;
    struct tm *timeinfo;
    
    
    time(&now);
    timeinfo = localtime(&now);
    
    
    getStudentName(id, studentName);
    
    
    sprintf(filename, "admit_card_%d.txt", id);
    
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error creating admit card file!\n");
        return;
    }
    
    fprintf(fp, "===============================================\n");
    fprintf(fp, "                 ADMIT CARD                   \n");
    fprintf(fp, "===============================================\n\n");
    
    fprintf(fp, "Student Details:\n");
    fprintf(fp, "----------------\n");
    fprintf(fp, "Student ID       : %d\n", id);
    fprintf(fp, "Student Name     : %s\n", studentName);
    fprintf(fp, "Generated On     : %02d-%02d-%04d at %02d:%02d\n", 
            timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900,
            timeinfo->tm_hour, timeinfo->tm_min);
    fprintf(fp, "\nAttendance Summary:\n");
    fprintf(fp, "------------------\n");
    fprintf(fp, "Total Days       : %d\n", total);
    fprintf(fp, "Present Days     : %d\n", present);
    fprintf(fp, "Absent Days      : %d\n", total - present);
    fprintf(fp, "Attendance %%     : %.2f%%\n", percentage);
    fprintf(fp, "\nEligibility Status:\n");
    fprintf(fp, "-------------------\n");
    
    if (isEligible) {
        fprintf(fp, "STATUS: ELIGIBLE FOR EXAMINATION\n");
        fprintf(fp, "✓ Minimum 75%% attendance requirement met\n\n");
    } else {
        fprintf(fp, "STATUS: NOT ELIGIBLE FOR EXAMINATION\n");
        fprintf(fp, "✗ Minimum 75%% attendance requirement NOT met\n");
        fprintf(fp, "Current attendance: %.2f%% (Required: 75%%)\n\n", percentage);
    }
    
    fprintf(fp, "\n===============================================\n");
    fprintf(fp, "This is a computer generated document.\n");
    fprintf(fp, "No signature required.\n");
    fprintf(fp, "===============================================\n");
    
    fclose(fp);
    
    
    printf("Admit card generated successfully: %s\n", filename);
    printf("Please open the file manually to view your admit card.\n");
}


int isStudentRegistered(int id) {
    FILE *fp = fopen("students.txt", "r");
    if (fp == NULL) return 0;
    
    int studentId;
    char name[50];
    while (fscanf(fp, "%d,%s", &studentId, name) != EOF) {
        if (studentId == id) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void addStudent() {
    FILE *fp = fopen("students.txt", "a");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }
    struct Student s;
    printf("Enter student ID: ");
    scanf("%d", &s.id);
    
    if (isStudentRegistered(s.id)) {
        printf("Student with ID %d already exists!\n", s.id);
        fclose(fp);
        return;
    }
    
    printf("Enter student name: ");
    scanf(" %[^\n]", s.name);
    fprintf(fp, "%d,%s\n", s.id, s.name);
    fclose(fp);
    printf("Student added successfully.\n");
}

void markAttendance() {
    int id;
    char status[10];
    int found = 0;
    printf("Enter student ID: ");
    scanf("%d", &id);
    
    if (!isStudentRegistered(id)) {
        printf("Student with ID %d is not registered!\n", id);
        return;
    }
    
    printf("Enter status (P/A): ");
    scanf(" %s", status);
    
    if (strcmp(status, "P") != 0 && strcmp(status, "A") != 0) {
        printf("Invalid status! Use 'P' for Present or 'A' for Absent.\n");
        return;
    }

    FILE *fp = fopen("attendance.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    if (fp == NULL) {
        fp = fopen("attendance.txt", "w");
        fprintf(fp, "%d,%s\n", id, status);
        fclose(fp);
        printf("Attendance marked.\n");
        return;
    }

    if (temp == NULL) {
        printf("Error creating temporary file!\n");
        fclose(fp);
        return;
    }

    int currentId;
    char currentStatus[100];

    while (fscanf(fp, "%d,%s", &currentId, currentStatus) != EOF) {
        if (currentId == id) {
            fprintf(temp, "%d,%s%s\n", currentId, currentStatus, status);
            found = 1;
        } else {
            fprintf(temp, "%d,%s\n", currentId, currentStatus);
        }
    }
    
    if (!found) {
        fprintf(temp, "%d,%s\n", id, status);
    }

    fclose(fp);
    fclose(temp);

    remove("attendance.txt");
    rename("temp.txt", "attendance.txt");

    printf("Attendance marked.\n");
}

void viewAttendance() {
    FILE *fp = fopen("attendance.txt", "r");
    if (fp == NULL) {
        printf("No attendance records found!\n");
        return;
    }
    
    int id;
    char status[100];
    printf("\n--- Attendance Records ---\n");
    printf("%-10s%-15s%-15s%-15s\n", "ID", "Total Days", "Present", "Absent");
    printf("------------------------------------------------\n");
    
    while (fscanf(fp, "%d,%s", &id, status) != EOF) {
        int present = 0, absent = 0;
        for(int i = 0; status[i] != '\0'; i++) {
            if(status[i] == 'P') present++;
            if(status[i] == 'A') absent++;
        }
        printf("%-10d%-15d%-15d%-15d\n", id, present+absent, present, absent);
    }
    fclose(fp);
}

void checkEligibility() {
    FILE *fp = fopen("attendance.txt", "r");
    if (fp == NULL) {
        printf("No attendance records found!\n");
        return;
    }

    int id, currentid;
    char status[100];
    printf("Enter student ID: ");
    scanf("%d", &id);

    int found = 0;
    while (fscanf(fp, "%d,%s", &currentid, status) != EOF) {
        if (currentid == id) {
            found = 1;
            int present = 0, total = 0;
            
            for(int i = 0; status[i] != '\0'; i++) {
                if(status[i] == 'P') present++;
                if(status[i] == 'P' || status[i] == 'A') total++;
            }

            if (total == 0) {
                printf("No attendance records for this student!\n");
            } else {
                float percentage = ((float)present/total) * 100;
                printf("Student ID: %d\n", id);
                printf("Total Days: %d\n", total);
                printf("Present Days: %d\n", present);
                printf("Absent Days: %d\n", total - present);
                printf("Attendance Percentage: %.2f%%\n", percentage);
                
                if(percentage >= 75.0) {
                    printf("✅ Eligible for exam - Generating Admit Card...\n");
                    generateAdmitCard(id, percentage, present, total, 1);
                } else {
                    printf("❌ Not eligible for exam - Generating Admit Card with status...\n");
                    generateAdmitCard(id, percentage, present, total, 0);
                }
            }
            break;
        }
    }

    if(!found) {
        printf("Student ID not found in attendance records!\n");
    }
    
    fclose(fp);
}

int main() {
    int choice;
    printf("=== Student Attendance Management System ===\n");
    
    do {
        printf("\n--- Main Menu ---\n");
        printf("1. Add Student\n");
        printf("2. Mark Attendance\n");
        printf("3. View Attendance\n");
        printf("4. Check Eligibility & Generate Admit Card\n");
        printf("5. Exit\n");
        printf("Enter your choice (1-5): ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: addStudent(); break;
            case 2: markAttendance(); break;
            case 3: viewAttendance(); break;
            case 4: checkEligibility(); break;
            case 5: printf("Thank you for using the Attendance System. Goodbye!\n"); break;
            default: printf("❌ Invalid choice! Please enter 1-5.\n");
        }
    } while (choice != 5);

    return 0;
}