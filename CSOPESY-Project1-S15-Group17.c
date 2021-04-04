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

        for (j=0; j<P[i].countStartEnd; j++) 
            printf("Start Time: %d End time: %d\n",  P[i].startEndPremp[j][0], P[i].startEndPremp[j][1]);

        printf("Waiting time: %d\n", P[i].waitingTime);
        printf("Turnaround time: %d\n", P[i].turnAroundTime);
        printf("************************************\n");

        sumWait += P[i].waitingTime;
    }

    printf("Average waiting time: %f\n\n", 1.0 * sumWait / XYZ[1]);
}

int isEmpty(struct Process Queue) { 

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
                P[lowIndex].countStartEnd++;            // para consistent sa printing
            }
        }
    }

    printProcessesPreemp(P, XYZ);
}

void roundRobbin(struct Process P[MAX_PROCESS_SIZE], struct Process queue[MAX_PROCESS_SIZE], int XYZ[3]) {
    int changei = 0; //not sure if needed but used for checking if index is changed
    int total = 0;
    int i, j, time, totalExe; 
    int totalExeCopy[XYZ[1]];
    struct Process process;

    // sort arrival time (using insertion sort)
    arrangeProcessArrivalTimes(P, XYZ);

    //copy the burst time in another array
    for (i = 0; i < XYZ[1], i++) { 
        totalExeCopy[i] = P[i].totalExeTime;
    }

    //set i = 0 
    i = 0

    //while i is less than the number of processes and while queue is not empty
    while (i < XYZ[1] && !isEmpty(queue)) { 
        if (i == 0) { 
            //set time to the arrival time whether starting time is a 0 or with skip
            time = P[i].arrivalTime;
            process = P[i];
        } else { 
            //check if queue is not yet empty
            if (!isEmpty(queue)) { 
                process = dequeue(queue);
                changei = 0;
            } else { 
                process = P[i];
                changei = 1;
            }
        }

        //set the start time of the process 
        
        process.startEndPrempt[countStartEnd][0] = time;

        //if less than quantum time
        if (process.totalExeTime <= XYZ[3]) { 
            time = time + process.totalExeTime;
            process.totalExeTime = 0;
        } else { //if greater than quantum time
            time = time + XYZ[3];
            process.totalExeTime = process.totalExeTime - XYZ[3];
        }
        process.startEndPrempt[countStartEnd][1] = time;
        process.countStartEnd++;

        //while the succeeding arrival times are within the total time 
        while (P[i+1].arrivalTime <= time) { 
            enqueue(P[i + 1]);
            i++;
        }

        //if total execution time is not yet 0, add it again to the queue
        if(process.totalExeTime != 0) { 
            enqueue(process);
        } else {
            process.endTime = time;
            //else compute for the turnaround time and waiting time
            for (j=0; j<XYZ[1]; j++) { 
                if (process.processID == P[j].processID) { 
                    totalExe = totalExeCopy[j];
                    break;
                }
            }
            //compute for the turn around time and waiting time
            process.turnAroundTime = process.endTime - process.arrivalTime;
            process.waitingTime = process.turnAroundTime - totalExe;
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
    struct Process queue[MAX_PROCESS_SIZE];
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
	}
	else {
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
            roundRobbin(processes, queue, XYZ);
            break;
    }
    
    return 0;
}