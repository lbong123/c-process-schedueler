#include "queue.h"

/*
    Completes the scheduling process.
*/
void schedule(char* filename, char* scheduling_mode, char* memory_mode, int quantum);

/*
    Performs the shortest job first scheduling algorithm.
*/
void schedule_SJF(process_node_t* disk_inputs, char* memory_mode, int quantum);

/*
    Performs the Round Robin scheduling algorithm.
*/
void schedule_RR(process_node_t* disk_inputs, char* memory_mode, int quantum);

/*
    Reads processes from the input file and adds them to an input queue.
*/
process_node_t* read_processes(char* filename);

/*
    Updates the process's service time remaining and sets it to null if it is completed.
*/
int update_process_completion(process_t* process, int quantum);

/*
    Initialises the memory bit array with 0s
*/
void initialise_memory(int* memory);

/*
    Finds the best whole for a process to go into.

    Returns either the index of the start of the hole or -1 if a valid hole cannot be found.
*/
int best_fit(int* memory, process_t* process);

/*
    Fills the memory bit map with 1's to allocate the process memory.
*/
void fill_memory(int* memory, process_t* process);

/*
    Frees the memory bit map with 0's to deallocate the process memory.
*/
void free_memory(int* memory, process_t* process);

/*
    Prints the performance statistics to the system output.
*/
void output_statistics(int simulation_time, float max_overhead, float total_overhead, 
                                                    int total_turnaround_time, int total_procs_completed);

/*
    Calculates the value of turnaround time given a recently completed process.
*/
int calculate_turnaround_time(process_t* completed_process, int simulation_time);

/*
    Modifies the value of the average and max overhead time given a recently completed process and its turnaround time;
*/
void update_overhead_values(float* max_overhead, float* average_overhead, process_t* completed_process, int proc_turnaround_time);