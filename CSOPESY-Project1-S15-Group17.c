/*************************************************************** 
Names: Tan, Ivy
       Vinco, Christianne 
Group: 17
Section: S15
***************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

int MAX_PROCESS_SIZE = 101;
int MAX_QUEUE_SIZE = 5;

struct StartEndTime {
    int startTime;
    int endTime;
    int IOQueue; // 1 or 0 1-> 1/O. 0 -> queue
    int queueID;
};

struct Process {
    int processID;
    int arrivalTime;
    int totalExeTime;
    int IOBurstLength;
    int IOBurstInterval;
    int turnAroundTime;
    int waitingTime;
    struct StartEndTime startEnd[1000];
    int countStartEnd;
    int currentExeTime;
    int currentQueue;
    int accumulatedCPU;
    int outside;
};

typedef struct node {
    int index;
    struct node *next;    
} node;

typedef struct queue {
    int count; 
    node *head; 
    node *tail;
} queue; 

struct QueueProcess {
    queue* q;
    int queueID;
    int priority;
    int timeQuantum;
};

//initialize the head and tail to null
queue* init_queue () {
    struct queue* q = (struct queue*)malloc(sizeof(struct queue));
    q->count = 0;
    q->head = NULL; 
    q->tail = NULL; 

    return q;
}

int isEmpty (queue *q) { 
    return (q->tail == NULL);
} 

void enqueue (queue *q, int index) { 
    node *Node = malloc(sizeof(node));
    Node->index = index; 
    Node->next = NULL;

    if (!isEmpty(q)) { 
        q->tail->next = Node;
        q->tail = Node;
    } else { 
        q->head = q->tail = Node;
    } 
    q->count++;
}

int dequeue(queue *q) { 

    node *Node = malloc(sizeof(node));
    Node = q->head;
    int index = q->head->index;
    
    q->head = q->head->next;
    q->count--;

    if (q->head == NULL)
        q->tail = NULL;

    //free memory
    free(Node);
    return index;
} 

int queue_head(queue *q){

    if (q->head == NULL) {
        return -1;
    } else {
        return q->head->index;
    }
}

void printProcesses(struct Process P[MAX_PROCESS_SIZE], int XYS[3]) {
    int i, j, sumWait = 0;

    printf("\n");
    for (i=0; i<XYS[1]; i++) {
        printf("P[%d]\n", P[i].processID);

        for (j=0; j<P[i].countStartEnd; j++) {
            //queue
            if (P[i].startEnd[j].IOQueue == 0)
                printf("Q[%d] ", P[i].startEnd[j].queueID);
            else
                printf("[IO] ");
            printf("Start Time: %d End time: %d\n",  P[i].startEnd[j].startTime, P[i].startEnd[j].endTime);
        }

        printf("Waiting time: %d\n", P[i].waitingTime);
        printf("Turnaround time: %d\n", P[i].turnAroundTime);
        // FIXME: to be removed
        printf("Current Exe time: %d\n", P[i].currentExeTime);
        printf("************************************\n");

        sumWait += P[i].waitingTime;
    }

    printf("Average waiting time: %f\n\n", 1.0 * sumWait / XYS[1]);
}

void arrangeProcessArrivalTimes(struct Process P[MAX_PROCESS_SIZE], int XYS[3]) {
    int i, j;
    struct Process key;
     
    for (i=1; i<XYS[1]; i++) {
        key = P[i];
        j = i-1;
        while (j > -1 && P[j].arrivalTime > key.arrivalTime) {
            P[j+1] = P[j];
            j--;
        }
        P[j+1] = key;
    }
}

void arrangeProcessQueuePriority(struct QueueProcess Q[MAX_QUEUE_SIZE], int XYS[3]) {
    int i, j;
    struct QueueProcess key;
     
    for (i=1; i<XYS[0]; i++) {
        key = Q[i];
        j = i-1;
        while (j > -1 && Q[j].priority > key.priority) {
            Q[j+1] = Q[j];
            j--;
        }
        Q[j+1] = key;
    }
}

int isAllQueueEmpty (struct QueueProcess Q[MAX_PROCESS_SIZE], int XYS[3]) {
    int i, empty = 1; // assuming all queues are empty

    for (i=0; i<XYS[0]; i++) {
        if (!isEmpty(Q[i].q)) {
            empty = 0; // found a queue that is not empty
        }
    }

    return empty;
}

void multilevelFeedbackQueue (struct QueueProcess Q[MAX_QUEUE_SIZE], struct Process P[MAX_PROCESS_SIZE], int XYS[3]) {
    int total = 0, changei = 1;
    int i, j, k, l, time, found = 0, index = 0, tempIndex, queueIndex = 0, countStartEnd, tempS = XYS[2], running = 0, pastIndex, currentCPUTime;

    // sort arrival time of processes
    arrangeProcessArrivalTimes(P, XYS);

    // sort priority of queues
    arrangeProcessQueuePriority(Q, XYS);

    i = 0;
    time = 0;
    //add process with the earliest arrival time to the top priority queue
    enqueue(Q[0].q, i);

    for (time=0;(i < XYS[1] && !isAllQueueEmpty(Q, XYS));time++) { 
        k = 0;
        l = 1; //start in the next queue
        found = 0; //finding the next process to execute
        j = 0;

        //TODO: CHECK
        while (k < XYS[1]){
            countStartEnd = P[k].countStartEnd;
            if (countStartEnd != 0 && P[k].startEnd[countStartEnd-1].IOQueue == 1 && P[k].startEnd[countStartEnd-1].endTime <= time && P[k].outside) {
                printf("hi: %d k: %d time: %d\n", P[k].currentQueue, k, time);
                if (P[k].accumulatedCPU >= Q[P[k].currentQueue].timeQuantum) {
                    enqueue(Q[P[k].currentQueue+1].q, k);
                    P[k].currentQueue += 1;
                    P[index].accumulatedCPU = 0;
                } else { 
                    enqueue(Q[P[k].currentQueue].q, k);
                }

                if (P[k].currentQueue < P[index].currentQueue)
                    dequeue(Q[P[index].currentQueue].q);

                P[k].outside = 0;
            }
            k++;
        }
        
        //add priority boost (S - XYS[2]) (move all the jobs in the system to the topmost queue)
        if (time >= 10) {
            printf("time: %d huhuh: %d %d\n", time, pastIndex, index);
        }
        //TODO:
        if (time >= tempS && running && pastIndex != index) {
            while (l < XYS[0]) { 
                while (!isEmpty(Q[l].q)) { 
                    // do not priority boost if the process is in the I/O
                    if (!P[k].outside) {
                        printf("time: %d\n", time);
                        tempIndex = dequeue(Q[l].q);
                        enqueue(Q[0].q, tempIndex);
                        P[tempIndex].accumulatedCPU = 0;
                        P[tempIndex].currentQueue = 0;
                    }
                }
                l++;
            }

            tempS += XYS[2]; // added this kasi every S yung priority boost
        }

        // loop from the first priority queue to the last until a process is found
        while (j < XYS[0] && !found) { 
            printf("4\n");
            if (!isEmpty(Q[j].q)) {
                index = queue_head(Q[j].q);     // get the top of the head only (don't remove from the queue)
                queueIndex = j;
                found = 1;
                printf("2: %d time: %d\n", index, time);
            }
            j++;
        }

        tempIndex = 100;
        // gap for when there are processes in I/O pa
        if (isAllQueueEmpty(Q, XYS) && time){
            for (j=0; j<XYS[1]; j++) {
                if (P[j].currentExeTime != 0 && P[tempIndex].startEnd[P[tempIndex].countStartEnd-1].endTime > P[j].startEnd[P[j].countStartEnd-1].endTime && P[j].outside) {
                    tempIndex = i;
                }
            }

            index = tempIndex;
            time = P[tempIndex].startEnd[P[tempIndex].countStartEnd-1].endTime;
        }

        // for gap and starting processes (changei is originally 1)
        if (changei) {
            time = P[index].arrivalTime; 
            changei = 0;
        }

        //run the process picked (BIG PART)¯¯
        countStartEnd = P[index].countStartEnd; //counts the number of rows for printing

        if (index == pastIndex){
            P[index].startEnd[countStartEnd].endTime++;
        } else {
            if (running && P[pastIndex].currentExeTime > 0 && !P[pastIndex].outside)
                P[pastIndex].countStartEnd++;

            P[index].startEnd[countStartEnd].IOQueue = 0; // meaning the startend time is for the queue
            P[index].startEnd[countStartEnd].queueID = queueIndex; // store the queue id
            P[index].startEnd[countStartEnd].startTime = time;
            P[index].startEnd[countStartEnd].endTime = time + 1;
        }

        printf("3\n");

        P[index].currentExeTime--;
        P[index].accumulatedCPU++;
        pastIndex = index;

        if (!running)
            running = 1;

        currentCPUTime = P[index].startEnd[countStartEnd].endTime - P[index].startEnd[countStartEnd].startTime;
        // if there is I/O, enter this statement
        if (P[index].IOBurstInterval > 0 && currentCPUTime >= P[index].IOBurstInterval && currentCPUTime < Q[queueIndex].timeQuantum) {
            P[index].outside = 1;
            dequeue(Q[queueIndex].q);
            P[index].countStartEnd++;

            // computes for the start and end time for it
            countStartEnd = P[index].countStartEnd;
            P[index].startEnd[countStartEnd].IOQueue = 1; // meaning the startend time is for IO
            P[index].startEnd[countStartEnd].startTime =  P[index].startEnd[countStartEnd-1].endTime;
            P[index].startEnd[countStartEnd].endTime = P[index].startEnd[countStartEnd].startTime + P[index].IOBurstLength;
            P[index].countStartEnd++;
        }

        printf("5\n");

        //while the succeeding arrival times are within the total time, add it to the highest priority queue
        while ((i+1) < XYS[1] && P[i+1].arrivalTime <= time) { 
            enqueue(Q[0].q, (i+1));
            i++;
        }

        printf("6\n");

        //if total execution time is not yet 0, add it again to one of the queues
        if(P[index].currentExeTime != 0) { 
            if (!P[index].outside) { 
                //if accumulated CPU reach the current queue demote the process to the next lowest
                if (P[index].accumulatedCPU >= Q[queueIndex].timeQuantum) {
                    dequeue(Q[queueIndex].q);
                    // add this so it would not go lower than the available queues
                        if (queueIndex + 1 >= XYS[0]) {
                            enqueue(Q[queueIndex].q, index);
                        } else { 
                            enqueue(Q[queueIndex+1].q, index);
                            P[index].currentQueue = queueIndex+1;
                        }
                    
                    P[index].accumulatedCPU = 0;
                }
            } 
        } else {
            //compute for the turn around time and waiting time
            // FIXME: recompute since there are I/O and other queue time
            P[index].turnAroundTime = time - P[index].arrivalTime;
            P[index].waitingTime = P[index].turnAroundTime - P[index].totalExeTime;
        }

        printf("8\n");
        // FIXME: haven't double checked (COPY FROM RR)
        //if the processes are not yet finished (there is a gap)
        // if (!isAllQueueEmpty(Q, XYS) && i < (XYS[1]-1)) { 
        //     i++;
        //     changei = 1;
            // enqueue(q, i); #FIXME:
        // }
        if (time % 15 == 0) {
            printProcesses(P, XYS);
        }
    }

    printProcesses(P, XYS);
}


int main () { 
    char fileName[100];
    int XYS[3];
    int i, temp;
    struct Process processes[MAX_PROCESS_SIZE];
    struct QueueProcess listQueues[MAX_QUEUE_SIZE];
    FILE *inputFile;
    
    // ask input filename from user
    printf("Enter filename: ");
    scanf("%s", fileName);

    //try this
    inputFile = fopen(fileName,"rt");

    if (inputFile != NULL) {
        // get first line (X, Y, S) X - number of queues Y - number of processes S - time for priority boost
        fscanf(inputFile,"%d %d %d",&XYS[0], &XYS[1], &XYS[2]);

        //error checking
        if (XYS[0] < 2  || XYS[0] > 5 || XYS[1] < 3 || XYS[1] > 100 || XYS[2] < 0) { 
            printf("Invalid input for X, Y and S. Please rerun the program again.\n");
            exit(0);
        }

        // loop all queues XYS[0]=X
        for (i=0; i<XYS[0]; i++) {
            listQueues[i].queueID = -1;
            listQueues[i].priority = -1;
            listQueues[i].timeQuantum = -1;
            
            fscanf(inputFile,"%d",&listQueues[i].queueID);
            fscanf(inputFile,"%d",&listQueues[i].priority);
            fscanf(inputFile,"%d",&listQueues[i].timeQuantum);

            //error checking
            if (listQueues[i].queueID < 0 || listQueues[i].priority < 0 || listQueues[i].timeQuantum < 1) { 
                printf("Invalid input for queues. Please rerun the program again.\n");
                exit(0);
            }

            listQueues[i].q = init_queue();
        }
        
        //loop all processes XYS[1] = Y
        for (i=0; i<XYS[1]; i++) {
            processes[i].processID = -1;
            processes[i].arrivalTime = -1;
            processes[i].totalExeTime = -1;
            processes[i].IOBurstLength = -1;
            processes[i].IOBurstInterval = -1;
            
            fscanf(inputFile,"%d",&processes[i].processID);
            fscanf(inputFile,"%d",&processes[i].arrivalTime);
            fscanf(inputFile,"%d",&processes[i].totalExeTime);
            fscanf(inputFile,"%d",&processes[i].IOBurstLength);
            fscanf(inputFile,"%d",&processes[i].IOBurstInterval);

            //error checking
            if (processes[i].processID < 0 || processes[i].arrivalTime < 0 || processes[i].totalExeTime < 1 || processes[i].IOBurstLength < 0 || processes[i].IOBurstInterval < 0) { 
                printf("Invalid input for processes. Please rerun the program again.\n");
                exit(0);
            }

            processes[i].currentExeTime = processes[i].totalExeTime;
            processes[i].countStartEnd = 0;
            processes[i].accumulatedCPU = 0;
            processes[i].currentQueue = 0;
            processes[i].outside = 0;
        }

        processes[100].startEnd[processes[100].countStartEnd].endTime = 2147483647;
        processes[100].countStartEnd++;

        // Y less than the number of processes
        if (fscanf(inputFile,"%d",&temp)==1) {
            printf("Y is less than the number of process lines. Please rerun the program again.\n");
            exit(0);
        }

        fclose(inputFile);
	} else {
		printf ("%s not found.\n", fileName);
        exit(0);
    }

    multilevelFeedbackQueue(listQueues, processes, XYS);
    
    return 0;
}