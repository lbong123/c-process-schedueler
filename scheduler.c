#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "scheduler.h"

#define SJF "SJF"
#define INFINITE "infinite"
#define MEMORY_SIZE 2048

int calculate_procs_remaining(process_node_t* ready_queue, process_node_t* input_queue) {
    int count = 0;
    while (!is_empty(&ready_queue)) {
        count++;
        ready_queue = ready_queue->next;
    }
    while (!is_empty(&input_queue)) {
        count++;
        input_queue = input_queue->next;
    }

    return count;
}

process_node_t* read_processes(char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    int arrived, service, memory;
    char name[MAX_FILENAME_SIZE];

    process_node_t* queue = NULL;

    // reads the processes
    while (fscanf(fp, "%d %s %d %d", &arrived, name, &service, &memory) == 4) {
        //printf("process read\n");
        process_t* process = (process_t*) malloc(sizeof(process_t));
        process->time_arrived = arrived;
        strcpy(process->process_name, name);
        process->service_time = service;
        process->original_service_time = service;
        process->memory_requirement = memory;
        process->bit_map_index = -1;

        //printf("process properly made\n");
        if (queue == NULL) {
            queue = new_node(process, 0);
            //printf("queue successfully made\n");
        } else {
            push(&queue, process);
            //printf("queue pushed to queue\n");
        }
    }

    return queue;
}

int update_process_completion(process_t* process, int quantum) {
    process->service_time -= quantum;
    if (process->service_time <= 0) {
        process->service_time = 0;
        return 1;
    }

    return 0;
}

void initialise_memory(int* memory) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }
}

int best_fit(int* memory, process_t* process) {
    int smallest_start = 0;
    int smallest_end = MEMORY_SIZE + 1;
    int start = 0;
    int end = 0;

    // used to track hole completions
    int valid_hole_found = 0;
    int new_hole_started = 0;

    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i] == 0 && !new_hole_started) {
            new_hole_started = 1;
            start = i;
            continue;
        } 

        if (memory[i] == 1 && new_hole_started) {
            end = i;

            if ((end - start) < (smallest_end - smallest_start) && (end - start) >= process->memory_requirement) {
                smallest_start = start;
                smallest_end = end;
                valid_hole_found = 1;
            }

            new_hole_started = 0;
        }
    }

    // check for the edge case at the end, where we have a hole at the end of the memory that hasn't terminated
    if (new_hole_started) {
        end = MEMORY_SIZE;
        if ((end - start) < (smallest_end - smallest_start) && (end - start) >= process->memory_requirement) {
            smallest_start = start;
            smallest_end = end;
            valid_hole_found = 1;
        }
    }

    // return based on if a hole was found
    if (valid_hole_found) {
        return smallest_start;
    }
    return -1;
}

void fill_memory(int* memory, process_t* process) {
    int start = process->bit_map_index;
    int end = process->bit_map_index + process->memory_requirement;

    for (int i = start; i < end; i++) {
        memory[i] = 1;
    }
}

void free_memory(int* memory, process_t* process) {
    int start = process->bit_map_index;
    int end = process->bit_map_index + process->memory_requirement;

    for (int i = start; i < end; i++) {
        memory[i] = 0;
    }
}

int calculate_turnaround_time(process_t* completed_process, int simulation_time) {
    return simulation_time - completed_process->time_arrived;
}

void update_overhead_values(float* max_overhead, float* total_overhead, process_t* completed_process, int proc_turnaround_time) {
    float current_overhead = proc_turnaround_time / (float) completed_process->original_service_time;

    *total_overhead += current_overhead;

    if (*max_overhead <= current_overhead) {
        *max_overhead = current_overhead;
    }
}

void output_statistics(int simulation_time, float max_overhead, float total_overhead, 
                                                    int total_turnaround_time, int total_procs_completed) {
    float average_turnaround_time = total_turnaround_time / (float) total_procs_completed;
    float average_overhead = total_overhead / (float) total_procs_completed;

    printf("Turnaround time %d\n", (int) ceil(average_turnaround_time));
    printf("Time overhead %.2f %.2f\n", max_overhead, average_overhead);
    printf("Makespan %d\n", simulation_time);
}

