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

int findCount(struct Process P[MAX_PROCESS_SIZE], int XYS[3]){
    int i, count = 0;

    for (i=0; i<XYS[1]; i++)
        count += P[i].countStartEnd;

    return count;
}

//TODO: don't delete first
// void printGanttChartPreemp(struct Process P[MAX_PROCESS_SIZE], int XYS[3], int count) {
//     int i, j, k, time = 0, found= 0, second = 0, index = 0, startEndIndex = 0, tempIndex = 0;
//     struct StartEndTime tempTimeLine[count];
 
//     printf("\nGantt Chart\n");

//     // first line
//     for (i=0; i<count; i++) 
//         printf("---------");
//     printf("\n|");

//     //processes
//     for (i=0; i<count; i++) {
//         found = 0;
//         for (j=0; j<XYS[1] && !found; j++) {
//             for (k=0; k<P[j].countStartEnd && !found; k++) {
//                 if (time == P[j].startEndPremp[k][0]) {
//                     found = 1;
//                     printf("P%-7d|", P[j].processID);
//                     time = P[j].startEndPremp[k][1];
//                     tempTimeLine[tempIndex].startTime = P[j].startEndPremp[k][0];
//                     tempTimeLine[tempIndex].endTime = P[j].startEndPremp[k][1];
//                     tempIndex++;
//                 }
//             }
//         }

//         // printf("\n%d %d %d\n", i, found, time);

//         // gap in front
//         if (time == 0 && !found) {
//             found = 1;
//             printf("-------|");    
//             printf("P%-7d|", P[0].processID);
//             time = P[0].startEndPremp[0][1];

//             tempTimeLine[tempIndex].startTime = P[0].startEndPremp[0][0];
//             tempTimeLine[tempIndex].endTime = P[0].startEndPremp[0][1];
//             tempIndex++;
//         }

//         // gap in between
//         if (!found){
//             second = P[100].currentExeTime;
//             // printf("\n%d\n", second);
//             for (j=0; j<XYS[1]; j++) {
//                 for (k=0; k<P[j].countStartEnd; k++) 
//                     if (P[j].startEndPremp[k][0] > time && P[j].startEndPremp[k][0] < second) {
//                         second = P[j].startEndPremp[k][0]; 
//                         index = j;
//                         startEndIndex = k;
//                     } 
//             }

//             printf("-------|");   
//             printf("P%-7d|", P[index].processID);
//             time = P[index].startEndPremp[startEndIndex][1];

//             tempTimeLine[tempIndex].startTime = P[index].startEndPremp[startEndIndex][0];
//             tempTimeLine[tempIndex].endTime = time;
//             tempIndex++;
//         }
//     }
//     printf("\n");
    
//     // last line
//     for (i=0; i<count; i++) 
//         printf("---------");
//     printf("\n");

//     // timeline
//     for (i=0; i<tempIndex; i++) {
//         if (i==0) {
//             printf("%-9d", 0);
//             if (tempTimeLine[i].startTime==0) 
//                 printf("%-9d", tempTimeLine[i].endTime);
//             else // gap in front
//                 printf("%-9d%-9d", tempTimeLine[i].startTime, tempTimeLine[i].endTime);
//         } else {
//             // gap in between
//             if (tempTimeLine[i].startTime != tempTimeLine[i-1].endTime) 
//                 printf("%-9d%-9d", tempTimeLine[i].startTime, tempTimeLine[i].endTime); 

//             else 
//                 printf("%-9d", tempTimeLine[i].endTime); 
//         }
//     }

//     printf("\n\n");
// }

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

//TODO: do not delete
// void roundRobbin(struct Process P[MAX_PROCESS_SIZE], int XYS[3]) {
//     int changei = 1; //not sure if needed but used for checking if index is changed
//     int total = 0;
//     int i, j, time, totalExe, countStartEnd, index; 

//     //for queue (initialize)
//     queue* q = init_queue();

//     // sort arrival time (using insertion sort)
//     arrangeProcessArrivalTimes(P, XYS);

//     //set i = 0 
//     i = 0;

//     // add the first process in the queue
//     enqueue(q, i);

