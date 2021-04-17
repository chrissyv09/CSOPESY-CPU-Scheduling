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

struct Process {
    int processID;
    int arrivalTime;
    int totalExeTime;
    int turnAroundTime;
    int waitingTime;
    int startTime; 
    int endTime;
    int startEndPremp[300][2];
    int countStartEnd;
    int currentExeTime;
};

struct StartEndTime {
    int startTime;
    int endTime;
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

int findCount(struct Process P[MAX_PROCESS_SIZE], int XYZ[3]){
    int i, count = 0;

    for (i=0; i<XYZ[1]; i++)
        count += P[i].countStartEnd;

    return count;
}

// works only for non-premptive
void printGanttChart(struct Process P[MAX_PROCESS_SIZE], int XYZ[3]) {
    int i, j;
    
    printf("\nGantt Chart\n");

    // first line
    for (i=0; i<XYZ[1]; i++) 
        printf("---------");
    printf("\n|");

    // processes
    for (i=0; i<XYZ[1]; i++) {
        if (i==0) {
            // gap in the front
            if (P[i].startTime!=0) 
                printf("-------|");             
        } else {
            // gap in between
            if (P[i].startTime != P[i-1].endTime)
                printf("-------|");
        }
        printf("P%-7d|", P[i].processID);
    }
    printf("\n");

    // last line
    for (i=0; i<XYZ[1]; i++) 
        printf("---------");
    printf("\n");

    // timeline
    for (i=0; i<XYZ[1]; i++) {
        if (i==0) {
            printf("%-9d", 0);
            if (P[i].startTime==0) 
                printf("%-9d", P[i].endTime);
            else // gap in front
                printf("%-9d%-9d", P[i].startTime, P[i].endTime);
        } else {
            // gap in between
            if (P[i].startTime != P[i-1].endTime) 
                printf("%-9d%-9d", P[i].startTime, P[i].endTime); 

            else 
                printf("%-9d", P[i].endTime); 
        }
    }
    printf("\n\n");
}

void printGanttChartPreemp(struct Process P[MAX_PROCESS_SIZE], int XYZ[3], int count) {
    int i, j, k, time = 0, found= 0, second = 0, index = 0, startEndIndex = 0, tempIndex = 0;
    struct StartEndTime tempTimeLine[count];
 
    printf("\nGantt Chart\n");

    // first line
    for (i=0; i<count; i++) 
        printf("---------");
    printf("\n|");

    //processes
    for (i=0; i<count; i++) {
        found = 0;
        for (j=0; j<XYZ[1] && !found; j++) {
            for (k=0; k<P[j].countStartEnd && !found; k++) {
                if (time == P[j].startEndPremp[k][0]) {
                    found = 1;
                    printf("P%-7d|", P[j].processID);
                    time = P[j].startEndPremp[k][1];
                    tempTimeLine[tempIndex].startTime = P[j].startEndPremp[k][0];
                    tempTimeLine[tempIndex].endTime = P[j].startEndPremp[k][1];
                    tempIndex++;
                }
            }
        }

        // printf("\n%d %d %d\n", i, found, time);

        // gap in front
        if (time == 0 && !found) {
            found = 1;
            printf("-------|");    
            printf("P%-7d|", P[0].processID);
            time = P[0].startEndPremp[0][1];

            tempTimeLine[tempIndex].startTime = P[0].startEndPremp[0][0];
            tempTimeLine[tempIndex].endTime = P[0].startEndPremp[0][1];
            tempIndex++;
        }

        // gap in between
        if (!found){
            second = P[100].currentExeTime;
            // printf("\n%d\n", second);
            for (j=0; j<XYZ[1]; j++) {
                for (k=0; k<P[j].countStartEnd; k++) 
                    if (P[j].startEndPremp[k][0] > time && P[j].startEndPremp[k][0] < second) {
                        second = P[j].startEndPremp[k][0]; 
                        index = j;
                        startEndIndex = k;
                    } 
            }

            printf("-------|");   
            printf("P%-7d|", P[index].processID);
            time = P[index].startEndPremp[startEndIndex][1];

            tempTimeLine[tempIndex].startTime = P[index].startEndPremp[startEndIndex][0];
            tempTimeLine[tempIndex].endTime = time;
            tempIndex++;
        }
    }
    printf("\n");
    
    // last line
    for (i=0; i<count; i++) 
        printf("---------");
    printf("\n");

    // timeline
    for (i=0; i<tempIndex; i++) {
        if (i==0) {
            printf("%-9d", 0);
            if (tempTimeLine[i].startTime==0) 
                printf("%-9d", tempTimeLine[i].endTime);
            else // gap in front
                printf("%-9d%-9d", tempTimeLine[i].startTime, tempTimeLine[i].endTime);
        } else {
            // gap in between
            if (tempTimeLine[i].startTime != tempTimeLine[i-1].endTime) 
                printf("%-9d%-9d", tempTimeLine[i].startTime, tempTimeLine[i].endTime); 

            else 
                printf("%-9d", tempTimeLine[i].endTime); 
        }
    }

    printf("\n\n");
}

void printProcesses(struct Process P[MAX_PROCESS_SIZE], int XYZ[3]) {
    int i, sumWait = 0;

    printf("\n");
    for (i=0; i<XYZ[1]; i++) {
        printf("P[%d]\nStart Time: %d End time: %d\n", P[i].processID, P[i].startTime, P[i].endTime);
        printf("Waiting time: %d\n", P[i].waitingTime);
        printf("Turnaround time: %d\n", P[i].turnAroundTime);
        printf("************************************\n");

        sumWait += P[i].waitingTime;
    }

    printf("Average waiting time: %f\n\n", 1.0 * sumWait / XYZ[1]);
}

void printProcessesPreemp(struct Process P[MAX_PROCESS_SIZE], int XYZ[3]) {
    int i, j, sumWait = 0;

    printf("\n");
    for (i=0; i<XYZ[1]; i++) {
        printf("P[%d]\n", P[i].processID);

        for (j=0; j<P[i].countStartEnd; j++) 
            printf("Start Time: %d End time: %d\n",  P[i].startEndPremp[j][0], P[i].startEndPremp[j][1]);

        printf("Waiting time: %d\n", P[i].waitingTime);
        printf("Turnaround time: %d\n", P[i].turnAroundTime);
        printf("************************************\n");

        sumWait += P[i].waitingTime;
    }

    printf("Average waiting time: %f\n\n", 1.0 * sumWait / XYZ[1]);
}

void arrangeProcessArrivalTimes(struct Process P[MAX_PROCESS_SIZE], int XYZ[3]) {
    int i, j;
    struct Process key;
     
    for (i=1; i<XYZ[1]; i++) {
        key = P[i];
        j = i-1;
        while (j > -1 && P[j].arrivalTime > key.arrivalTime) {
            P[j+1] = P[j];
            j--;
        }
        P[j+1] = key;
    }
}

void firstComeFirstServe(struct Process P[MAX_PROCESS_SIZE], int XYZ[3]) {
    int i, time;

    // sort arrival time (using insertion sort)
    arrangeProcessArrivalTimes(P, XYZ);

    // calculate turnaround time, waiting time, start time, end time
    time = 0;
    for (i=0; i<XYZ[1]; i++) { 
        // computes start and end time
        if (time >= P[i].arrivalTime) {
            P[i].startTime = time;     
            time += (P[i].totalExeTime); 
        } else {
            P[i].startTime = P[i].arrivalTime;
            time = P[i].arrivalTime + P[i].totalExeTime; 
        }  
        P[i].endTime =  P[i].startTime + P[i].totalExeTime;
        
        // computes turnaround time
        P[i].turnAroundTime = time - P[i].arrivalTime;

        // waiting time
        if (i == 0) 
            P[i].waitingTime = 0;
        else {
            P[i].waitingTime = P[i].turnAroundTime - P[i].totalExeTime;
        }
    }

    printProcesses(P, XYZ);   
    printGanttChart(P, XYZ); 
}

void nonPreemptiveShortestJobFirst(struct Process P[MAX_PROCESS_SIZE], int XYZ[3]) {
    int i, j, lowIndex, time, found;
    struct Process temp;

    // sort arrival time (using insertion sort)
    arrangeProcessArrivalTimes(P, XYZ);

    //sort burst time
    time = 0;
    i = 0;
    while (i < XYZ[1]) {
        found = 0;

        // find lowest index
        j=i;
        lowIndex = j;

        while (j < XYZ[1] && time >= P[j].arrivalTime) {
            found = 1;

            if (P[lowIndex].totalExeTime > P[j].totalExeTime) {
                lowIndex = j;
            }
           j++;
        }

        if (!found) {
            time++;
        } else {
            temp = P[i];
            P[i] = P[lowIndex];
            P[lowIndex] = temp;

            P[i].startTime = time;
            time += P[i].totalExeTime;
            P[i].endTime = time;

            P[i].turnAroundTime = P[i].endTime - P[i].arrivalTime;

            // waiting time
            if (i == 0) 
                P[i].waitingTime = 0;
            else {
                P[i].waitingTime = P[i].turnAroundTime - P[i].totalExeTime;
            }

            i++;
        }
    }

    printProcesses(P, XYZ);
    printGanttChart(P, XYZ); 
}

void preemptiveShortestJobFirst(struct Process P[MAX_PROCESS_SIZE], int XYZ[3]) {
    int time, i, j, lowIndex, pastLowIndex = -1, countStartEnd, found, start = 0; 

    // sort arrival time (using insertion sort)
    arrangeProcessArrivalTimes(P, XYZ);

    i = 0;
    time = 0;
    for (time = 0; i < XYZ[1]; time++) {
        lowIndex = 100; //process with the biggest current execution time
        found = 0;

        j = 0;
        //find the lowest burst time at current time
        while (j < XYZ[1] && time >= P[j].arrivalTime) {
            found = 1;

            if (P[lowIndex].currentExeTime > P[j].currentExeTime && P[j].currentExeTime > 0) {
                lowIndex = j;
            }
            j++;
        }

        if (found) {
            countStartEnd = P[lowIndex].countStartEnd;

            // check if it is still the same process after time++
            if (lowIndex == pastLowIndex) {
                P[lowIndex].startEndPremp[countStartEnd][1]++;                      // increment end time since old process
            } else {
                // 
                if (start && P[pastLowIndex].currentExeTime > 0) 
                    P[pastLowIndex].countStartEnd++;                    // so that it would not increment when pastLowIndex is still -1 (increments the past low index since new process)
                P[lowIndex].startEndPremp[countStartEnd][0] = time;     // set start time since new process
                P[lowIndex].startEndPremp[countStartEnd][1] = time + 1;     // set end time since new process
            }        
            
            //updating the execution time left
            P[lowIndex].currentExeTime--;
            pastLowIndex = lowIndex;
            if (!start)
                start = 1;      // signifies that CPU starts processing (there is pastLowIndex already)

            //compute waiting time and turnaround time if no more execution time left 
            if(P[lowIndex].currentExeTime == 0) {
                i++;
                countStartEnd = P[lowIndex].countStartEnd;
                P[lowIndex].turnAroundTime = P[lowIndex].startEndPremp[countStartEnd][1] - P[lowIndex].arrivalTime;
                P[lowIndex].waitingTime = P[lowIndex].turnAroundTime - P[lowIndex].totalExeTime;
                P[lowIndex].countStartEnd++;    
            }
        }
    }

    printProcessesPreemp(P, XYZ);
    printGanttChartPreemp(P, XYZ, findCount(P,XYZ)); 
}

void roundRobbin(struct Process P[MAX_PROCESS_SIZE], int XYZ[3]) {
    int changei = 1; //not sure if needed but used for checking if index is changed
    int total = 0;
    int i, j, time, totalExe, countStartEnd, index; 

    //for queue (initialize)
    queue* q = init_queue();

    // sort arrival time (using insertion sort)
    arrangeProcessArrivalTimes(P, XYZ);

    //set i = 0 
    i = 0;

    // add the first process in the queue
    enqueue(q, i);

    //while i is less than the number of processes and while queue is not empty
    while (i < XYZ[1] && !isEmpty(q)) { 

        index = dequeue(q);

        // for gap and starting processes (changei is originally 1)
        if (changei) {
            time = P[i].arrivalTime; 
            changei = 0;
        }

        //set the start time of the process    
        countStartEnd = P[index].countStartEnd;
        P[index].startEndPremp[countStartEnd][0] = time;

        //if less than quantum time
        if (P[index].currentExeTime <= XYZ[2]) { 
            time = time + P[index].currentExeTime;
            P[index].currentExeTime = 0;
        } else { //if greater than quantum time
            time = time + XYZ[2];
            P[index].currentExeTime = P[index].currentExeTime - XYZ[2];
        }
        P[index].startEndPremp[countStartEnd][1] = time;
        P[index].countStartEnd++;

        //while the succeeding arrival times are within the total time 
        while ((i+1) < XYZ[1] && P[i+1].arrivalTime <= time) { 
            enqueue(q, (i+1));
            i++;
        }

        //if total execution time is not yet 0, add it again to the queue
        if(P[index].currentExeTime != 0) { 
            enqueue(q, index);
        } else {
            //compute for the turn around time and waiting time
            P[index].turnAroundTime = time - P[index].arrivalTime;
            P[index].waitingTime = P[index].turnAroundTime - P[index].totalExeTime;
        }

        //if the processes are not yet finished (there is gap)
        if (isEmpty(q) && i < (XYZ[1]-1)) { 
            i++;
            changei = 1;
            enqueue(q, i);
        }
    }

    printProcessesPreemp(P, XYZ);
    printGanttChartPreemp(P, XYZ, findCount(P,XYZ)); 
}

int main () { 
    char fileName[100];
    int XYZ[3];
    int i, temp;
    struct Process processes[MAX_PROCESS_SIZE];
    FILE *inputFile;
    
    // ask input filename from user
    printf("Enter filename: ");
    scanf("%s", fileName);

    //try this
    inputFile = fopen(fileName,"rt");

    if (inputFile != NULL) {
        // get first line (X, Y, Z)
        fscanf(inputFile,"%d",&XYZ[0]);
        fscanf(inputFile,"%d",&XYZ[1]);
        fscanf(inputFile,"%d",&XYZ[2]);

        //error checking
        if (XYZ[0] < 0 || XYZ[0] > 3 || XYZ[1] < 3 || XYZ[1] > 100 || XYZ[2] < 1 || XYZ[2] > 100) { 
            printf("Invalid input for X, Y and Z. Please rerun the program again.\n");
            exit(0);
        } 

        if ((XYZ[0] >= 0 && XYZ[0] < 3) && XYZ[2] != 1) {
            printf("Time quantum should be set to 1 for CPU scheduling that are not Round Robin.\n");
            exit(0);
        }

        // loop all processes XYZ[1]=Y
        for (i=0; i<XYZ[1]; i++) {
            processes[i].processID = -1;
            processes[i].arrivalTime = -1;
            processes[i].totalExeTime = -1;
            
            fscanf(inputFile,"%d",&processes[i].processID);
            fscanf(inputFile,"%d",&processes[i].arrivalTime);
            fscanf(inputFile,"%d",&processes[i].totalExeTime);

            if (processes[i].processID < 0 || processes[i].arrivalTime < 0 || processes[i].totalExeTime < 0) { 
                printf("Invalid input for processes. Please rerun the program again.\n");
                exit(0);
            }

            processes[i].currentExeTime = processes[i].totalExeTime;
            processes[i].countStartEnd = 0;
        }

        // Y less than the number of processes
        if (fscanf(inputFile,"%d",&temp)==1) {
            printf("Y is less than the number of process lines. Please rerun the program again.\n");
            exit(0);
        }

        processes[100].currentExeTime = 2147483647; // biggest value for int

        fclose(inputFile);
	} else {
		printf ("%s not found.\n", fileName);
        exit(0);
    }

    switch (XYZ[0]) {
        // FCFS
        case 0:
            XYZ[2] = 1;
            firstComeFirstServe(processes, XYZ);
            break;
        // NSJF
        case 1:
            XYZ[2] = 1;
            nonPreemptiveShortestJobFirst(processes, XYZ);
            break;
        // PSJF
        case 2:
            XYZ[2] = 1;
            preemptiveShortestJobFirst(processes, XYZ);
            break;
        // RR 
        case 3:
            roundRobbin(processes, XYZ);
            break;
    }
    
    return 0;
}