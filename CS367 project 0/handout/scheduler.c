/* Fill in your Name and GNumber in the following two comment fields
 * Name:
 * GNumber:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clock.h"
#include "structs.h"
#include "constants.h"
#include "scheduler.h"

/* Called when the program terminates.
 * You may use this to clean up memory or for any other purpose.
 */
void scheduler_exit(Schedule *schedule) {
  scheduler_free(schedule);
  return;
}

/* Initialize the Schedule Struct
 * Follow the specification for this function.
 * Returns a pointer to the new Schedule or NULL on any error.
 */
Schedule *scheduler_init() {
  
  Schedule *schedule = (Schedule *) malloc(sizeof(Schedule));

  if (schedule == NULL) {
    return NULL;
  }
  
  List *ready_list = (List *) malloc(sizeof(List));

  if (ready_list == NULL) {
    return NULL;
  }
  ready_list->head = NULL;
  ready_list->count = 0;

  List *stopped_list = (List *) malloc(sizeof(List));
  if (stopped_list == NULL) {
    return NULL;
  }
  stopped_list->head = NULL;
  stopped_list->count = 0;

  List *defunct_list = (List *) malloc(sizeof(List));
  if (defunct_list == NULL) {
    return NULL;
  }
  defunct_list->head = NULL;
  defunct_list->count = 0;

  schedule->ready_list = ready_list;
  schedule->stopped_list = stopped_list;
  schedule->defunct_list = defunct_list;

  return schedule;
}

int insert_into_list(List *lst, Process *process) {
    if (lst != NULL) {        
        Process *tmp = lst->head;  
        Process *prev = NULL;   
        int done = 0;  
        // Adding the process in ascending order of pid
        while (tmp != NULL) {
          if(tmp->pid > process->pid) {
            if(prev == NULL) {
              process->next = tmp;
              lst->head = process;
              done = 1;
              break;  
            } else {
              prev->next = process;
              process->next = tmp;
              done = 1;
              break;
            }
          }
          prev = tmp;
          tmp = tmp->next;
        }

        if(done == 0) {
          if (prev != NULL) { // Reached at the end
            prev->next = process;
          } else { // List is empty
            lst->head = process;
          }
        }
        lst->count += 1;
        return 0;
    } else {
      return -1;
    }
}

/* Add the process into the appropriate linked list.
 * Follow the specification for this function.
 * Returns a 0 on success or a -1 on any error.
 */
int scheduler_add(Schedule *schedule, Process *process) {
 if (process == NULL || schedule == NULL) {
    return -1;
  }
  if ((process->flags & STATE_CREATED) == STATE_CREATED) {
    int mask = 1 << 0; 
    process->flags  = (process->flags & ~mask) | ((0 << 0) & mask);
    process->flags = process->flags | STATE_READY;

    List *ready_list = schedule->ready_list;
    return insert_into_list(ready_list, process);

  } else if ((process->flags & STATE_READY) == STATE_READY) {
    if (process->time_remaining > 0) {
      List *ready_list = schedule->ready_list;
      return insert_into_list(ready_list, process);
    } else if (process->time_remaining == 0){
      int mask = 1 << 1; 
      process->flags  = (process->flags & ~mask) | ((0 << 1) & mask);
      process->flags = process->flags | STATE_DEFUNCT;
      List *defunct_list = schedule->defunct_list;
      return insert_into_list(defunct_list, process);
    } else if ((process->flags & STATE_DEFUNCT) == 1) {
      List *defunct_list = schedule->defunct_list;
      return insert_into_list(defunct_list, process);
    }
  }

  return -1;
}

/* Move the process with matching pid from Ready to Stopped.
 * Change its State to Stopped.
 * Follow the specification for this function.
 * Returns a 0 on success or a -1 on any error.
 */
