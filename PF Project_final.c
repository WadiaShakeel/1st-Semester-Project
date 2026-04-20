#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_SLOTS 20
#define TICKET_OFFSET 17900

/* File names */
const char *PARKING_DATA_FILE = "parking_data.txt";
const char *REVENUE_FILE = "revenue.txt";
const char *CHECKIN_LOG_FILE = "checkin_log.txt";
const char *CHECKOUT_LOG_FILE = "checkout_log.txt";
const char *FEEDBACK_FILE = "feedback.txt";

const char *SUPERVISOR_PASS = "admin123";

struct Vehicle {
    char type[20];
    char number_plate[15];
    int isoccupied;
    int slot_number;
    time_t Checkin_time;
    int fee;
};

struct Vehicle vehicles[MAX_SLOTS];
int total_revenue = 0;

void ensure_files_exist(void);
void load_parking_data(void);
void save_parking_data(void);
void load_revenue(void);
void save_revenue(int fee);

void log_checkin(const struct Vehicle *v);
void log_checkout(const struct Vehicle *v, int hours);

int Check_in();
int Check_out();
void display_slots();
void feedback();

void supervisor_menu(void);
void display_file_contents(const char *filename);
void reset_revenue(void);
void reset_logs(void);
void reset_entire_system(void);

/* Utility */
int stricmp_portable(const char *a, const char *b);
void flush_stdin(void);
int count_total_checkins(void);

/* -------------------- main -------------------- */

int main(void) {
    /* initialize in-memory slots */
    for (int i = 0; i < MAX_SLOTS; ++i) {
        vehicles[i].slot_number = i + 1;
        vehicles[i].isoccupied = 0;
        vehicles[i].type[0] = '\0';
        vehicles[i].number_plate[0] = '\0';
        vehicles[i].Checkin_time = 0;
        vehicles[i].fee = 0;
    }

    /* ensure files exist, load persisted data */
    ensure_files_exist();
    load_parking_data();
    load_revenue();

    int main_choice = 0;
    do {
        printf("\n=========== PARKING LOT MANAGEMENT SYSTEM ===========\n");
        printf("1. User\n");
        printf("2. Supervisor\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &main_choice) != 1) {
			printf("Invalid input.\n");
			flush_stdin();
			main_choice = 0;
			continue;
		}

        switch(main_choice) {
            case 1: {
                int user_choice = 0;
                do {
                    printf("\n------ USER MENU ------\n");
                    printf("1. Vehicle Check-in\n");
                    printf("2. Vehicle Check-out\n");
                    printf("3. View Parking Slots\n");
                    printf("4. Feedback & Rating\n");
                    printf("5. Back\n");
                    printf("Enter choice: ");
                    if (scanf("%d", &user_choice) != 1){
						printf("Invalid input.\n"); flush_stdin(); user_choice = 5; continue;
					}

                    switch (user_choice) {
                        case 1: Check_in(); break;
                        case 2: Check_out(); break;
                        case 3: display_slots(); break;
                        case 4: feedback(); break;
                        case 5: break;
                        default: printf("Invalid choice!\n"); break;
                    }
                } while (user_choice != 5);
                break;
            }

            case 2: {
                char pass[64];
                printf("Enter Supervisor Password: ");
                flush_stdin();
                if (scanf("%63s", pass) != 1) { printf("Invalid input.\n"); break; }
                if (strcmp(pass, SUPERVISOR_PASS) == 0) supervisor_menu();
                else printf("Incorrect password.\n");
                break;
            }

            case 3:
                save_parking_data();
                printf("All data saved. Exiting...\n");
                break;

            default:
                printf("Invalid choice!\n");
                break;
        }
    } while (main_choice != 3);

    return 0;
}

/* -------------------- Function DEFINITIONS (after main) -------------------- */

