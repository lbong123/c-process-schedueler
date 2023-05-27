#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "queue.h"

int compare_processes(process_t* p1, process_t* p2) {
   if (p1->time_arrived < p2->time_arrived) {
      return -1;
   } else if (p1->time_arrived > p2->time_arrived) {
      return 1;
   }
   return strcmp(p1->process_name, p2->process_name);
}

process_node_t* new_node(process_t* process, int priority) {
   process_node_t* temp = (process_node_t*) malloc(sizeof(process_node_t));

   temp->process = process;
   temp->priority = priority;
   temp->next = NULL;
   return temp;
}

process_t* peek(process_node_t** head) {
   return (*head)->process;
}

void print_queue(process_node_t* head) {
      process_node_t* printing_queue = head;
      printf("printing queue... \n");

      while (!is_empty(&printing_queue)) {
         process_t* curr = peek(&printing_queue);
         printf("%d %s %d %d\n", curr->time_arrived, curr->process_name, curr->service_time, curr->memory_requirement);
         printing_queue = printing_queue->next;
      }
}


void pop(process_node_t** head, int index) {
   process_node_t* curr = *head;
   process_node_t* prev = NULL;

   int i;

   for (i = 0; i < index; i++) {
      if (curr->next == NULL) {
         break;
      }

      prev = curr;
      curr =  curr->next;
   }

   if (i == 0) {
      (*head) = (*head)->next;
   } else if (i == index) {
      prev->next = curr->next;
   } else {
      printf("invalid pop attempted\n");
      return;
   }

   free(curr);
   curr = NULL;
}

void push(process_node_t** head, process_t* new_process) {
   process_node_t* start = (*head);
   process_node_t* temp = new_node(new_process, 0);

   while (start->next != NULL) {
      start = start->next;
   }

   start->next = temp;
}

void priority_push(process_node_t** head, process_t* new_process, int new_priority) {
   process_node_t* start = (*head);
   process_node_t* temp = new_node(new_process, new_priority);
   
   if ((*head)->priority > new_priority 
            || ((*head)->priority == new_priority && compare_processes(new_process, (*head)->process) < 0)) {    
      temp->next = *head;
      (*head) = temp;
   } 
   else {
      while (start->next != NULL && start->next->priority <= new_priority) {
         // if the priorities are equal and the new process comes second comparatively
         if (start->next->priority == new_priority && compare_processes(new_process, start->next->process) < 0) {
            break;
         }
         start = start->next;
      }

      // at the ends of the list or at a suitable spot
      temp->next = start->next;
      start->next = temp;
   }
}

int is_empty(process_node_t** head) {
   return (*head) == NULL;
}