int scheduler_stop(Schedule *schedule, int pid) {

  if (schedule != NULL) {
    List *ready_list = schedule->ready_list;
    Process *tmp = ready_list->head;
    Process *prev = NULL;
    Process *p = NULL;
    while (tmp != NULL) {
      if (tmp->pid == pid) {
        p = tmp;
        if (prev == NULL) {
          ready_list->head = tmp->next;
        } else {
          prev->next = tmp->next;
        }
        p->next = NULL;
        ready_list->count -= 1;
        break;
      }
      prev = tmp;
      tmp = tmp->next;
    }
    if (p != NULL) {
      for (int i=0;i<5;i++) {
        int mask = 1 << i; 
        p->flags  = (p->flags & ~mask) | ((0 << i) & mask);
      }
      p->flags = p->flags | STATE_STOPPED;
      List *stopped_list = schedule->stopped_list;
      return insert_into_list(stopped_list, p);
    } 
  }


  return -1;
}

/* Move the process with matching pid from Stopped to Ready.
 * Change its State to Ready.
 * Follow the specification for this function.
 * Returns a 0 on success or a -1 on any error.
 */
int scheduler_continue(Schedule *schedule, int pid) {

  if (schedule != NULL) {
    List *stopped_list = schedule->stopped_list;
    Process *tmp = stopped_list->head;
    Process *prev = NULL;
    Process *p = NULL;
    while (tmp != NULL) {
      if (tmp->pid == pid) {
        p = tmp;
        if (prev == NULL) {
          stopped_list->head = tmp->next;
        } else {
          prev->next = tmp->next;
        }
        p->next = NULL;
        stopped_list->count -= 1;
        break;
      }
      prev = tmp;
      tmp = tmp->next;
    }
    if (p != NULL) {
      for (int i=0;i<5;i++) {
        int mask = 1 << i; 
        p->flags  = (p->flags & ~mask) | ((0 << i) & mask);
      }
      p->flags = p->flags | STATE_READY;
      List *ready_list = schedule->ready_list;
      return insert_into_list(ready_list, p);
    } 
  }

  return -1;
}

/* Remove the process with matching pid from Defunct.
 * Follow the specification for this function.
 * Returns its exit code (from flags) on success or a -1 on any error.
 */
int scheduler_reap(Schedule *schedule, int pid) {
  if (schedule != NULL) {
    List *defunct_list = schedule->defunct_list;
    Process *tmp = defunct_list->head;
    Process *prev = NULL;
    Process *p = NULL;
    while (tmp != NULL) {
      if (tmp->pid == pid) {
        p = tmp;
        if (prev == NULL) {
          defunct_list->head = tmp->next;
        } else {
          prev->next = tmp->next;
        }
        p->next = NULL;
        defunct_list->count -= 1;
        break;
      }
      prev = tmp;
      tmp = tmp->next;
    }
    if (p != NULL) {
      for (int i=0;i<5;i++) {
        int mask = 1 << i; 
        p->flags  = (p->flags & ~mask) | ((0 << i) & mask);
      }
      p->flags = p->flags | STATE_TERMINATED;
      unsigned int exit_code = p->flags;
      exit_code = exit_code >> 6;

      Process *head = p;
      Process *tmp;
      while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp->command);
        free(tmp);
      }
      free(p);
      return exit_code;
    }
  }

  return -1;
}

/* Create a new Process with the given information.
 * - Malloc and copy the command string, don't just assign it!
 * Set the STATE_CREATED flag.
 * If is_sudo, also set the PF_SUPERPRIV flag.
 * Follow the specification for this function.
 * Returns the Process on success or a NULL on any error.
 */
Process *scheduler_generate(char *command, int pid, int time_remaining, int is_sudo) {
  
  Process *process = (Process *) malloc(sizeof(Process));

  if (process == NULL) {
    return NULL;
  }
  process->command = malloc (40* sizeof (char));
  strcpy(process->command, command);
  process->pid = pid;
  process->time_remaining = time_remaining;
  process->time_last_run = clock_get_time();
  process->next = NULL;
  
  process->flags = 0x0 | STATE_CREATED;
  if (is_sudo == 1) {
    process->flags = PF_SUPERPRIV | STATE_CREATED;
  }

  return process;
}

/* Select the next process to run from Ready List.
 * Follow the specification for this function.
 * Returns the process selected or NULL if none available or on any errors.
 */
