/*************************************************************** 
Names: Tan, Ivy
       Vinco, Christianne 
Group: 17
Section: S15
***************************************************************/
#include <stdio.h>
#include <stdlib.h>

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

typdef struct node
{
 struct Process process; 
 struct node *next;    
} node;

typedef struct queue {
    int count; 
    node *head; 
    node *tail;
} queue; 

//initialize the head and tail to null
void init_queue (queue *q) {
    q->count = 0;
    q->head = NULL; 
    q->tail = NULL; 
}

int isEmpty (queue *q) { 
    return (q->tail == NULL);
} 

void enqueue (queue *q, struct Process P) { 
    node *Node = malloc(sizeof(node));
    newnode->process = p; 
    newnode->next = NULL;

    if (!isEmpty(q)) { 
        q->tail->next = Node;
        q->tail = Node;
    } else { 
        q->head = q->tail = Node;
    } 
    q->count++;
}

struct Process dequeue(queue *q) { 
    node *Node = malloc(sizeof(node));
    struct Process P = q->head->process;
    Node = q->head;
    q->head = q->head->next;
    q->count--;

    //free memory
    free(Node);
    return P;
} 


int MAX_PROCESS_SIZE = 101;

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

        for (j=0; j<=P[i].countStartEnd; j++) 
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
            }
        }
    }

    printProcessesPreemp(P, XYZ);
}

void roundRobbin(struct Process P[MAX_PROCESS_SIZE], int XYZ[3]) {
    int changei = 0; //not sure if needed but used for checking if index is changed
    int total = 0;
    int i, j, time, totalExe; 
    struct Process process;

    //for queue
    queue *q;
    q = malloc(sizeof(queue));

    // sort arrival time (using insertion sort)
    arrangeProcessArrivalTimes(P, XYZ);

    //set i = 0 
    i = 0;

    //set time to the arrival time whether starting time is a 0 or with skip
    enqueue(q, P[i]);
    time = P[i].arrivalTime;

    //while i is less than the number of processes and while queue is not empty
    while (i < XYZ[1] && !isEmpty(q)) { 

        //check if queue is not yet empty
        if (!isEmpty(q)) { 
            process = dequeue(q);
            changei = 0;
        } else { 
            i++; 
            process = P[i];
            changei = 1;
            time = P[i].arrivalTime; 
        }
        

        //set the start time of the process      
        process.startEndPrempt[countStartEnd][0] = time;

        //if less than quantum time
        if (process.currentExeTime <= XYZ[3]) { 
            time = time + process.currentExeTime;
            process.currentExeTime = 0;
        } else { //if greater than quantum time
            time = time + XYZ[3];
            process.currentExeTime = process.currentExeTime - XYZ[3];
        }
        process.startEndPrempt[countStartEnd][1] = time;
        process.countStartEnd++;

        //while the succeeding arrival times are within the total time 
        while (P[i+1].arrivalTime <= time && i < XYZ[1]) { 
            enqueue(q, P[i + 1]);
            i++;
        }

        //if total execution time is not yet 0, add it again to the queue
        if(process.currentExeTime != 0) { 
            enqueue(q, process);
        } else {
            //compute for the turn around time and waiting time
            process.turnAroundTime = time - process.arrivalTime;
            process.waitingTime = process.turnAroundTime - process.totalExeTime;
        }

        //if the processes are not yet finished
        if (changei && i < XYZ[1]) { 
            i++;
        }
    }
}

int main () { 
    char fileName[100];
    int XYZ[3];
    int i;
    struct Process processes[MAX_PROCESS_SIZE];
    FILE *inputFile;
    
    // ask input filename from user
    printf("Enter filename: ");
    scanf("%s", fileName);

    inputFile = fopen(fileName,"rt");

    if (inputFile != NULL) {
        // get first line (X, Y, Z)
        fscanf(inputFile,"%d",&XYZ[0]);
        fscanf(inputFile,"%d",&XYZ[1]);
        fscanf(inputFile,"%d",&XYZ[2]);

        // loop all processes XYZ[1]=Y
        for (i=0; i<XYZ[1]; i++) {
            fscanf(inputFile,"%d",&processes[i].processID);
            fscanf(inputFile,"%d",&processes[i].arrivalTime);
            fscanf(inputFile,"%d",&processes[i].totalExeTime);

            processes[i].currentExeTime = processes[i].totalExeTime;
            processes[i].countStartEnd = 0;
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