/* Utility: case-insensitive compare */
int stricmp_portable(const char *a, const char *b) {
    while (*a && *b) {
        char ca = tolower((unsigned char)*a);
        char cb = tolower((unsigned char)*b);
        if (ca != cb) return (unsigned char)ca - (unsigned char)cb;
        ++a; ++b;
    }
    return (unsigned char)tolower((unsigned char)*a) - (unsigned char)tolower((unsigned char)*b);
}

/* Utility: flush stdin until newline */
void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* Ensure files exist so subsequent operations won't fail */
void ensure_files_exist(void) {
    FILE *f;
    f = fopen(PARKING_DATA_FILE, "a"); if (f) fclose(f);
    f = fopen(REVENUE_FILE, "a"); if (f) fclose(f);
    f = fopen(CHECKIN_LOG_FILE, "a"); if (f) fclose(f);
    f = fopen(CHECKOUT_LOG_FILE, "a"); if (f) fclose(f);
    f = fopen(FEEDBACK_FILE, "a"); if (f) fclose(f);
}

/* Load parking data from file (simple space-separated format) */
void load_parking_data(void) {
    FILE *f = fopen(PARKING_DATA_FILE, "r");
    if (!f) return;
    for (int i = 0; i < MAX_SLOTS; ++i) {
        int occ = 0, fee = 0, slotnum = i + 1;
        long t = 0;
        char type_temp[20], plate_temp[20];
        /* format: isoccupied type plate checkin_time fee slot_number */
        if (fscanf(f, "%d %19s %19s %ld %d %d", &occ, type_temp, plate_temp, &t, &fee, &slotnum) == 6) {
            vehicles[i].isoccupied = occ ? 1 : 0;
            vehicles[i].slot_number = slotnum;
            vehicles[i].fee = fee;
            vehicles[i].Checkin_time = (time_t)t;
            if (vehicles[i].isoccupied) {
                strncpy(vehicles[i].type, type_temp, sizeof(vehicles[i].type)-1);
                vehicles[i].type[sizeof(vehicles[i].type)-1] = '\0';
                strncpy(vehicles[i].number_plate, plate_temp, sizeof(vehicles[i].number_plate)-1);
                vehicles[i].number_plate[sizeof(vehicles[i].number_plate)-1] = '\0';
            } else {
                vehicles[i].type[0] = '\0';
                vehicles[i].number_plate[0] = '\0';
                vehicles[i].Checkin_time = 0;
            }
        } else {
            /* If file shorter / malformed, ensure rest are blanked */
            vehicles[i].isoccupied = 0;
            vehicles[i].slot_number = i + 1;
            vehicles[i].type[0] = '\0';
            vehicles[i].number_plate[0] = '\0';
            vehicles[i].Checkin_time = 0;
            vehicles[i].fee = 0;
        }
    }
    fclose(f);
}

/* Save parking data to file */
void save_parking_data(void) {
    FILE *f = fopen(PARKING_DATA_FILE, "w");
    if (!f) { printf("Error: could not save parking data.\n"); return; }
    for (int i = 0; i < MAX_SLOTS; ++i) {
        fprintf(f, "%d %s %s %ld %d %d\n",
                vehicles[i].isoccupied,
                vehicles[i].isoccupied ? vehicles[i].type : "-",
                vehicles[i].isoccupied ? vehicles[i].number_plate : "-",
                (long)vehicles[i].Checkin_time,
                vehicles[i].fee,
                vehicles[i].slot_number);
    }
    fclose(f);
}

/* Load revenue */
void load_revenue(void) {
    FILE *f = fopen(REVENUE_FILE, "r");
    if (!f) { total_revenue = 0; return; }
    if (fscanf(f, "%d", &total_revenue) != 1) total_revenue = 0;
    fclose(f);
}

/* Save (add) revenue */
void save_revenue(int fee) {
    total_revenue += fee;
    FILE *f = fopen(REVENUE_FILE, "w");
    if (!f) { printf("Error: could not save revenue.\n"); return; }
    fprintf(f, "%d\n", total_revenue);
    fclose(f);
}