Process *scheduler_select(Schedule *schedule) {

  if (schedule != NULL) {
    if (schedule->ready_list != NULL) {
      
      List *ready_list = schedule->ready_list;
      Process *p = NULL;
      int lowestTimeRemaining = 0;
      int lowestPid = 0;
      Process *tmp = ready_list->head;
      if (tmp != NULL) {
        lowestTimeRemaining = tmp->time_remaining;
        lowestPid = tmp->pid;
      }
      
      while(tmp != NULL) {
        if(tmp->time_remaining <=lowestTimeRemaining) {
          lowestTimeRemaining = tmp->time_remaining;
          if (tmp->pid <= lowestPid) {
            lowestPid = tmp->pid;
            p = tmp;
          }
        }
        tmp = tmp->next;
      }

      tmp = ready_list->head;
      while(tmp != NULL) {
        if ((clock_get_time() - tmp->time_last_run) >= TIME_STARVATION) {
          lowestPid = tmp->pid;
          p = tmp;
          break;
        }
        tmp = tmp->next;
      }

      if (p != NULL) {
          tmp = ready_list->head;
          Process *prev = NULL;
          Process *selected = NULL;
          while (tmp != NULL) {
            if (tmp->pid == lowestPid) {
              selected = tmp;
              if (prev == NULL) {
                ready_list->head = tmp->next;
              } else {
                prev->next = tmp->next;
              }
              selected->next = NULL;
              ready_list->count -= 1;
              break;
            }
            prev = tmp;
            tmp = tmp->next;
         }
         return selected;
      }
    }
  }
  return NULL;
}

/* Returns the number of items in a given List
 * Follow the specification for this function.
 * Returns the count of the List, or -1 on any errors.
 */
int scheduler_count(List *ll) {
  if (ll != NULL) {
    return ll->count;
  }
  return -1;
}

/* Completely frees all allocated memory in the scheduler
 * Follow the specification for this function.
 */
void scheduler_free(Schedule *scheduler) {
  if (scheduler != NULL) {
    if (scheduler->ready_list != NULL) {

      if(scheduler->ready_list->head != NULL) {
        Process *head = scheduler->ready_list->head;
        Process *tmp;
        while (head != NULL) {
          tmp = head;
          head = head->next;
          if (tmp->command != NULL) {
          	free(tmp->command);
	  }
          if (tmp != NULL) {
          	free(tmp);
	  }
        }
      }
      if (scheduler->ready_list != NULL) {	
      	free(scheduler->ready_list);
      }
      //scheduler->ready_list = NULL;
    }

    if (scheduler->stopped_list != NULL) {

      if(scheduler->stopped_list->head != NULL) {
        Process *head = scheduler->stopped_list->head;
        Process *tmp;
        while (head != NULL) {
          tmp = head;
          head = head->next;
	  if (tmp->command != NULL) {	
          	free(tmp->command);
	  }
          if (tmp != NULL) {
          	free(tmp);
	  }
        }
      }
      
      if (scheduler->stopped_list != NULL) {
      	free(scheduler->stopped_list);
      }
      //scheduler->ready_list = NULL;
    }

    if (scheduler->defunct_list != NULL) {

      if(scheduler->defunct_list->head != NULL) {
        Process *head = scheduler->defunct_list->head;
        Process *tmp;
        while (head != NULL) {
          tmp = head;
          head = head->next;
          if (tmp->command != NULL) {
	  	free(tmp->command);
	  }
          if (tmp != NULL) {
          	free(tmp);
	  }
        }
      }

      if (scheduler->defunct_list != NULL) {
      		free(scheduler->defunct_list);
      }
      //scheduler->ready_list = NULL;
    }

    free(scheduler);
    //scheduler = NULL;
  }
  return;
}

/*
int main(int argc, char const *argv[])
{
  Schedule *s = scheduler_init();
  if (s == NULL) {
    printf("%s\n", "Error");
  }
  char *t = "test";
  Process *p = scheduler_generate(t, 1, 1000, 1);
  if (p == NULL) {
    printf("%s\n", "Error");
  }
  int i = 13;
  i = i << 6;
  scheduler_add(s, p);
  p->flags = i;
  int result = scheduler_reap(s, 1);
  printf("Result: %d\n", result);
  result = scheduler_stop(s, 1);
  printf("Result: %d\n", result);
  result = scheduler_continue(s, 1);
  printf("Result: %d\n", result);
  printf("%d %d %d\n", p->time_last_run, p->flags, s->ready_list->count);
  scheduler_free(s);

  return 0;
}
//*/