//     //while i is less than the number of processes and while queue is not empty
//     while (i < XYS[1] && !isEmpty(q)) { 

//         index = dequeue(q);

//         // for gap and starting processes (changei is originally 1)
//         if (changei) {
//             time = P[i].arrivalTime; 
//             changei = 0;
//         }

//         //set the start time of the process    
//         countStartEnd = P[index].countStartEnd;
//         P[index].startEndPremp[countStartEnd][0] = time;

//         //if less than quantum time
//         if (P[index].currentExeTime <= XYS[2]) { 
//             time = time + P[index].currentExeTime;
//             P[index].currentExeTime = 0;
//         } else { //if greater than quantum time
//             time = time + XYS[2];
//             P[index].currentExeTime = P[index].currentExeTime - XYS[2];
//         }
//         P[index].startEndPremp[countStartEnd][1] = time;
//         P[index].countStartEnd++;

//         //while the succeeding arrival times are within the total time 
//         while ((i+1) < XYS[1] && P[i+1].arrivalTime <= time) { 
//             enqueue(q, (i+1));
//             i++;
//         }

//         //if total execution time is not yet 0, add it again to the queue
//         if(P[index].currentExeTime != 0) { 
//             enqueue(q, index);
//         } else {
//             //compute for the turn around time and waiting time
//             P[index].turnAroundTime = time - P[index].arrivalTime;
//             P[index].waitingTime = P[index].turnAroundTime - P[index].totalExeTime;
//         }

//         //if the processes are not yet finished (there is gap)
//         if (isEmpty(q) && i < (XYS[1]-1)) { 
//             i++;
//             changei = 1;
//             enqueue(q, i);
//         }
//     }

//     // printProcessesPreemp(P, XYS);
//     // printGanttChartPreemp(P, XYS, findCount(P,XYS)); 
// }

int isAllQueueEmpty (struct QueueProcess Q[MAX_PROCESS_SIZE], int XYS[3]) {
    int i, empty = 0;

    for (i=0; i<XYS[0]; i++) {
        if (isEmpty(Q[i].q)) {
            empty = 1;
        }
    }

    return empty;
}