/* Log check-in event */
void log_checkin(const struct Vehicle *v) {
    FILE *f = fopen(CHECKIN_LOG_FILE, "a");
    if (!f) return;
    time_t t = v->Checkin_time;
    char *ts = ctime(&t);
    if (ts) {
        /* ctime returns newline-terminated string; keep it for readability */
        fprintf(f, "CHECK-IN | Slot:%d | Type:%s | Plate:%s | Time:%s",
                v->slot_number, v->type, v->number_plate, ts);
    } else {
        fprintf(f, "CHECK-IN | Slot:%d | Type:%s | Plate:%s | Time:unknown\n",
                v->slot_number, v->type, v->number_plate);
    }
    fclose(f);
}

/* Log check-out event */
void log_checkout(const struct Vehicle *v, int hours) {
    FILE *f = fopen(CHECKOUT_LOG_FILE, "a");
    if (!f) return;
    time_t now = time(NULL);
    char *ts = ctime(&now);
    if (ts) {
        fprintf(f, "CHECK-OUT | Slot:%d | Type:%s | Plate:%s | Hours:%d | Fee:%d | Time:%s | TotalRevenue:%d\n",
                v->slot_number, v->type, v->number_plate, hours, v->fee, ts, total_revenue);
    } else {
        fprintf(f, "CHECK-OUT | Slot:%d | Type:%s | Plate:%s | Hours:%d | Fee:%d | Time:unknown | TotalRevenue:%d\n",
                v->slot_number, v->type, v->number_plate, hours, v->fee, total_revenue);
    }
    fclose(f);
}

/* User: Vehicle check-in */
int Check_in(void) {
    int slot = -1;
    for (int i = 0; i < MAX_SLOTS; ++i) if (!vehicles[i].isoccupied) { slot = i; break; }
    if (slot == -1) { printf("Parking is FULL!\n"); return 0; }

    printf("Enter Vehicle Type (Car/Bike/Van): ");
    flush_stdin();
    if (scanf("%19s", vehicles[slot].type) != 1) { printf("Invalid input.\n"); return 0; }

    if (stricmp_portable(vehicles[slot].type, "Car") != 0 &&
        stricmp_portable(vehicles[slot].type, "Bike") != 0 &&
        stricmp_portable(vehicles[slot].type, "Van") != 0) {
        printf("Invalid vehicle type! Allowed: Car, Bike, Van\n");
        vehicles[slot].type[0] = '\0';
        return 0;
    }

    printf("Enter Number Plate: ");
    if (scanf("%14s", vehicles[slot].number_plate) != 1) { printf("Invalid input.\n"); vehicles[slot].type[0] = '\0'; return 0; }

    vehicles[slot].isoccupied = 1;
    vehicles[slot].Checkin_time = time(NULL);
    vehicles[slot].fee = 0;

    log_checkin(&vehicles[slot]);
    save_parking_data();

    printf("\nVehicle Checked-in Successfully!\n");
    printf("Slot Allotted: %d\n", vehicles[slot].slot_number);
    printf("Ticket Number: %d\n", vehicles[slot].slot_number + TICKET_OFFSET);
    printf("Check-in Time: %s", ctime(&vehicles[slot].Checkin_time));
    return 1;
}

