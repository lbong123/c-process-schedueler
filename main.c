#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "scheduler.h"

#define F_INPUT "-f"
#define S_INPUT "-s"
#define M_INPUT "-m"
#define Q_INPUT "-q"

#define MEMORY_INPUT_SIZE 9
#define TYPE_INPUT_SIZE 3
#define SCHEDULER_INPUT_SIZE 4



int main(int argc, char *argv[]) {
    char* filename;
    char scheduling_mode[SCHEDULER_INPUT_SIZE];
    char memory_mode[MEMORY_INPUT_SIZE];
    int quantum = 0;

    char type[TYPE_INPUT_SIZE] = "NA";

    // filter through the input for key information
    for (int i = 1; i < argc; i++) {
        if (strcmp(type, "NA") == 0) {
            if (strcmp(argv[i], F_INPUT) == 0) {
                strcpy(type, F_INPUT);
            } else if (strcmp(argv[i], S_INPUT) == 0) {
                strcpy(type, S_INPUT);
            } else if (strcmp(argv[i], M_INPUT) == 0) {
                strcpy(type, M_INPUT);
            } else if (strcmp(argv[i], Q_INPUT) == 0) {
                strcpy(type, Q_INPUT);
            }
        } else {
            // if previous type was -f
            if (strcmp(type, F_INPUT) == 0) {
                filename = (char*) malloc(strlen(argv[i]) + 1);   
                strcpy(filename, argv[i]);
                assert(filename);
            } 
            // if previous type was -s
            if (strcmp(type, S_INPUT) == 0) {
                strcpy(scheduling_mode, argv[i]);
            } 
            // if previous type was -m
            else if (strcmp(type, M_INPUT) == 0) {
                strcpy(memory_mode, argv[i]);
            }
            // if previous type was -q
            else if (strcmp(type, Q_INPUT) == 0) {
                quantum = atoi(argv[i]);
            } 

            // set type to NA
            strcpy(type, "NA");
        }
    }

    schedule(filename, scheduling_mode, memory_mode, quantum);

    free(filename);
    filename = NULL;

}