void multilevelFeedbackQueue (struct QueueProcess Q[MAX_QUEUE_SIZE], struct Process P[MAX_PROCESS_SIZE], int XYS[3]) {
    int total = 0, changei = 1;
    int i, j, k, l, time, totalExe, found = 0, index, tempIndex, queueIndex, countStartEnd, IO;

    // sort arrival time of processes
    arrangeProcessArrivalTimes(P, XYS);

    // sort priority of queues
    arrangeProcessQueuePriority(Q, XYS);

    i = 0;
    j = 0;
    //add process with the earliest arrival time to the top priority queue
    enqueue(Q[0].q, i);

    while (i < XYS[1] && !isAllQueueEmpty(Q, XYS)) { 
        k = 0;
        IO = 0;
        l = 1; //start in the next queue

        //add priority boost (S - XYS[2]) (move all the jobs in the system to the topmost queue)
        //TODO:
        if (time >= XYS[2]) {
            //TODO:
            while (l < XYS[0]) { 
                while (!isEmpty(Q[l].q)) { 
                    tempIndex = dequeue(Q[l].q);
                    P[tempIndex].accumulatedCPU = 0;
                    enqueue(Q[0].q, tempIndex);
                }
                l++;
            }
        }

        //TODO: CHECK
        while (k < XYS[1]){
            if (P[k].startEnd[countStartEnd-1].IOQueue == 1 && P[k].startEnd[countStartEnd-1].endTime >= time) {
                if (P[k].accumulatedCPU >= Q[P[k].currentQueue].timeQuantum) {
                    enqueue(Q[P[k].currentQueue+1].q, k);
                    P[k].currentQueue += 1;
                    P[index].accumulatedCPU = 0;
                } else { 
                    enqueue(Q[P[k].currentQueue].q, k);
                }
            }
            k++;
        }

        // loop from the first priority queue to the last until a process is found
        while (j < XYS[0] && !found) { 
            if (!isEmpty(Q[j].q)) {
                index = dequeue(Q[j].q); 
                queueIndex = j;
                found = 1;
            }
            j++;
        }

        // FIXME: mamaya na yung gap uwu T_T
        // for gap and starting processes (changei is originally 1)
        if (changei) {
            time = P[index].arrivalTime; 
            changei = 0;
        }

        //run the process picked (BIG PART)
        //set the start time of the process  (FROM ROUND ROBIN ALGO)  
        countStartEnd = P[index].countStartEnd; //counts the number of rows for printing
        P[index].startEnd[countStartEnd].IOQueue = 0; // meaning the startend time is for the queue
        P[index].startEnd[countStartEnd].queueID = queueIndex; // store the queue id
        P[index].startEnd[countStartEnd].startTime = time;

        //if less than or equal to quantum time
        if (P[index].currentExeTime <= Q[queueIndex].timeQuantum) { 
            // if there is I/O burst
            if (P[index].IOBurstInterval < P[index].currentExeTime) {
                time += P[index].IOBurstInterval;
                P[index].currentExeTime -= P[index].IOBurstInterval;
                IO = 1;
            // there is no I/O burst
            } else {
                time += P[index].currentExeTime;
                P[index].currentExeTime = 0;
            }
            
        } else { //if greater than quantum time
            // if there is I/O burst
            if (P[index].IOBurstInterval < Q[queueIndex].timeQuantum) {
                time += P[index].IOBurstInterval;
                P[index].currentExeTime -= P[index].IOBurstInterval;
                IO = 1;
            // there is no I/O burst
            } else {
                time += Q[queueIndex].timeQuantum;
                P[index].currentExeTime -= Q[queueIndex].timeQuantum;
            }
        }

        //processed in the queue first before I/O
        P[index].accumulatedCPU += (time - P[index].startEnd[countStartEnd].startTime);
        P[index].startEnd[countStartEnd].endTime = time;
        P[index].countStartEnd++;

        // if there is an IO, compute for the start and end time for it
        if (IO) {
            countStartEnd = P[index].countStartEnd;
            P[index].startEnd[countStartEnd].IOQueue = 1; // meaning the startend time is for IO
            P[index].startEnd[countStartEnd].startTime = time;
            P[index].startEnd[countStartEnd].endTime = time + P[index].IOBurstLength;
            P[index].countStartEnd++;
        }

        //while the succeeding arrival times are within the total time, add it to the highest priority queue
        // FIXME: haven't double checked (COPY FROM RR)
        while ((i+1) < XYS[1] && P[i+1].arrivalTime <= time) { 
            enqueue(Q[0].q, (i+1));
            i++;
        }

        //if total execution time is not yet 0, add it again to one of the queues
        if(P[index].currentExeTime != 0) { 
            if (!IO) { 
                //if accumulated CPU reach the current queue demote the process to the next lowest
                if (P[index].accumulatedCPU >= Q[queueIndex].timeQuantum) {
                    enqueue(Q[queueIndex+1].q, index);
                    P[index].currentQueue = queueIndex+1;
                    P[index].accumulatedCPU = 0;
                }
                // else place it back to the current queue
                else {
                    enqueue(Q[queueIndex].q, index);
                }
            } 
        } else {
            // FIXME: haven't double checked (COPY FROM RR)
            //compute for the turn around time and waiting time
            P[index].turnAroundTime = time - P[index].arrivalTime;
            P[index].waitingTime = P[index].turnAroundTime - P[index].totalExeTime;
        }

        // FIXME: haven't double checked (COPY FROM RR)
        //if the processes are not yet finished (there is gap)
        if (!isAllQueueEmpty(Q, XYS) && i < (XYS[1]-1)) { 
            i++;
            changei = 1;
            // enqueue(q, i); #FIXME:
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
        }

        // Y less than the number of processes
        if (fscanf(inputFile,"%d",&temp)==1) {
            printf("Y is less than the number of process lines. Please rerun the program again.\n");
            exit(0);
        }

        // FIXME: check at the end if we need this
        processes[100].currentExeTime = 2147483647; // biggest value for int

        fclose(inputFile);
	} else {
		printf ("%s not found.\n", fileName);
        exit(0);
    }

    multilevelFeedbackQueue(listQueues, processes, XYS);
    
    return 0;
}