void schedule_SJF(process_node_t* disk_inputs, char* memory_mode, int quantum) {
    int simulation_time = -1 * quantum;
    process_node_t* ready_queue = NULL;
    process_node_t* input_queue = NULL;
    process_t* current_process = NULL;
    int transition_ready = 1;

    int total_procs_completed = 0;
    int total_turnaround_time = 0;
    float max_overhead = 0;
    float total_overhead = 0;

    int memory[MEMORY_SIZE];
    initialise_memory(memory);

    // keep going until the current process is done, and ready, input and the disk are empty
    while(current_process != NULL || !is_empty(&ready_queue) || !is_empty(&input_queue) || !is_empty(&disk_inputs)) {
        simulation_time += quantum;

        // checks if the process should be terminated and freed
        if (current_process != NULL && update_process_completion(current_process, quantum) == 1) {
            int turnaround_time = calculate_turnaround_time(current_process, simulation_time);

            transition_ready = 1;
            total_procs_completed++;
            total_turnaround_time += turnaround_time;
            update_overhead_values(&max_overhead, &total_overhead, current_process, turnaround_time);

            printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n", simulation_time, 
                                        current_process->process_name, calculate_procs_remaining(ready_queue, input_queue));
            
            if (strcmp(memory_mode, INFINITE) != 0) {
                free_memory(memory, current_process);
            }

            free(current_process);
            current_process = NULL;
        }

        // adds all processes from the disk that have arrived to the input queue
        while(!is_empty(&disk_inputs)) {
            if (disk_inputs->process->time_arrived <= simulation_time) {
                // input queue empty
                if (is_empty(&input_queue)) {
                    input_queue = new_node(disk_inputs->process, 0);
                } else {
                    push(&input_queue, disk_inputs->process);
                }
                pop(&disk_inputs, 0);

            } else {
                break;
            }
        }

        // add the processes to memory, if possible
        if (strcmp(memory_mode, INFINITE) == 0) {
            while (!is_empty(&input_queue)) {
                if (is_empty(&ready_queue)) {
                    ready_queue = new_node(input_queue->process, input_queue->process->service_time);
                } else {
                    priority_push(&ready_queue, input_queue->process, input_queue->process->service_time);
                }
                pop(&input_queue, 0);
            }
        } else {
            process_node_t* input_queue_reader = input_queue;
            int current_pop_index = 0;

            while (!is_empty(&input_queue_reader)) {
                int start_hole_index = best_fit(memory, input_queue_reader->process);

                if (start_hole_index == -1) {
                    current_pop_index += 1;
                    input_queue_reader = input_queue_reader->next;
                } else {
                    // allocate memory to the current process
                    input_queue_reader->process->bit_map_index = start_hole_index;
                    fill_memory(memory, input_queue_reader->process);
                    printf("%d,READY,process_name=%s,assigned_at=%d\n", simulation_time, 
                            input_queue_reader->process->process_name, start_hole_index);

                    // add the process to the ready queue
                    if (is_empty(&ready_queue)) {
                        ready_queue = new_node(input_queue_reader->process, input_queue_reader->process->service_time);
                    } else {
                        priority_push(&ready_queue, input_queue_reader->process, input_queue_reader->process->service_time);
                    }

                    input_queue_reader = input_queue_reader->next;
                    pop(&input_queue, current_pop_index);
                }
            }
        }

        // change process only if current process is completed
        if (current_process == NULL && !is_empty(&ready_queue)) {
            current_process = ready_queue->process;
            pop(&ready_queue, 0);
        }

        if (transition_ready && current_process != NULL) {
            printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", simulation_time, 
                                            current_process->process_name, current_process->service_time);
            transition_ready = 0;
        }
    }

    output_statistics(simulation_time, max_overhead, total_overhead, total_turnaround_time, total_procs_completed);
}

