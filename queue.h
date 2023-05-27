#define MAX_FILENAME_SIZE 9

typedef struct process {
    int time_arrived;
    char process_name[MAX_FILENAME_SIZE];
    int service_time;
    int original_service_time;
    int memory_requirement;
    int bit_map_index;

} process_t;

typedef struct process_node process_node_t;

struct process_node {
    process_t* process;
    int priority;
    process_node_t* next;
};

/*
    Instantiates a new process node and returns its pointer.
*/
process_node_t* new_node(process_t* process, int priority);

/*
    Finds the process pointer of the head of the queue.
*/
process_t* peek(process_node_t** head);

/*
    Prints the queue to the the system output.
*/
void print_queue(process_node_t* head);

/*
    Pops the head of the queue
*/
void pop(process_node_t** head, int index);

/*
    Return value < 0 then it indicates process1 is less than process2.

    if Return value > 0 then it indicates process1 is greater than process2.

*/
int compare_processes(process_t* p1, process_t* p2);

/*
    Inserts the new process into the queue based on priority.
*/
void priority_push(process_node_t** head, process_t* new_process, int new_priority);

/*
    Inserts the new process to the back of the queue.
*/
void push(process_node_t** head, process_t* new_process);

/* 
    Checks if the queue is empty.
*/
int is_empty(process_node_t** head);