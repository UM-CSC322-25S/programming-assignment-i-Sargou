#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_BOATS 120
#define MAX_NAME_LEN 128
#define MAX_TYPE_LEN 16
#define MAX_INFO_LEN 16

typedef struct {
    char name[MAX_NAME_LEN];
    int length;
    char type[MAX_TYPE_LEN];
    char info[MAX_INFO_LEN];
    float owed;
} Boat;

Boat* boats[MAX_BOATS];
int boat_count = 0;

float get_rate(char* type) {
    if (strcmp(type, "slip") == 0) return 12.5;
    if (strcmp(type, "land") == 0) return 14.0;
    if (strcmp(type, "trailer") == 0) return 25.0;
    if (strcmp(type, "storage") == 0) return 11.2;
    return 0.0;
}

void to_lower(char* s) {
    while (*s) {
        *s = tolower(*s);
        s++;
    }
}

void load_data(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (boat_count >= MAX_BOATS) break;
        Boat* b = malloc(sizeof(Boat));
        sscanf(line, "%127[^,],%d,%15[^,],%15[^,],%f",
               b->name, &b->length, b->type, b->info, &b->owed);
        to_lower(b->type); // Convert type to lowercase
        boats[boat_count++] = b;
    }
    fclose(file);
}

void save_data(const char* filename) {
    FILE* file = fopen(filename, "w");
    for (int i = 0; i < boat_count; i++) {
        fprintf(file, "%s,%d,%s,%s,%.2f\n",
                boats[i]->name, boats[i]->length, boats[i]->type,
                boats[i]->info, boats[i]->owed);
    }
    fclose(file);
}

int compare_names(const void* a, const void* b) {
    Boat* ba = *(Boat**)a;
    Boat* bb = *(Boat**)b;
    return strcmp(ba->name, bb->name);
}

void display_inventory() {
    qsort(boats, boat_count, sizeof(Boat*), compare_names);
    for (int i = 0; i < boat_count; i++) {
        printf("%-20s %3d'  %-7s %-6s Owes $%8.2f\n",
               boats[i]->name, boats[i]->length,
               boats[i]->type, boats[i]->info, boats[i]->owed);
    }
}

void add_boat() {
    if (boat_count >= MAX_BOATS) return;
    char input[256];
    printf("Please enter the boat data in CSV format                 : ");
    fgets(input, sizeof(input), stdin);
    Boat* b = malloc(sizeof(Boat));
    if (sscanf(input, "%127[^,],%d,%15[^,],%15[^,],%f",
               b->name, &b->length, b->type, b->info, &b->owed) != 5) {
        printf("Invalid boat data format.\n");
        free(b);
        return;
    }
    to_lower(b->type); // Convert type to lowercase
    boats[boat_count++] = b;
}

void remove_boat() {
    char name[MAX_NAME_LEN];
    printf("Please enter the boat name to be removed                 : ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;
    for (int i = 0; i < boat_count; i++) {
        if (strcmp(boats[i]->name, name) == 0) {
            free(boats[i]);
            for (int j = i; j < boat_count - 1; j++) boats[j] = boats[j + 1];
            boat_count--;
            return;
        }
    }
    printf("Boat not found.\n");
}

void accept_payment() {
    char name[MAX_NAME_LEN];
    float amount;
    printf("Please enter the boat name to accept a payment           : ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;
    printf("Enter amount paid: ");
    if (scanf("%f", &amount) != 1) {
        printf("Invalid amount.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');
    for (int i = 0; i < boat_count; i++) {
        if (strcmp(boats[i]->name, name) == 0) {
            if (amount > boats[i]->owed) {
                printf("Payment too large.\n");
                return;
            }
            boats[i]->owed -= amount;
            return;
        }
    }
    printf("Boat not found.\n");
}

void update_monthly_charges() {
    for (int i = 0; i < boat_count; i++) {
        boats[i]->owed += boats[i]->length * get_rate(boats[i]->type);
    }
    printf("Monthly charges applied to all boats.\n");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s BoatData.csv\n", argv[0]);
        return 1;
    }
    load_data(argv[1]);
    char choice;
    int running = 1;
    while (running) {
        printf("\n(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it : ");
        scanf(" %c", &choice);
        while (getchar() != '\n');
        switch (tolower(choice)) {
            case 'i': display_inventory(); break;
            case 'a': add_boat(); break;
            case 'r': remove_boat(); break;
            case 'p': accept_payment(); break;
            case 'm': update_monthly_charges(); break;
            case 'x': running = 0; break;
            default: printf("Invalid option.\n"); break;
        }
    }
    save_data(argv[1]);
    for (int i = 0; i < boat_count; i++) free(boats[i]);
    return 0;
}