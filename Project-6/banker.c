#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
#define INIT_FILE "resources.txt"

/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];
/* the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int is_leq(int *a, int *b, int n) {
    for(int i = 0 ; i != n; ++i) {
        if(a[i] > b[i]) {
            return 0;
        }
    }
    return 1;
}

int is_safe() {
    int work[NUMBER_OF_RESOURCES], finish[NUMBER_OF_CUSTOMERS];
    memcpy(work, available, NUMBER_OF_RESOURCES * sizeof(int));
    memset(finish, 0, NUMBER_OF_CUSTOMERS * sizeof(int));
    for(int round = 0; round != NUMBER_OF_CUSTOMERS; ++round) {
        int flag = 0;
        for(int i = 0; i != NUMBER_OF_CUSTOMERS; ++i) {
            if(finish[i] == 0 && is_leq(need[i], work, NUMBER_OF_RESOURCES)) {
                flag = 1;
                finish[i] = 1;
                for(int j = 0; j != NUMBER_OF_RESOURCES; ++j) {
                    work[j] += allocation[i][j];
                }
                break;
            }
        }
        if(!flag) {
            return 0;
        }
    }
    return 1;
}

// Banker's Algorithm: request resources
// customer: the target customer
// release: number of resources to request
// return: 0 for success, non-zero for errors
int request_resources(int customer, int request[NUMBER_OF_RESOURCES]) {
    if(customer < 0 || customer >= NUMBER_OF_CUSTOMERS) {
        printf("Invalid customer: %d\n", customer);
        return -1;
    }
    int err = 0;
    for(int i = 0; i != NUMBER_OF_RESOURCES; ++i) {
        if(request[i] < 0 || request[i] > need[customer][i]) {
            printf(
                "Invalid number of resources to request: <customer: %d, resource: %d, need: %d, to request: %d>\n",
                customer, i, need[customer][i], request[i]
            );
            err = -1;
        }
        if(request[i] > available[i]) {
            printf(
                "No enough resources to allocate: <customer: %d, resource: %d, available: %d, to request: %d>\n",
                customer, i, available[i], request[i]
            );
            err = -2;
        }
        if(err != 0) {  // rollback
            while(i--) {
                available[i] += request[i];
                allocation[customer][i] -= request[i];
                need[customer][i] += request[i];
            }
            return err;
        }
        // allocate resources
        available[i] -= request[i];
        allocation[customer][i] += request[i];
        need[customer][i] -= request[i];
    }
    if(!is_safe()) {
        // rollback
        printf("Unsafe state after request!\n");
        for(int i = 0; i != NUMBER_OF_RESOURCES; ++i) {
            available[i] += request[i];
            allocation[customer][i] -= request[i];
            need[customer][i] += request[i];
        }
        return -3;
    }
    return 0;
}


// Banker's Algorithm: release resources
// customer: the target customer
// release: number of resources to release
// return: 0 for success, non-zero for errors
int release_resources(int customer, int release[NUMBER_OF_RESOURCES]) {
    if(customer < 0 || customer >= NUMBER_OF_CUSTOMERS) {
        printf("Invalid customer: %d\n", customer);
        return -1;
    }
    for(int i = 0; i != NUMBER_OF_RESOURCES; ++i) {
        if(release[i] < 0 || release[i] > allocation[customer][i]) {
            printf(
                "Invalid number of resources to release: <customer: %d, resource: %d, allocated: %d, to release: %d>\n",
                customer, i, allocation[customer][i], release[i]
            );
            // rollback
            while(i--) {
                allocation[customer][i - 1] += release[i - 1];
                available[i] -= release[i];
            }
            return -1;
        }
        // release resources
        allocation[customer][i] -= release[i];
        available[i] += release[i];
    }
    return 0;
}

void request_wrapper() {
    int request[NUMBER_OF_RESOURCES], customer;
    scanf("%d", &customer);
    for(int i = 0; i != NUMBER_OF_RESOURCES; ++i) {
        scanf("%d", &request[i]);
    }
    if(request_resources(customer, request) != 0) {
        printf("DENIED.\n");
    } else {
        printf("SUCCESS.\n");
    }
}

void release_wrapper() {
    int release[NUMBER_OF_RESOURCES], customer;
    scanf("%d", &customer);
    for(int i = 0; i != NUMBER_OF_RESOURCES; ++i) {
        scanf("%d", &release[i]);
    }
    if(release_resources(customer, release) != 0) {
        printf("DENIED.\n");
    } else {
        printf("SUCCESS.\n");
    }
}

void display_usage() {
    printf("=============================================================\n");
    printf("<this program> <available resources of size %d>\n", NUMBER_OF_RESOURCES);
    printf("Operations:\n");
    printf("    Request resources: RQ <customer> <resources>\n");
    printf("    Release resources: RL <customer> <resources>\n");
    printf("    Display resources: *\n");
    printf("=============================================================\n");
}

void display_resources() {
    printf("=============================================================\n");
    printf("Availbale resources:\n");
    for(int i = 0; i != NUMBER_OF_RESOURCES; ++i) {
        printf("%d ", available[i]);
    }
    printf("\n");
    printf("-------------------------------------------------------------\n");
    printf("Maximum resources for each customer:\n");
    for(int customer = 0; customer != NUMBER_OF_CUSTOMERS; ++customer) {
        printf("%d: ", customer);
        for(int r = 0; r != NUMBER_OF_RESOURCES; ++r) {
            printf("%d ", maximum[customer][r]);
        }
        printf("\n");
    }
    printf("-------------------------------------------------------------\n");
    printf("Allocated resources for each customer:\n");
    for(int customer = 0; customer != NUMBER_OF_CUSTOMERS; ++customer) {
        printf("%d: ", customer);
        for(int r = 0; r != NUMBER_OF_RESOURCES; ++r) {
            printf("%d ", allocation[customer][r]);
        }
        printf("\n");
    }
    printf("-------------------------------------------------------------\n");
    printf("Needed resources for each customer:\n");
    for(int customer = 0; customer != NUMBER_OF_CUSTOMERS; ++customer) {
        printf("%d: ", customer);
        for(int r = 0; r != NUMBER_OF_RESOURCES; ++r) {
            printf("%d ", need[customer][r]);
        }
        printf("\n");
    }
    printf("=============================================================\n");
}

int init(int argc, char *argv[], const char *resources_file) {
    if(argc != 1 + NUMBER_OF_RESOURCES) {
        printf("Incorrect number of parameters.\n");
        return -1;
    }
    for(int i = 0; i != NUMBER_OF_RESOURCES; ++i) {
        available[i] = atoi(argv[i + 1]);
    }
    FILE *f = fopen(resources_file, "r");
    if(f == NULL) {
        printf("Unable to open file: %s\n", resources_file);
        return -2;
    }
    for(int c = 0; c != NUMBER_OF_CUSTOMERS; ++c) {
        for(int r = 0; r != NUMBER_OF_RESOURCES; ++r) {
            fscanf(f, "%d", &maximum[c][r]);
            need[c][r] = maximum[c][r];
        }
    }
    fclose(f);
    printf("Initialized, ready to run.\n");
    return 0;
}

int main(int argc, char *argv[]) {
    if(init(argc, argv, INIT_FILE) != 0) {
        display_usage();
        return 0;
    }
    char op[5];
    printf("> ");
    while(scanf("%s", op) == 1) {
        if(strcmp(op, "RQ") == 0) {
            request_wrapper();
        } else if(strcmp(op, "RL") == 0) {
            release_wrapper();
        } else if(strcmp(op, "*") == 0) {
            display_resources();
        } else {
            display_usage();
        }
        printf("> ");
    }
    return 0;
}