void schedule_RR(process_node_t* disk_inputs, char* memory_mode, int quantum) {
    int simulation_time = -1 * quantum;
    process_node_t* ready_queue = NULL;
    process_node_t* input_queue = NULL;
    process_t* current_process = NULL;
    int transition_ready = 1;

    int total_procs_completed = 0;
    int total_turnaround_time = 0;
    float max_overhead = 0;
    float total_overhead = 0;

    int memory[MEMORY_SIZE];
    initialise_memory(memory);

     // keep going until the current process is done, and ready, input and the disk are empty
    while(current_process != NULL || !is_empty(&ready_queue) || !is_empty(&input_queue) || !is_empty(&disk_inputs)) {
        simulation_time += quantum;

        // checks if the process should be terminated and freed
        if (current_process != NULL && update_process_completion(current_process, quantum) == 1) {
            int turnaround_time = calculate_turnaround_time(current_process, simulation_time);

            transition_ready = 1;
            total_procs_completed++;
            total_turnaround_time += turnaround_time;
            update_overhead_values(&max_overhead, &total_overhead, current_process, turnaround_time);

            printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n", simulation_time, 
                                    current_process->process_name, calculate_procs_remaining(ready_queue, input_queue));
            
            if (strcmp(memory_mode, INFINITE) != 0) {
                free_memory(memory, current_process);
            }

            free(current_process);
            current_process = NULL;
        }

        // adds all processes from the disk that have arrived to the input queue
        while(!is_empty(&disk_inputs)) {
            if (disk_inputs->process->time_arrived <= simulation_time) {
                // input queue empty
                if (is_empty(&input_queue)) {
                    input_queue = new_node(disk_inputs->process, 0);
                } else {
                    push(&input_queue, disk_inputs->process);
                }
                pop(&disk_inputs, 0);

            } else {
                break;
            }
        }
        // add the processes to memory, if possible
        if (strcmp(memory_mode, INFINITE) == 0) {
            while (!is_empty(&input_queue)) {
                if (is_empty(&ready_queue)) {
                    ready_queue = new_node(input_queue->process, 0);
                } else {
                    push(&ready_queue, input_queue->process);
                }
                pop(&input_queue, 0);
            }
        } else {
            process_node_t* input_queue_reader = input_queue;
            int current_pop_index = 0;

            while (!is_empty(&input_queue_reader)) {
                int start_hole_index = best_fit(memory, input_queue_reader->process);

                if (start_hole_index == -1) {
                    current_pop_index += 1;
                    input_queue_reader = input_queue_reader->next;
                } else {
                    // allocate memory to the current process
                    input_queue_reader->process->bit_map_index = start_hole_index;
                    fill_memory(memory, input_queue_reader->process);
                    printf("%d,READY,process_name=%s,assigned_at=%d\n", simulation_time, 
                            input_queue_reader->process->process_name, start_hole_index);

                    // add the process to the ready queue
                    if (is_empty(&ready_queue)) {
                        ready_queue = new_node(input_queue_reader->process, 0);
                    } else {
                        push(&ready_queue, input_queue_reader->process);
                    }

                    input_queue_reader = input_queue_reader->next;
                    pop(&input_queue, current_pop_index);
                }
            }
        }

        // processes waiting in the ready queue
        if (!is_empty(&ready_queue)) {
            // push it to the tail if there is a currently running process
            if (current_process != NULL) {
                push(&ready_queue, current_process);
            }
            
            current_process = ready_queue->process;
            pop(&ready_queue, 0);
            transition_ready = 1;
        }

        if (transition_ready && current_process != NULL) {
            printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", simulation_time, 
                                            current_process->process_name, current_process->service_time);
            transition_ready = 0;
        }
    }
    output_statistics(simulation_time, max_overhead, total_overhead, total_turnaround_time, total_procs_completed);
}

void schedule(char* filename, char* scheduling_mode, char* memory_mode, int quantum) {
    // read processes from the file
    process_node_t* disk_inputs = read_processes(filename);

    if (strcmp(scheduling_mode, SJF) == 0) {
        schedule_SJF(disk_inputs, memory_mode, quantum);
    } else {
        schedule_RR(disk_inputs, memory_mode, quantum);
    }
}