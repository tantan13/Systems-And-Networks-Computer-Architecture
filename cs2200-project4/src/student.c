
/*
 * student.c
 * Multithreaded OS Simulation for CS 2200
 *
 * This file contains the CPU scheduler for the simulation.
 */

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "os-sim.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/** Function prototypes **/
extern void idle(unsigned int cpu_id);
extern void preempt(unsigned int cpu_id);
extern void yield(unsigned int cpu_id);
extern void terminate(unsigned int cpu_id);
extern void wake_up(pcb_t *process);


/*
 * current[] is an array of pointers to the currently running processes.
 * There is one array element corresponding to each CPU in the simulation.
 *
 * current[] should be updated by schedule() each time a process is scheduled
 * on a CPU.  Since the current[] array is accessed by multiple threads, you
 * will need to use a mutex to protect it.  current_mutex has been provided
 * for your use.
 */
static pcb_t **current;
static pthread_mutex_t curr_mutex;
static pthread_cond_t thread_cond;
static pthread_mutex_t process_q;
static pcb_t* head;
static int rr_slice; // time slice for round robin
static unsigned int cpu_num; // number of cpus
static int scheduler; // the type of scheduling algorigthm:1.FIFO 2.RoundRobin 3.SRTF

void push(pcb_t* head);
pcb_t* pop(void);
pcb_t* ready_queue(void);

/*
*helper enqueue method to push process into the ready queue
*/

void push(pcb_t* process) {
    pthread_mutex_lock(&process_q);
    pcb_t* curr;
    process->next = NULL;
    if (head != NULL) {
        curr = head;
        while (curr->next != NULL) {
            curr = curr -> next;
        }
        curr -> next = process;
    } else {
        head = process;
    }
    pthread_mutex_unlock(&process_q);
    pthread_cond_signal(&thread_cond);
}

/* 
* helper method to dequeue 
*/
extern pcb_t* pop(void)
{
    pthread_mutex_lock(&process_q);
    pcb_t *curr;
    if (head != NULL){
        //pthread_mutex_lock(&process_q);
        curr = head;
        head = head->next;
        curr->next = NULL;
        //pthread_mutex_unlock(&process_q);
    } else {
        pthread_mutex_unlock(&process_q);
        return NULL;
    }
    pthread_mutex_unlock(&process_q);
    return curr;
}

pcb_t* ready_queue() {
    pthread_mutex_lock(&process_q);
    pcb_t* sjb;
    if (head != NULL) {
        sjb = head;
        pcb_t* curr = head;
        while (curr != NULL) {
            if (sjb -> time_remaining > curr -> time_remaining) {
                sjb = curr;
            }
            curr = curr -> next;
        }
    } else {
        pthread_mutex_unlock(&process_q);
        return NULL;
    }
    pcb_t *temp = head;
    if (sjb -> pid == head -> pid) {
        head = head->next;
    } else {
        while (temp->next->pid != sjb-> pid) {
            temp = temp->next;
        }
        temp -> next = sjb -> next;
    }
    pthread_mutex_unlock(&process_q);
    return sjb;
}

/*
 * schedule() is your CPU scheduler.  It should perform the following tasks:
 *
 *   1. Select and remove a runnable process from your ready queue which 
 *  you will have to implement with a linked list or something of the sort.
 *
 *   2. Set the process state to RUNNING
 *
 *   3. Set the currently running process using the current array
 *
 *   4. Call context_switch(), to tell the simulator which process to execute
 *      next on the CPU.  If no process is runnable, call context_switch()
 *      with a pointer to NULL to select the idle process.
 *
 *  The current array (see above) is how you access the currently running process indexed by the cpu id. 
 *  See above for full description.
 *  context_switch() is prototyped in os-sim.h. Look there for more information 
 *  about it and its parameters.
 */
static void schedule(unsigned int cpu_id)
{
    pcb_t* curr;
    if (scheduler == 2) {
        curr = ready_queue();
    } else {
        curr = pop();
    }
    if (curr == NULL) {
        context_switch(cpu_id, NULL, rr_slice);
    } else {
        curr -> state = PROCESS_RUNNING;
        pthread_mutex_lock(&curr_mutex);
        current[cpu_id] = curr;
        pthread_mutex_unlock(&curr_mutex);
        context_switch(cpu_id, curr, rr_slice);
    }
}


/*
 * idle() is your idle process.  It is called by the simulator when the idle
 * process is scheduled.
 *
 * This function should block until a process is added to your ready queue.
 * It should then call schedule() to select the process to run on the CPU.
 */
extern void idle(unsigned int cpu_id)
{
    /* FIX ME */
    pthread_mutex_lock(&process_q);
    while (head == NULL) {
        pthread_cond_wait(&thread_cond, &process_q);
    }
    pthread_mutex_unlock(&process_q);
    schedule(cpu_id);
}

    /*
     * REMOVE THE LINE BELOW AFTER IMPLEMENTING IDLE()
     *
     * idle() must block when the ready queue is empty, or else the CPU threads
     * will spin in a loop.  Until a ready queue is implemented, we'll put the
     * thread to sleep to keep it from consuming 100% of the CPU time.  Once
     * you implement a proper idle() function using a condition variable,
     * remove the call to mt_safe_usleep() below.
     */
    //mt_safe_usleep(1000000);