/* User: Vehicle check-out */
int Check_out(void) {
    int ticket;
    printf("Enter Ticket Number: ");
    if (scanf("%d", &ticket) != 1) { printf("Invalid input.\n"); flush_stdin(); return 0; }

    for (int i = 0; i < MAX_SLOTS; ++i) {
        if (vehicles[i].isoccupied && (vehicles[i].slot_number + TICKET_OFFSET) == ticket) {
            time_t outt = time(NULL);
            double hours_d = difftime(outt, vehicles[i].Checkin_time) / 3600.0;
            int hours = (int)hours_d;
            if (hours_d - hours > 0.0) hours++;

            int fee = 0;
            if (stricmp_portable(vehicles[i].type, "Car") == 0) fee = 100 * hours;
            else if (stricmp_portable(vehicles[i].type, "Bike") == 0) fee = 50 * hours;
            else if (stricmp_portable(vehicles[i].type, "Van") == 0) fee = 150 * hours;
            else fee = 200 * hours;

            /* Make a copy to log/print before clearing the slot */
            struct Vehicle copy = vehicles[i];
            copy.fee = fee;

            /* Update revenue + logs */
            vehicles[i].fee = fee;
            save_revenue(fee);
            log_checkout(&copy, hours);

            /* Print receipt BEFORE clearing */
            printf("\n----- RECEIPT -----\n");
            printf("Vehicle: %s\n", copy.type);
            printf("Plate: %s\n", copy.number_plate);
            printf("Hours: %d\n", hours);
            printf("Fee: Rs.%d\n", fee);
            printf("-------------------\n");

            /* Free the slot */
            vehicles[i].isoccupied = 0;
            vehicles[i].type[0] = '\0';
            vehicles[i].number_plate[0] = '\0';
            vehicles[i].Checkin_time = 0;
            vehicles[i].fee = 0;
            save_parking_data();

            return 1;
        }
    }

    printf("Invalid Ticket Number or vehicle not parked!\n");
    return 0;
}

/* Display currently parked vehicles */
void display_slots(void) {
    printf("\n------ CURRENT PARKING ------\n");
    int occupied = 0;
    for (int i = 0; i < MAX_SLOTS; ++i) {
        if (vehicles[i].isoccupied) {
            occupied++;
        }
    }
    printf("\nTotal Occupied: %d / %d\n", occupied, MAX_SLOTS);   
    if (occupied == 0) printf("All slots are available.\n");
}

/* Feedback & rating */
void feedback(void) {
    int rating;
    char comment[256];
    printf("Rate (1-5): ");
    if (scanf("%d", &rating) != 1 || rating < 1 || rating > 5) {
		printf("Invalid rating.\n");
		flush_stdin(); return;
	}
    printf("Enter Comment: ");
    flush_stdin();
    if (!fgets(comment, sizeof(comment), stdin)) comment[0] = '\0';
    else {
        size_t len = strlen(comment);
        if (len > 0 && comment[len-1] == '\n') comment[len-1] = '\0';
    }

    FILE *f = fopen(FEEDBACK_FILE, "a");
    if (!f) { printf("Error saving feedback.\n"); return; }
    time_t now = time(NULL);
    char *ts = ctime(&now);
    if (ts && ts[strlen(ts)-1] == '\n') ts[strlen(ts)-1] = '\0';
    fprintf(f, "Time: %s | Rating: %d | Comment: %s\n", ts ? ts : "unknown", rating, comment);
    fclose(f);
    printf("Thank you for your feedback!\n");
}

/* Display contents of a text file to console */
void display_file_contents(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) { printf("Could not open %s\n", filename); return; }
    char line[512];
    printf("\n--- Contents of %s ---\n", filename);
    while (fgets(line, sizeof(line), f)) printf("%s", line);
    printf("\n----------------------------\n");
    fclose(f);
}

/* Count total check-ins (lines starting with CHECK-IN in log) */
int count_total_checkins(void) {
    FILE *f = fopen(CHECKIN_LOG_FILE, "r");
    if (!f) return 0;
    char line[512];
    int cnt = 0;
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "CHECK-IN", 8) == 0) ++cnt;
    }
    fclose(f);
    return cnt;
}

/* Reset revenue to zero (file + in-memory) */
void reset_revenue(void) {
    printf("Are you sure you want to reset total revenue? (1 = Yes, 2 = No): ");
    int conf = 0;
    if (scanf("%d", &conf) != 1) { printf("Invalid input.\n"); flush_stdin(); return; }
    if (conf == 1) {
        total_revenue = 0;
        FILE *f = fopen(REVENUE_FILE, "w");
        if (f) { fprintf(f, "0\n"); fclose(f); }
        printf("Total revenue reset to 0.\n");
    } else {
        printf("Cancelled.\n");
    }
}

