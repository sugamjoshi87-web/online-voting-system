/*========================================================
                ONLINE VOTING SYSTEM
==========================================================

Mini Project in C Programming

Project Title:
Online Voting System

Group Members:
1. Sugam Joshi       Roll No:  45
2. Kapil Bohora      Roll No:  23
3. Subodh Pant       Roll No:  43
4. Rashmi Bhatt      Roll No:  38

----------------------------------------------------------
Project Summary:
----------------------------------------------------------

This project is a simple Online Voting System developed
using C programming language. The system allows voters
to register, login securely and cast their votes.
The project also includes an admin panel for managing
the election process.

Main features of the project:

- Voter registration system
- Automatic voter ID generation
- Password encryption for security
- One person can vote only once
- Election date management
- Admin control system
- Automatic vote counting
- Result display system
- File handling for permanent data storage

The project uses concepts like:
structures, functions, file handling,
conditional statements, loops,
authentication and date/time handling.

--------------------------------------------------------
ADMIN VOTING CONTROL
--------------------------------------------------------

In this project, the admin can also manually start
the voting process using the admin panel.

Reason:
Even if the election date arrives, the admin may need
to verify the system, check technical setup or confirm
that all election processes are ready before officially
starting voting.

The system stores voter information, election details,
party details, and voting status in separate files.
Only registered users can vote and each voter can vote
only once.

This project helps demonstrate how programming concepts
can be applied to solve real-world problems digitally.

========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ================= STRUCTURES =================

struct voter {
    int id;
    char firstName[30];
    char lastName[30];
    char password[20];
    int voted;
};

struct party {
    int id;
    char name[50];
    char symbol[20];
    int votes;
};

struct election {
    int day, month, year;
};

// ================= ENCRYPTION =================

void encrypt(char *pass) {
    for (int i = 0; pass[i] != '\0'; i++)
        pass[i] += 3;
}

// ================= AUTO ID =================

int generateVoterID() {
    FILE *fp = fopen("voters.dat", "rb");
    if (fp == NULL) return 1001;

    struct voter v;
    int count = 0;

    while (fread(&v, sizeof(v), 1, fp)) count++;

    fclose(fp);
    return 1001 + count;
}

// ================= VOTING STATUS =================

void startVoting() {
    FILE *fp = fopen("status.dat", "wb");
    int status = 1;
    fwrite(&status, sizeof(int), 1, fp);
    fclose(fp);
    printf("Voting STARTED by admin!\n");
}

int isVotingStarted() {
    FILE *fp = fopen("status.dat", "rb");
    if (fp == NULL) return 0;

    int status;
    fread(&status, sizeof(int), 1, fp);
    fclose(fp);

    return status;
}

// ================= INIT PARTIES =================

void initParties() {
    FILE *fp = fopen("parties.dat", "rb");

    if (fp == NULL) {
        fp = fopen("parties.dat", "wb");

        struct party p[3] = {
            {1, "Party A", "Star", 0},
            {2, "Party B", "Moon", 0},
            {3, "Party C", "Bolt", 0}
        };

        fwrite(p, sizeof(p), 1, fp);
    }

    fclose(fp);
}

// ================= CHECK REGISTRATION =================

int checkRegistration() {
    FILE *fp = fopen("election.dat", "rb");
    if (fp == NULL) return 0;

    struct election e;
    fread(&e, sizeof(e), 1, fp);
    fclose(fp);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    int d = t->tm_mday;
    int m = t->tm_mon + 1;
    int y = t->tm_year + 1900;

    if (y == e.year && m == e.month) {
        if (d >= (e.day - 10) && d < e.day)
            return 1;
    }

    return 0;
}

// ================= REGISTER =================

void registerVoter() {

    if (!checkRegistration()) {
        printf("Registration closed (only 10 days before election)\n");
        return;
    }

    FILE *fp = fopen("voters.dat", "ab");
    struct voter v;

    printf("Enter First Name: ");
    scanf("%s", v.firstName);

    printf("Enter Last Name: ");
    scanf("%s", v.lastName);

    printf("Enter Password: ");
    scanf("%s", v.password);

    v.id = generateVoterID();
    printf("Your Voter ID is: %d\n", v.id);

    encrypt(v.password);
    v.voted = 0;

    fwrite(&v, sizeof(v), 1, fp);
    fclose(fp);

    printf("Registration successful!\n");
}

// ================= SET DATE =================

void setElectionDate() {
    FILE *fp = fopen("election.dat", "wb");
    struct election e;

    printf("Enter Election Date (DD MM YYYY): ");
    scanf("%d %d %d", &e.day, &e.month, &e.year);

    fwrite(&e, sizeof(e), 1, fp);
    fclose(fp);

    printf("Election date set!\n");
}

// ================= CHECK DAY =================

int checkElectionDay() {
    FILE *fp = fopen("election.dat", "rb");
    if (fp == NULL) return 0;

    struct election e;
    fread(&e, sizeof(e), 1, fp);
    fclose(fp);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    return (t->tm_mday == e.day &&
            t->tm_mon + 1 == e.month &&
            t->tm_year + 1900 == e.year);
}

// ================= VOTE =================

void castVote() {

    if (!checkElectionDay() && !isVotingStarted()) {
        printf("Voting not started yet!\n");
        return;
    }

    FILE *fp = fopen("voters.dat", "rb+");
    struct voter v;

    int id;
    char pass[20];

    printf("Enter ID: ");
    scanf("%d", &id);

    printf("Enter Password: ");
    scanf("%s", pass);
    encrypt(pass);

    while (fread(&v, sizeof(v), 1, fp)) {

        if (v.id == id && strcmp(v.password, pass) == 0) {

            if (v.voted == 1) {
                printf("Already voted!\n");
                fclose(fp);
                return;
            }

            printf("\n--- PARTIES ---\n");

            FILE *pf = fopen("parties.dat", "rb");
            struct party p;

            while (fread(&p, sizeof(p), 1, pf)) {
                printf("%d. %s (%s)\n", p.id, p.name, p.symbol);
            }

            fclose(pf);

            int choice;
            printf("Enter choice: ");
            scanf("%d", &choice);

            pf = fopen("parties.dat", "rb+");

            while (fread(&p, sizeof(p), 1, pf)) {
                if (p.id == choice) {
                    p.votes++;
                    fseek(pf, -sizeof(p), SEEK_CUR);
                    fwrite(&p, sizeof(p), 1, pf);
                    break;
                }
            }

            fclose(pf);

            v.voted = 1;
            fseek(fp, -sizeof(v), SEEK_CUR);
            fwrite(&v, sizeof(v), 1, fp);

            printf("Vote successful!\n");
            fclose(fp);
            return;
        }
    }

    printf("Invalid login!\n");
    fclose(fp);
}

// ================= RESULTS =================

void showResults() {
    FILE *fp = fopen("parties.dat", "rb");
    struct party p;

    printf("\n--- RESULTS ---\n");

    while (fread(&p, sizeof(p), 1, fp)) {
        printf("%s (%s): %d votes\n", p.name, p.symbol, p.votes);
    }

    fclose(fp);
}

// ================= ADMIN =================

void adminPanel() {

    char user[20], pass[20];

    printf("Admin Username: ");
    scanf("%s", user);

    printf("Admin Password: ");
    scanf("%s", pass);

    if (strcmp(user, "admin") == 0 && strcmp(pass, "1234") == 0) {

        int ch;
        while (1) {
            printf("\n--- ADMIN PANEL ---\n");
            printf("1. Set Election Date\n");
            printf("2. View Results\n");
            printf("3. Start Voting\n");
            printf("4. Exit\n");

            scanf("%d", &ch);

            if (ch == 1) setElectionDate();
            else if (ch == 2) showResults();
            else if (ch == 3) startVoting();
            else break;
        }

    } else {
        printf("Invalid admin login!\n");
    }
}

// ================= MAIN =================

int main() {

    initParties();

    int ch;

    while (1) {

        printf("\n===== VOTING SYSTEM =====\n");
        printf("1. Register\n");
        printf("2. Vote\n");
        printf("3. Admin Panel\n");
        printf("4. Exit\n");

        printf("Enter choice: ");
        scanf("%d", &ch);

        switch (ch) {
            case 1: registerVoter(); break;
            case 2: castVote(); break;
            case 3: adminPanel(); break;
            case 4: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
}