/*
 * preempt() is the handler called by the simulator when a process is
 * preempted due to its timeslice expiring.
 *
 * This function should place the currently running process back in the
 * ready queue, and call schedule() to select a new runnable process.
 *
 * Remember to set the status of the process to the proper value.
 */
extern void preempt(unsigned int cpu_id)
{
    /* FIX ME */
    pthread_mutex_lock(&curr_mutex);
    pcb_t* curr = current[cpu_id];
    curr -> state = PROCESS_READY;
    push(curr);
    pthread_mutex_unlock(&curr_mutex);
    schedule(cpu_id);
}


/*
 * yield() is the handler called by the simulator when a process yields the
 * CPU to perform an I/O request.
 *
 * It should mark the process as WAITING, then call schedule() to select
 * a new process for the CPU.
 */
extern void yield(unsigned int cpu_id)
{
    pthread_mutex_lock(&curr_mutex);
    current[cpu_id]->state = PROCESS_WAITING;
    pthread_mutex_unlock(&curr_mutex);
    schedule(cpu_id);
}


/*
 * terminate() is the handler called by the simulator when a process completes.
 * It should mark the process as terminated, then call schedule() to select
 * a new process for the CPU.
 */
extern void terminate(unsigned int cpu_id)
{
    /* FIX ME */
    pthread_mutex_lock(&curr_mutex);
    current[cpu_id] -> state = PROCESS_TERMINATED;
    pthread_mutex_unlock(&curr_mutex);
    schedule(cpu_id);
}


/*
 * wake_up() is the handler called by the simulator when a process's I/O
 * request completes.  It should perform the following tasks:
 *
 *   1. Mark the process as READY, and insert it into the ready queue.
 *
 *   2. If the scheduling algorithm is SRTF, wake_up() may need
 *      to preempt the CPU with the highest remaining time left to allow it to
 *      execute the process which just woke up.  However, if any CPU is
 *      currently running idle, or all of the CPUs are running processes
 *      with a lower remaining time left than the one which just woke up, wake_up()
 *      should not preempt any CPUs.
 *  To preempt a process, use force_preempt(). Look in os-sim.h for 
 *  its prototype and the parameters it takes in.
 */
extern void wake_up(pcb_t *process)
{
    /* FIX ME */
    process->state = PROCESS_READY;
    push(process);
    
    if (scheduler == 2) {
        unsigned int max;
        unsigned int preempt_id;
        unsigned int longestTime = 0;

        pthread_mutex_lock(&curr_mutex);
        for (max = 0; max < cpu_num; max++) {
            if (current[max] == NULL) {
                pthread_mutex_unlock(&curr_mutex);
                return;
            } else {
                if (current[max]->time_remaining > longestTime) {
                    longestTime = current[max]->time_remaining;
                    preempt_id = max;
                }
            }
        }
        pthread_mutex_unlock(&curr_mutex);
        
        if (longestTime > process->time_remaining) { 
            force_preempt(preempt_id);
        }
    }
}


/*
 * main() simply parses command line arguments, then calls start_simulator().
 * You will need to modify it to support the -r and -s command-line parameters.
 */
int main(int argc, char *argv[])
{
    unsigned int cpu_count;


    /*
     * Check here if the number of arguments provided is valid.
     * You will need to modify this when you add more arguments.
     */
    if (argc > 4 || argc < 2)
    {
        fprintf(stderr, "CS 2200 Project 4 -- Multithreaded OS Simulator\n"
            "Usage: ./os-sim <# CPUs> [ -r <time slice> | -s ]\n"
            "    Default : FIFO Scheduler\n"
            "         -r : Round-Robin Scheduler\n"
            "         -s : Shortest Remaining Time First Scheduler\n\n");
        return -1;
    }

    /* Parse the command line arguments */
    cpu_count = strtoul(argv[1], NULL, 0);

    /* FIX ME - Add support for -r and -s parameters*/
    char quantum;
    scheduler = 0;
    rr_slice = -1;

    if (argc > 2) {
        quantum = *((argv[2]) + 1);
        if (quantum == 'r') {
            scheduler = 1;
            rr_slice = strtoul(argv[3], NULL, 0);
        }
        else if (quantum == 's') {
            scheduler = 2;
        }
    }
    
    /* Allocate the current[] array and its mutex */
    current = malloc(sizeof(pcb_t*) * cpu_count);
    assert(current != NULL);
    pthread_mutex_init(&curr_mutex, NULL);

    /* Start the simulator in the library */
    start_simulator(cpu_count);

    return 0;
}


#pragma GCC diagnostic pop