/* Reset logs: empties checkin, checkout, feedback logs (keeps files) */
void reset_logs(void) {
    printf("Are you sure you want to delete all logs? (1 = Yes, 2 = No): ");
    int conf = 0;
    if (scanf("%d", &conf) != 1) {
		printf("Invalid input.\n");
		flush_stdin(); return;
	}
    if (conf == 1) {
        FILE *f;
        f = fopen(CHECKIN_LOG_FILE, "w"); if (f) fclose(f);
        f = fopen(CHECKOUT_LOG_FILE, "w"); if (f) fclose(f);
        f = fopen(FEEDBACK_FILE, "w"); if (f) fclose(f);
        printf("All logs cleared.\n");
    } else {
        printf("Cancelled.\n");
    }
}

/* Reset entire system: clears revenue, logs, and parking slots */
void reset_entire_system(void) {
    printf("RESET ENTIRE SYSTEM will clear revenue, logs, and all parked slots.\n");
    printf("Are you sure? (1 = Yes, 2 = No): ");
    int conf = 0;
    if (scanf("%d", &conf) != 1) { printf("Invalid input.\n"); flush_stdin(); return; }
    if (conf == 1) {
        /* Reset revenue */
        total_revenue = 0;
        FILE *f = fopen(REVENUE_FILE, "w"); if (f) { fprintf(f, "0\n"); fclose(f); }

        /* Clear logs */
        f = fopen(CHECKIN_LOG_FILE, "w"); if (f) fclose(f);
        f = fopen(CHECKOUT_LOG_FILE, "w"); if (f) fclose(f);
        f = fopen(FEEDBACK_FILE, "w"); if (f) fclose(f);

        /* Clear parking slots in memory and save parking data */
        for (int i = 0; i < MAX_SLOTS; ++i) {
            vehicles[i].isoccupied = 0;
            vehicles[i].type[0] = '\0';
            vehicles[i].number_plate[0] = '\0';
            vehicles[i].Checkin_time = 0;
            vehicles[i].fee = 0;
        }
        save_parking_data();

        printf("System reset completed.\n");
    } else {
        printf("Cancelled.\n");
    }
}

/* Supervisor menu */
void supervisor_menu(void) {
    int choice = 0;
    do {
        printf("\n------ SUPERVISOR MENU ------\n");
        printf("1. View Total Revenue\n");
        printf("2. View Parked Vehicles\n");
        printf("3. Open Check-in Log\n");
        printf("4. Open Check-out Log\n");
        printf("5. Open Feedback Log\n");
        printf("6. Reset Revenue\n");
        printf("7. Reset Logs (Check-in, Check-out, Feedback)\n");
        printf("8. Reset Entire System\n");
        printf("9. Back\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
			printf("Invalid input.\n");
			flush_stdin();
			choice = 9; continue; 
		}

        switch (choice) {
            case 1:
                load_revenue(); /* reload from file */
                printf("Total Revenue: Rs.%d\n", total_revenue);
                printf("Total vehicles processed (check-ins): %d\n", count_total_checkins());
                break;
            case 2:
                display_slots();
                break;
            case 3:
                /* Ensure file exists then open with Notepad (Windows) */
                { FILE *f = fopen(CHECKIN_LOG_FILE, "a"); if (f) fclose(f); }
                printf("Opening Check-in Log ...\n");
                system("start notepad.exe checkin_log.txt");
                break;
            case 4:
                { FILE *f = fopen(CHECKOUT_LOG_FILE, "a"); if (f) fclose(f); }
                printf("Opening Check-out Log...\n");
                system("start notepad.exe checkout_log.txt");
                break;
            case 5:
                { FILE *f = fopen(FEEDBACK_FILE, "a"); if (f) fclose(f); }
                printf("Opening Feedback Log ...\n");
                system("start notepad.exe feedback.txt");
                break;
            case 6:
                reset_revenue();
                break;
            case 7:
                reset_logs();
                break;
            case 8:
                reset_entire_system();
                break;
            case 9:
                break;
            default:
                printf("Invalid choice!\n");
                break;
        }
    } while (choice != 9);
}
